#include "SceneView.hpp"

#include <QDebug>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>

#include <gsl/narrow>

#include <algorithm>
#include <cmath>

namespace fcad::app {

constexpr int windowWeight = 1076;
constexpr int windowHeight = 768;
constexpr geo::BoundingBox defaultBounds{ .min{ 0.0, 0.0 }, .max{ 1.0, 1.0 } };
// Empty space around the fitted scene, as a fraction of its extent, per side.
constexpr double viewMargin = 0.05;
// Extent shown when the scene has neither width nor height (e.g. a single point).
constexpr double minimumViewExtent = 1.0;
// Floats per point in the vertex buffers (x, y).
constexpr int componentsPerVertex = 2;
// Line width in logical pixels, independent of the zoom.
constexpr double lineWidth = 1.5;
// Zoom factor per mouse wheel notch.
constexpr double zoomStepFactor = 1.2;
// Zoom limits relative to the fitted view, to stay within float precision.
constexpr double minimumZoom = 0.01;
constexpr double maximumZoom = 10000.0;

SceneView::SceneView(const domain::Document& document)
    : m_document{ document }
{
    setMouseTracking(true);
}

QSize SceneView::sizeHint() const { return QSize{ windowWeight, windowHeight }; }

void SceneView::updateDocument()
{
    makeCurrent();
    uploadGeometry();
    doneCurrent();
}

void SceneView::uploadGeometry()
{
    const geo::BoundingBox sceneBounds{ domain::boundingBox(m_document).value_or(defaultBounds) };
    m_sceneBounds = sceneBounds;

    std::vector<float> vertices;

    m_lines.instanceCount = 0;
    for (const domain::Layer& layer : m_document.layers()) {
        if (layer.visible) {
            for (const geo::Line& line : layer.lines) {
                vertices.push_back(gsl::narrow_cast<float>(line.start.x));
                vertices.push_back(gsl::narrow_cast<float>(line.start.y));
                vertices.push_back(gsl::narrow_cast<float>(line.end.x));
                vertices.push_back(gsl::narrow_cast<float>(line.end.y));
                ++m_lines.instanceCount;
            }
        }
    }

    m_lines.vao.bind();
    m_lines.vbo.bind();
    m_lines.vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

    m_lines.vbo.allocate(vertices.data(), gsl::narrow<int>(vertices.size() * sizeof(float)));

    // Per line: start point, then end point.
    constexpr int stride{ 2 * componentsPerVertex * sizeof(float) };
    const int startLocation{ m_program->attributeLocation("a_start") };
    const int endLocation{ m_program->attributeLocation("a_end") };
    m_program->enableAttributeArray(startLocation);
    m_program->enableAttributeArray(endLocation);
    m_program->setAttributeBuffer(startLocation, GL_FLOAT, 0, componentsPerVertex, stride);
    m_program->setAttributeBuffer(
        endLocation, GL_FLOAT, componentsPerVertex * sizeof(float), componentsPerVertex, stride
    );
    glVertexAttribDivisor(gsl::narrow_cast<GLuint>(startLocation), 1);
    glVertexAttribDivisor(gsl::narrow_cast<GLuint>(endLocation), 1);

    m_lines.vbo.release();
    m_lines.vao.release();
    m_program->release();

    fitViewToScene();
    update();
}

void SceneView::initializeGL()
{
    initializeOpenGLFunctions();

    m_program = std::make_unique<QOpenGLShaderProgram>();
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/line.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/line.frag");
    if (!m_program->link()) {
        qWarning() << "Linking the line shader failed:" << m_program->log();
    }

    m_lines.vao.create();
    m_lines.vbo.create();

    m_program->release();

    uploadGeometry();
}

void SceneView::resizeGL(int /* width */, int /* height */)
{
    const qreal pixelRatio{ devicePixelRatio() };
    glViewport(
        0,
        0,
        gsl::narrow_cast<int>(width() * pixelRatio),
        gsl::narrow_cast<int>(height() * pixelRatio)
    );

    // Keep centre and scale, so the drawing keeps its size. Only fit if that
    // couldn't happen yet because the widget had no size.
    if (m_viewScale == 0.0) {
        fitViewToScene();
    }
}

void SceneView::paintGL()
{
    glClearColor(0.1F, 0.1F, 0.1F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const qreal pixelRatio{ devicePixelRatio() };

    m_program->bind();
    m_program->setUniformValue("u_viewProjection", viewProjection());
    m_program->setUniformValue(
        "u_viewportSize",
        QVector2D{ gsl::narrow_cast<float>(width() * pixelRatio),
                   gsl::narrow_cast<float>(height() * pixelRatio) }
    );
    m_program->setUniformValue("u_lineWidth", gsl::narrow_cast<float>(lineWidth * pixelRatio));

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_lines.vao);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_lines.instanceCount);

