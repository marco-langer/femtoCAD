#include "SceneView.hpp"

#include <QDebug>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>

constexpr int windowWeight = 1076;
constexpr int windowHeight = 768;
constexpr BoundingBox defaultBounds{ .min{ 0.0, 0.0 }, .max{ 1.0, 1.0 } };

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
                vertices.push_back(static_cast<float>(line.start.x));
                vertices.push_back(static_cast<float>(line.start.y));
                vertices.push_back(static_cast<float>(line.end.x));
                vertices.push_back(static_cast<float>(line.end.y));
                m_lines.vertexCount += 2;
            }
        }
    }

    m_lines.vao.bind();
    m_lines.vbo.bind();
    m_lines.vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

    m_lines.vbo.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(float)));

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
    const QSize windowSize{ size() };
    const QPoint& eventPos{ event->pos() };

    const double x{ m_sceneBounds.min.x
                    + static_cast<double>(eventPos.x()) / windowSize.width()
                          * getWidth(m_sceneBounds) };
    const double y{ m_sceneBounds.min.y
                    + static_cast<double>(eventPos.y()) / windowSize.height()
                          * getHeight(m_sceneBounds) };

    const QPointF currentMousePosition{ x, y };
    emit mousePositionChanged(currentMousePosition);
}

void SceneView::updateTransform()
{
    m_viewMatrix.setToIdentity();
    m_viewMatrix.translate(static_cast<float>(m_pan.x()), static_cast<float>(m_pan.y()));
    m_viewMatrix.scale(m_zoom, m_zoom);
}

void SceneView::resetViewToSceneBounds()
{
    const Coordinate& min{ m_sceneBounds.min };
    const double bbWidth{ getWidth(m_sceneBounds) };
    const double bbHeight{ getHeight(m_sceneBounds) };

    glViewport(
        static_cast<int>(min.x),
        static_cast<int>(min.y),
        static_cast<int>(bbWidth),
        static_cast<int>(bbHeight)
    );

    m_projMatrix.setToIdentity();
    m_projMatrix.ortho(
        static_cast<float>(min.x),
        static_cast<float>(min.x + bbWidth),
        static_cast<float>(min.y + bbHeight),
        static_cast<float>(min.y),
        -1.0F,
        1.0F
    );
}
