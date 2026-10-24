#include "SceneView.hpp"

#include <QDebug>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>

#include <gsl/narrow>

constexpr int windowWeight = 1076;
constexpr int windowHeight = 768;
constexpr BoundingBox defaultBounds{ .min{ 0.0, 0.0 }, .max{ 1.0, 1.0 } };
// Empty space around the fitted scene, as a fraction of its extent, per side.
constexpr double viewMargin = 0.05;
// Extent shown when the scene has neither width nor height (e.g. a single point).
constexpr double minimumViewExtent = 1.0;

SceneView::SceneView(Workspace& workspace)
    : m_workspace{ workspace }
{
    setMouseTracking(true);
}

QSize SceneView::sizeHint() const { return QSize{ windowWeight, windowHeight }; }

void SceneView::updateWorkspace()
{
    const BoundingBox sceneBounds{ boundingBox(m_workspace).value_or(defaultBounds) };
    m_sceneBounds = sceneBounds;

    std::vector<float> vertices;

    m_lines.vertexCount = 0;
    for (const Layer& layer : m_workspace.layers()) {
        if (layer.visible) {
            for (const Line& line : layer.lines) {
                vertices.push_back(gsl::narrow_cast<float>(line.start.x));
                vertices.push_back(gsl::narrow_cast<float>(line.start.y));
                vertices.push_back(gsl::narrow_cast<float>(line.end.x));
                vertices.push_back(gsl::narrow_cast<float>(line.end.y));
                m_lines.vertexCount += 2;
            }
        }
    }

    m_lines.vao.bind();
    m_lines.vbo.bind();
    m_lines.vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

    m_lines.vbo.allocate(vertices.data(), gsl::narrow<int>(vertices.size() * sizeof(float)));

    m_program->enableAttributeArray("position");
    m_program->setAttributeBuffer("position", GL_FLOAT, 0, m_lines.vertexCount);

    m_lines.vbo.release();
    m_lines.vao.release();
    m_program->release();

    update();
    resetViewToSceneBounds();
}

void SceneView::initializeGL()
{
    initializeOpenGLFunctions();

    static const char* vertexShaderSource =
        R"(#version 330 core
        layout(location = 0) in vec2 position;
        uniform mat4 u_mvp;

        void main() {
            gl_Position = u_mvp * vec4(position, 0.0, 1.0);
        })";

    static const char* fragmentShaderSource =
        R"(#version 330 core
        out vec4 fragColor;
        void main() {
            fragColor = vec4(1.0, 0.0, 0.0, 1.0);  // Red color
        })";

    m_program = std::make_unique<QOpenGLShaderProgram>();
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();

    m_lines.vao.create();
    m_lines.vbo.create();

    m_program->release();

    updateWorkspace();
}

void SceneView::resizeGL(int /* width */, int /* height */) { resetViewToSceneBounds(); }

void SceneView::paintGL()
{
    glClearColor(0.1F, 0.1F, 0.1F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);

    const QMatrix4x4 mvp{ m_projMatrix * m_viewMatrix };

    m_program->bind();
    m_program->setUniformValue("u_mvp", mvp);

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_lines.vao);

    glDrawArrays(m_lines.primitiveType, 0, m_lines.vertexCount);

    m_program->release();
}

