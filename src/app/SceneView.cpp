#include "SceneView.hpp"

#include <QDebug>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>

#include <gsl/narrow>

#include <algorithm>
#include <cmath>

constexpr int windowWeight = 1076;
constexpr int windowHeight = 768;
constexpr BoundingBox defaultBounds{ .min{ 0.0, 0.0 }, .max{ 1.0, 1.0 } };
// Empty space around the fitted scene, as a fraction of its extent, per side.
constexpr double viewMargin = 0.05;
// Extent shown when the scene has neither width nor height (e.g. a single point).
constexpr double minimumViewExtent = 1.0;
// Floats per vertex in the vertex buffers (x, y).
constexpr int componentsPerVertex = 2;
// Zoom factor per mouse wheel notch.
constexpr double zoomStepFactor = 1.2;
// Zoom limits relative to the fitted view, to stay within float precision.
constexpr double minimumZoom = 0.01;
constexpr double maximumZoom = 10000.0;

SceneView::SceneView(Workspace& workspace)
    : m_workspace{ workspace }
{
    setMouseTracking(true);
}

QSize SceneView::sizeHint() const { return QSize{ windowWeight, windowHeight }; }

void SceneView::updateWorkspace()
{
    makeCurrent();
    uploadGeometry();
    doneCurrent();
}

void SceneView::uploadGeometry()
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
    m_program->setAttributeBuffer("position", GL_FLOAT, 0, componentsPerVertex);

    m_lines.vbo.release();
    m_lines.vao.release();
    m_program->release();

    resetZoom();

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

    uploadGeometry();
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

void SceneView::wheelEvent(QWheelEvent* event)
{
    const double notches{ event->angleDelta().y() / 120.0 };
    if (notches == 0.0) {
        return;
    }

    // Keep the world point under the cursor fixed: with view = zoom * world + pan,
    // its view position must stay the same after changing the zoom.
    const QPointF world{ mapToWorld(event->position()) };
    const double oldZoom{ static_cast<double>(m_zoom) };
    const QPointF view{ oldZoom * world + m_pan };

    const double newZoom{
        std::clamp(oldZoom * std::pow(zoomStepFactor, notches), minimumZoom, maximumZoom)
    };
    m_zoom = gsl::narrow_cast<float>(newZoom);
    m_pan = view - newZoom * world;

    updateTransform();
    update();
    event->accept();
}

void SceneView::mousePressEvent(QMouseEvent* /* event */) {}

void SceneView::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() != Qt::MiddleButton) {
        QOpenGLWidget::mouseDoubleClickEvent(event);
        return;
    }

    resetZoom();
    update();
    emit mousePositionChanged(mapToWorld(event->position()));
}

void SceneView::mouseMoveEvent(QMouseEvent* event)
{
    emit mousePositionChanged(mapToWorld(event->position()));
}

QPointF SceneView::mapToWorld(const QPointF& widgetPosition) const
{
    const QSize widgetSize{ size() };

    // Map widget pixels (y down) to normalized device coordinates (y up) and back
    // through the inverse of the transformation used for rendering.
    const QVector4D ndc{
        gsl::narrow_cast<float>(2.0 * widgetPosition.x() / widgetSize.width() - 1.0),
        gsl::narrow_cast<float>(1.0 - 2.0 * widgetPosition.y() / widgetSize.height()),
        0.0F,
        1.0F
    };
    const QVector4D world{ (m_projMatrix * m_viewMatrix).inverted() * ndc };

    return QPointF{ static_cast<double>(world.x()), static_cast<double>(world.y()) };
}

void SceneView::resetZoom()
{
    m_zoom = 1.0F;
    m_pan = QPointF{};
    updateTransform();
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