    m_program->release();
}

void SceneView::wheelEvent(QWheelEvent* event)
{
    const double notches{ event->angleDelta().y() / 120.0 };
    if (notches == 0.0 || m_viewScale == 0.0) {
        return;
    }

    const double newScale{ std::clamp(
        m_viewScale / std::pow(zoomStepFactor, notches),
        m_fittedViewScale / maximumZoom,
        m_fittedViewScale / minimumZoom
    ) };

    // Keep the world point under the cursor fixed by scaling its offset from the
    // view centre.
    const QPointF world{ mapToWorld(event->position()) };
    const double ratio{ newScale / m_viewScale };
    m_viewCenter = geo::Coordinate{ .x = world.x() - (world.x() - m_viewCenter.x) * ratio,
                                    .y = world.y() - (world.y() - m_viewCenter.y) * ratio };
    m_viewScale = newScale;

    update();
    event->accept();
}

void SceneView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton) {
        m_lastMousePos = event->position();
        setCursor(Qt::ClosedHandCursor);
    }
}

void SceneView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton) {
        unsetCursor();
    }
}

void SceneView::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() != Qt::MiddleButton) {
        QOpenGLWidget::mouseDoubleClickEvent(event);
        return;
    }

    fitViewToScene();
    update();
    emit mousePositionChanged(mapToWorld(event->position()));
}

void SceneView::mouseMoveEvent(QMouseEvent* event)
{
    const bool panning{ event->buttons().testFlag(Qt::MiddleButton) };
    if (panning) {
        // Move the view so the grabbed world point stays under the cursor.
        // Widget y points down, world y up.
        const QPointF delta{ event->position() - m_lastMousePos };
        m_viewCenter.x -= delta.x() * m_viewScale;
        m_viewCenter.y += delta.y() * m_viewScale;
        m_lastMousePos = event->position();
        update();
    }

    emit mousePositionChanged(mapToWorld(event->position()));
}

QPointF SceneView::mapToWorld(const QPointF& widgetPosition) const
{
    // Widget y points down, world y up.
    return QPointF{ m_viewCenter.x + (widgetPosition.x() - width() / 2.0) * m_viewScale,
                    m_viewCenter.y - (widgetPosition.y() - height() / 2.0) * m_viewScale };
}

QMatrix4x4 SceneView::viewProjection() const
{
    const double halfWidth{ width() * m_viewScale / 2.0 };
    const double halfHeight{ height() * m_viewScale / 2.0 };

    QMatrix4x4 projection;
    projection.ortho(
        gsl::narrow_cast<float>(m_viewCenter.x - halfWidth),
        gsl::narrow_cast<float>(m_viewCenter.x + halfWidth),
        gsl::narrow_cast<float>(m_viewCenter.y - halfHeight),
        gsl::narrow_cast<float>(m_viewCenter.y + halfHeight),
        -1.0F,
        1.0F
    );
    return projection;
}

void SceneView::fitViewToScene()
{
    const int widgetWidth{ width() };
    const int widgetHeight{ height() };
    if (widgetWidth <= 0 || widgetHeight <= 0) {
        // Nothing to fit to yet; resizeGL() fits once the widget has a size.
        m_viewScale = 0.0;
        return;
    }

    double bbWidth{ geo::getWidth(m_sceneBounds) };
    double bbHeight{ geo::getHeight(m_sceneBounds) };
    if (bbWidth == 0.0 && bbHeight == 0.0) {
        bbWidth = minimumViewExtent;
        bbHeight = minimumViewExtent;
    }

    // The limiting dimension needs the most world units per pixel.
    const double marginFactor{ 1.0 + 2.0 * viewMargin };
    m_viewScale =
        std::max(bbWidth * marginFactor / widgetWidth, bbHeight * marginFactor / widgetHeight);
    m_fittedViewScale = m_viewScale;
    m_viewCenter = geo::Coordinate{ .x = (m_sceneBounds.min.x + m_sceneBounds.max.x) / 2.0,
                                    .y = (m_sceneBounds.min.y + m_sceneBounds.max.y) / 2.0 };
}

}   // namespace fcad::app