void SceneView::wheelEvent(QWheelEvent* /* event */)
{
    //    // Step 1: Get mouse position in widget coordinates
    //    QPointF mousePos = event->position();
    //
    //    // Step 2: Convert to world coordinates BEFORE zoom
    //    QMatrix4x4 inverseOldTransform = (m_projMatrix * m_viewMatrix).inverted();
    //    QVector4D before = inverseOldTransform * QVector4D(mousePos.x(), mousePos.y(), 0, 1);
    //    before /= before.w();
    //
    //    // Step 3: Apply zoom change
    //    float delta = event->angleDelta().y() / 120.0f;   // mouse wheel notches
    //    float factor = std::pow(1.1f, delta);
    //    m_zoom *= factor;
    //
    //    // Step 4: Update view matrix
    //    updateTransform();
    //
    //    // Step 5: Convert to world coordinates AFTER zoom
    //    QMatrix4x4 inverseNewTransform = (m_projMatrix * m_viewMatrix).inverted();
    //    QVector4D after = inverseNewTransform * QVector4D(mousePos.x(), mousePos.y(), 0, 1);
    //    after /= after.w();
    //
    //    // Step 6: Compute difference and adjust pan to keep cursor fixed
    //    QVector2D worldShift = QVector2D(after - before);
    //    m_pan -= QPointF(worldShift.x(), worldShift.y());
    //
    //    // Step 7: Update again with adjusted pan
    //    updateTransform();
    //    update();
}

void SceneView::mousePressEvent(QMouseEvent* /* event */) {}

void SceneView::mouseMoveEvent(QMouseEvent* event)
{
    const QSize widgetSize{ size() };
    const QPointF eventPos{ event->position() };

    // Map widget pixels (y down) to normalized device coordinates (y up) and back
    // through the inverse of the transformation used for rendering.
    const QVector4D ndc{ gsl::narrow_cast<float>(2.0 * eventPos.x() / widgetSize.width() - 1.0),
                         gsl::narrow_cast<float>(1.0 - 2.0 * eventPos.y() / widgetSize.height()),
                         0.0F,
                         1.0F };
    const QVector4D world{ (m_projMatrix * m_viewMatrix).inverted() * ndc };

    const QPointF currentMousePosition{ static_cast<double>(world.x()),
                                        static_cast<double>(world.y()) };
    emit mousePositionChanged(currentMousePosition);
}

void SceneView::updateTransform()
{
    m_viewMatrix.setToIdentity();
    m_viewMatrix.translate(gsl::narrow_cast<float>(m_pan.x()), gsl::narrow_cast<float>(m_pan.y()));
    m_viewMatrix.scale(m_zoom, m_zoom);
}

void SceneView::resetViewToSceneBounds()
{
    const int widgetWidth{ width() };
    const int widgetHeight{ height() };
    if (widgetWidth <= 0 || widgetHeight <= 0) {
        return;
    }

    const qreal pixelRatio{ devicePixelRatio() };
    glViewport(
        0,
        0,
        gsl::narrow_cast<int>(widgetWidth * pixelRatio),
        gsl::narrow_cast<int>(widgetHeight * pixelRatio)
    );

    double bbWidth{ getWidth(m_sceneBounds) };
    double bbHeight{ getHeight(m_sceneBounds) };
    if (bbWidth == 0.0 && bbHeight == 0.0) {
        bbWidth = minimumViewExtent;
        bbHeight = minimumViewExtent;
    }

    // Fit the limiting dimension and extend the other one to the widget's aspect
    // ratio. Cross-multiplying avoids dividing by a zero extent.
    double viewWidth{};
    double viewHeight{};
    if (bbWidth * widgetHeight >= bbHeight * widgetWidth) {
        viewWidth = bbWidth * (1.0 + 2.0 * viewMargin);
        viewHeight = viewWidth * widgetHeight / widgetWidth;
    } else {
        viewHeight = bbHeight * (1.0 + 2.0 * viewMargin);
        viewWidth = viewHeight * widgetWidth / widgetHeight;
    }

    const Coordinate center{ .x = (m_sceneBounds.min.x + m_sceneBounds.max.x) / 2.0,
                             .y = (m_sceneBounds.min.y + m_sceneBounds.max.y) / 2.0 };

    m_projMatrix.setToIdentity();
    m_projMatrix.ortho(
        gsl::narrow_cast<float>(center.x - viewWidth / 2.0),
        gsl::narrow_cast<float>(center.x + viewWidth / 2.0),
        gsl::narrow_cast<float>(center.y - viewHeight / 2.0),
        gsl::narrow_cast<float>(center.y + viewHeight / 2.0),
        -1.0F,
        1.0F
    );
}
