#include "SceneView.hpp"

#include "core/Overload.hpp"
#include "domain/Linetype.hpp"
#include "domain/Linetypes.hpp"

#include <QColor>
#include <QDebug>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>

#include <gsl/narrow>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <utility>
#include <variant>
#include <vector>

namespace fcad::app {

namespace {

    // Vertical gradient, from the top to the bottom edge of the view.
    struct BackgroundStyle final
    {
        QColor topColor;
        QColor bottomColor;
    };

    // Axes through the world origin, behind the drawing. Width in logical pixels.
    struct AxesStyle final
    {
        QColor xColor;
        QColor yColor;
        double width;
    };

    // Dot grid, behind the axes. Spacings are powers of ten in world units.
    // The minor dots use the finest one at least minimumSpacing apart and
    // fade in as they spread out, until the next one replaces them. Major
    // dots are ten times as far apart. Spacing and radii in logical pixels.
    struct GridStyle final
    {
        // Alpha is the strength of a fully shown dot.
        QColor color;
        double minimumSpacing;
        double minorRadius;
        double majorRadius;
    };

    // Line widths in logical pixels, independent of the zoom.
    struct LineStyle final
    {
        // All lines, when lineweights are hidden.
        double width;
        // The narrowest a line gets when lineweights are shown, so that thin ones stay visible.
        double minimumLineweightWidth;
        // Dash patterns shorter than this are drawn continuous, since their dashes would blur.
        double minimumPatternLength;
    };

    // Mouse wheel zoom. The limits are relative to the fitted view, to stay
    // within float precision.
    struct ZoomBehavior final
    {
        // Zoom factor per mouse wheel notch.
        double stepFactor;
        double minimum;
        double maximum;
    };

}   // namespace

constexpr int windowWeight = 1076;
constexpr int windowHeight = 768;
constexpr geo::BoundingBox defaultBounds{ .min{ 0.0, 0.0 }, .max{ 1.0, 1.0 } };
// Empty space around the fitted scene, as a fraction of its extent, per side.
constexpr double viewMargin = 0.05;
// Extent shown when the scene has neither width nor height (e.g. a single point).
constexpr double minimumViewExtent = 1.0;
constexpr BackgroundStyle backgroundStyle{ .topColor{ 0x23, 0x27, 0x2E },
                                           .bottomColor{ 0x11, 0x13, 0x16 } };
constexpr AxesStyle axesStyle{ .xColor{ 0xB8, 0x4A, 0x4A },
                               .yColor{ 0x4A, 0x9A, 0x55 },
                               .width = 1.0 };
constexpr GridStyle gridStyle{ .color{ 0xA8, 0xB2, 0xC2, 0x99 },
                               .minimumSpacing = 16.0,
                               .minorRadius = 1.0,
                               .majorRadius = 1.75 };
// Floats per point in the vertex buffers (x, y).
constexpr int componentsPerVertex = 2;
// Floats per color in the vertex buffers (r, g, b).
constexpr int componentsPerColor = 3;
// Floats per lineweight in the vertex buffers (millimeters).
constexpr int componentsPerLineweight = 1;
// What adaptive colors become: white, which contrasts with the dark background.
constexpr QColor adaptiveColor{ 0xFF, 0xFF, 0xFF };
constexpr LineStyle lineStyle{ .width = 1.5,
                               .minimumLineweightWidth = 1.0,
                               .minimumPatternLength = 4.0 };
// Floats per dash pattern reference in the vertex buffers (first element, count, scale).
constexpr int componentsPerPattern = 3;
// Must match maximumPatternElements in shaders/line.frag.
constexpr std::size_t maximumPatternElements = 512;
constexpr double millimetersPerInch = 25.4;
constexpr ZoomBehavior zoomBehavior{ .stepFactor = 1.2, .minimum = 0.01, .maximum = 10000.0 };

namespace {

    QColor toQColor(const domain::Color& color)
    {
        return std::visit(
            core::Overload{
                [](const domain::Rgb& rgb) { return QColor{ rgb.r, rgb.g, rgb.b }; },
                [](const domain::Adaptive& /* adaptive */) { return adaptiveColor; },
            },
            color
        );
    }

    float toMillimeters(
        const domain::LayerLineweight& lineweight, domain::Lineweight defaultLineweightWidth
    )
    {
        const domain::Lineweight width{ std::visit(
            core::Overload{
                [defaultLineweightWidth](const domain::DefaultLineweight& /* default */) {
                    return defaultLineweightWidth;
                },
                [](const domain::Lineweight& own) { return own; },
            },
            lineweight
        ) };

        return gsl::narrow_cast<float>(width.value()) / 100.0F;
    }

    // Where a linetype's dash pattern is in Patterns::elements.
    struct PatternRange final
    {
        float offset{ 0.0F };
        // 0 for a continuous line.
        float count{ 0.0F };
    };

    // The dash patterns of a document's linetypes, as u_patterns in line.frag expects them.
    struct Patterns final
    {
        std::vector<float> elements;
        std::vector<std::pair<domain::LinetypeId, PatternRange>> ranges;

        PatternRange range(domain::LinetypeId id) const
        {
            const auto it{
                std::ranges::find(ranges, id, &std::pair<domain::LinetypeId, PatternRange>::first)
            };

            return it != ranges.end() ? it->second : PatternRange{};
        }
    };

    float toShaderLength(const domain::PatternElement& element)
    {
        return std::visit(
            core::Overload{
                [](const domain::Dash& dash) {
                    return gsl::narrow_cast<float>(dash.length.value());
                },
                [](const domain::Gap& gap) { return -gsl::narrow_cast<float>(gap.length.value()); },
                [](const domain::Dot& /* dot */) { return 0.0F; },
            },
            element
        );
    }

    // Linetypes whose patterns don't fit into the shader anymore are drawn continuous.
    Patterns toPatterns(const domain::Linetypes& linetypes)
    {
        Patterns patterns;

        for (const domain::Linetype& linetype : linetypes) {
            if (linetype.pattern().isContinuous()) {
                continue;
            }

            const domain::PatternElements& pattern{ linetype.pattern().elements() };

            if (patterns.elements.size() + pattern.size() > maximumPatternElements) {
                qWarning() << "Too many dash pattern elements; drawing linetype"
                           << linetype.name().value() << "continuous";
                continue;
            }

            patterns.ranges.emplace_back(
                linetype.id(),
                PatternRange{ .offset = gsl::narrow_cast<float>(patterns.elements.size()),
                              .count = gsl::narrow_cast<float>(pattern.size()) }
            );
            for (const domain::PatternElement& element : pattern) {
                patterns.elements.push_back(toShaderLength(element));
            }
        }

        return patterns;
    }

}   // namespace

SceneView::SceneView(const std::optional<domain::Document>& document)
    : m_document{ document }
{
    setMouseTracking(true);
}

QSize SceneView::sizeHint() const { return QSize{ windowWeight, windowHeight }; }

void SceneView::updateDocument()
{
    // Before the first show there is no GL context yet. initializeGL() uploads the document then.
    if (!isValid()) {
        return;
    }

    makeCurrent();
    uploadGeometry();
    doneCurrent();

    fitViewToScene();
    update();
}

void SceneView::setAxesVisible(bool visible)
{
    m_axesVisible = visible;
    update();
}

void SceneView::setGridVisible(bool visible)
{
    m_gridVisible = visible;
    update();
}

void SceneView::setLineweightsVisible(bool visible)
{
    m_lineweightsVisible = visible;
    update();
}

void SceneView::setDefaultLineweightWidth(domain::Lineweight width)
{
    m_defaultLineweightWidth = width;

    // The width is part of the uploaded geometry. Without a GL context, initializeGL() uploads it.
    if (!isValid()) {
        return;
    }

    // Upload again without fitting the view, which stays where it is.
    makeCurrent();
    uploadGeometry();
    doneCurrent();

    update();
}

void SceneView::uploadGeometry()
{
    std::optional<geo::BoundingBox> documentBounds;
    std::vector<float> vertices;

    m_lines.instanceCount = 0;
    m_patterns.clear();
    if (m_document.has_value()) {
        const domain::Document& document{ m_document.value() };
        documentBounds = domain::boundingBox(document);

        Patterns patterns{ toPatterns(document.linetypes()) };
        const double documentLinetypeScale{ document.linetypeScale().value() };

        for (const domain::Layer& layer : document.layers()) {
            if (layer.isVisible()) {
                for (const domain::Line& entity : layer.lines()) {
                    const geo::Line& line{ entity.geometry() };
                    const QColor color{ toQColor(domain::resolved(entity.color(), layer.color())) };
                    const float lineweight{ toMillimeters(
                        domain::resolved(entity.lineweight(), layer.lineweight()),
                        m_defaultLineweightWidth
                    ) };
                    const PatternRange pattern{
                        patterns.range(domain::resolved(entity.linetype(), layer.linetype()))
                    };
                    const float linetypeScale{ gsl::narrow_cast<float>(
                        documentLinetypeScale * entity.linetypeScale().value()
                    ) };
                    vertices.push_back(gsl::narrow_cast<float>(line.start.x));
                    vertices.push_back(gsl::narrow_cast<float>(line.start.y));
                    vertices.push_back(gsl::narrow_cast<float>(line.end.x));
                    vertices.push_back(gsl::narrow_cast<float>(line.end.y));
                    vertices.push_back(color.redF());
                    vertices.push_back(color.greenF());
                    vertices.push_back(color.blueF());
                    vertices.push_back(lineweight);
                    vertices.push_back(pattern.offset);
                    vertices.push_back(pattern.count);
                    vertices.push_back(linetypeScale);
                    ++m_lines.instanceCount;
                }
            }
        }

        m_patterns = std::move(patterns.elements);
    }

    m_sceneBounds = documentBounds.value_or(defaultBounds);

    // QOpenGLBuffer takes the size as int. Draw nothing rather than truncate the buffer.
    if (!std::in_range<int>(vertices.size() * sizeof(float))) {
        qWarning() << "Too many lines to draw:" << m_lines.instanceCount;
        vertices.clear();
        m_lines.instanceCount = 0;
    }

    m_lines.vao.bind();
    m_lines.vbo.bind();
    m_lines.vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

    m_lines.vbo.allocate(vertices.data(), gsl::narrow_cast<int>(vertices.size() * sizeof(float)));

    // Per line: start point, end point, color, lineweight, then dash pattern.
    constexpr int stride{ (2 * componentsPerVertex + componentsPerColor + componentsPerLineweight
                           + componentsPerPattern)
                          * sizeof(float) };
    const int startLocation{ m_lineProgram->attributeLocation("a_start") };
    const int endLocation{ m_lineProgram->attributeLocation("a_end") };
    const int colorLocation{ m_lineProgram->attributeLocation("a_color") };
    const int lineweightLocation{ m_lineProgram->attributeLocation("a_lineweight") };
    const int patternLocation{ m_lineProgram->attributeLocation("a_pattern") };
    m_lineProgram->enableAttributeArray(startLocation);
    m_lineProgram->enableAttributeArray(endLocation);
    m_lineProgram->enableAttributeArray(colorLocation);
    m_lineProgram->enableAttributeArray(lineweightLocation);
    m_lineProgram->enableAttributeArray(patternLocation);
    m_lineProgram->setAttributeBuffer(startLocation, GL_FLOAT, 0, componentsPerVertex, stride);
    m_lineProgram->setAttributeBuffer(
        endLocation, GL_FLOAT, componentsPerVertex * sizeof(float), componentsPerVertex, stride
    );
    m_lineProgram->setAttributeBuffer(
        colorLocation, GL_FLOAT, 2 * componentsPerVertex * sizeof(float), componentsPerColor, stride
    );
    m_lineProgram->setAttributeBuffer(
        lineweightLocation,
        GL_FLOAT,
        (2 * componentsPerVertex + componentsPerColor) * sizeof(float),
        componentsPerLineweight,
        stride
    );
    m_lineProgram->setAttributeBuffer(
        patternLocation,
        GL_FLOAT,
        (2 * componentsPerVertex + componentsPerColor + componentsPerLineweight) * sizeof(float),
        componentsPerPattern,
        stride
    );
    glVertexAttribDivisor(gsl::narrow_cast<GLuint>(startLocation), 1);
    glVertexAttribDivisor(gsl::narrow_cast<GLuint>(endLocation), 1);
    glVertexAttribDivisor(gsl::narrow_cast<GLuint>(colorLocation), 1);
    glVertexAttribDivisor(gsl::narrow_cast<GLuint>(lineweightLocation), 1);
    glVertexAttribDivisor(gsl::narrow_cast<GLuint>(patternLocation), 1);

    m_lines.vbo.release();
    m_lines.vao.release();
    m_lineProgram->release();
}

void SceneView::initializeGL()
{
    initializeOpenGLFunctions();

    m_backgroundProgram = std::make_unique<QOpenGLShaderProgram>();
    m_backgroundProgram->addShaderFromSourceFile(
        QOpenGLShader::Vertex, ":/shaders/background.vert"
    );
    m_backgroundProgram->addShaderFromSourceFile(
        QOpenGLShader::Fragment, ":/shaders/background.frag"
    );
    if (!m_backgroundProgram->link()) {
        qWarning() << "Linking the background shader failed:" << m_backgroundProgram->log();
    }
    m_backgroundVao.create();

    m_lineProgram = std::make_unique<QOpenGLShaderProgram>();
    m_lineProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/line.vert");
    m_lineProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/line.frag");
    if (!m_lineProgram->link()) {
        qWarning() << "Linking the line shader failed:" << m_lineProgram->log();
    }

    m_lines.vao.create();
    m_lines.vbo.create();

    m_lineProgram->release();

    uploadGeometry();
    fitViewToScene();
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
    // Not fitted yet because the widget has no size, so there is nothing to draw.
    if (m_viewScale == 0.0) {
        return;
    }

    const qreal pixelRatio{ devicePixelRatio() };

    // The background covers every pixel, so there is no need to clear first.
    glDisable(GL_BLEND);
    m_backgroundProgram->bind();
    m_backgroundProgram->setUniformValue("u_topColor", backgroundStyle.topColor);
    m_backgroundProgram->setUniformValue("u_bottomColor", backgroundStyle.bottomColor);
    // Window coordinates have y up, widget coordinates y down.
    const QPointF origin{ mapFromWorld(geo::Coordinate{ .x = 0.0, .y = 0.0 }) };
    const double originX{ origin.x() * pixelRatio };
    const double originY{ (height() - origin.y()) * pixelRatio };
    m_backgroundProgram->setUniformValue(
        "u_origin", QVector2D{ gsl::narrow_cast<float>(originX), gsl::narrow_cast<float>(originY) }
    );
    m_backgroundProgram->setUniformValue(
        "u_axisWidth", gsl::narrow_cast<float>(axesStyle.width * pixelRatio)
    );
    m_backgroundProgram->setUniformValue("u_xAxisColor", axesStyle.xColor);
    m_backgroundProgram->setUniformValue("u_yAxisColor", axesStyle.yColor);
    m_backgroundProgram->setUniformValue("u_axesVisible", m_axesVisible);

    // The minor spacing ends up in [minimumSpacing, 10 * minimumSpacing),
    // and the minor dots' intensity goes from 0 to 1 over that range.
    const double gridSpacingWorld{
        std::pow(10.0, std::ceil(std::log10(gridStyle.minimumSpacing * m_viewScale)))
    };
    const double gridSpacing{ gridSpacingWorld / m_viewScale };
    const double gridSpacingDevice{ gridSpacing * pixelRatio };
    const double majorGridSpacingDevice{ gridSpacingDevice * 10.0 };
    m_backgroundProgram->setUniformValue(
        "u_gridSpacing", gsl::narrow_cast<float>(gridSpacingDevice)
    );
    m_backgroundProgram->setUniformValue(
        "u_gridMinorOffset",
        QVector2D{ gsl::narrow_cast<float>(std::fmod(originX, gridSpacingDevice)),
                   gsl::narrow_cast<float>(std::fmod(originY, gridSpacingDevice)) }
    );
    m_backgroundProgram->setUniformValue(
        "u_gridMajorOffset",
        QVector2D{ gsl::narrow_cast<float>(std::fmod(originX, majorGridSpacingDevice)),
                   gsl::narrow_cast<float>(std::fmod(originY, majorGridSpacingDevice)) }
    );
    m_backgroundProgram->setUniformValue(
        "u_gridMinorRadius", gsl::narrow_cast<float>(gridStyle.minorRadius * pixelRatio)
    );
    m_backgroundProgram->setUniformValue(
        "u_gridMajorRadius", gsl::narrow_cast<float>(gridStyle.majorRadius * pixelRatio)
    );
    m_backgroundProgram->setUniformValue(
        "u_gridMinorIntensity",
        gsl::narrow_cast<float>(std::log10(gridSpacing / gridStyle.minimumSpacing))
    );
    m_backgroundProgram->setUniformValue("u_gridColor", gridStyle.color);
    m_backgroundProgram->setUniformValue("u_gridVisible", m_gridVisible);
    {
        QOpenGLVertexArrayObject::Binder vaoBinder(&m_backgroundVao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    m_backgroundProgram->release();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_lineProgram->bind();
    m_lineProgram->setUniformValue("u_viewProjection", viewProjection());
    m_lineProgram->setUniformValue(
        "u_viewportSize",
        QVector2D{ gsl::narrow_cast<float>(width() * pixelRatio),
                   gsl::narrow_cast<float>(height() * pixelRatio) }
    );
    m_lineProgram->setUniformValue("u_lineweightsVisible", m_lineweightsVisible);
    // The logical DPI follows the user's display scaling, the physical one is often wrong.
    m_lineProgram->setUniformValue(
        "u_pixelsPerMillimeter",
        gsl::narrow_cast<float>(logicalDpiX() / millimetersPerInch * pixelRatio)
    );
    m_lineProgram->setUniformValue(
        "u_minimumLineWidth", gsl::narrow_cast<float>(lineStyle.minimumLineweightWidth * pixelRatio)
    );
    m_lineProgram->setUniformValue(
        "u_lineWidth", gsl::narrow_cast<float>(lineStyle.width * pixelRatio)
    );
    if (!m_patterns.empty()) {
        m_lineProgram->setUniformValueArray(
            "u_patterns", m_patterns.data(), gsl::narrow_cast<int>(m_patterns.size()), 1
        );
    }
    m_lineProgram->setUniformValue(
        "u_worldPerPixel", gsl::narrow_cast<float>(m_viewScale / pixelRatio)
    );
    m_lineProgram->setUniformValue(
        "u_minimumPatternLength",
        gsl::narrow_cast<float>(lineStyle.minimumPatternLength * pixelRatio)
    );

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_lines.vao);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_lines.instanceCount);

    m_lineProgram->release();
}

void SceneView::wheelEvent(QWheelEvent* event)
{
    const double notches{ event->angleDelta().y() / 120.0 };
    if (notches == 0.0 || m_viewScale == 0.0) {
        return;
    }

    const double newScale{ std::clamp(
        m_viewScale / std::pow(zoomBehavior.stepFactor, notches),
        m_fittedViewScale / zoomBehavior.maximum,
        m_fittedViewScale / zoomBehavior.minimum
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

QPointF SceneView::mapFromWorld(const geo::Coordinate& world) const
{
    // Widget y points down, world y up.
    return QPointF{ width() / 2.0 + (world.x - m_viewCenter.x) / m_viewScale,
                    height() / 2.0 - (world.y - m_viewCenter.y) / m_viewScale };
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
