#pragma once

#include "domain/Document.hpp"
#include "geo/BoundingBox.hpp"

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QPointF>

#include <memory>
#include <optional>

namespace fcad::app {

class SceneView final
    : public QOpenGLWidget
    , protected QOpenGLExtraFunctions
{
    Q_OBJECT

signals:
    void mousePositionChanged(const QPointF&);

public:
    SceneView(const std::optional<domain::Document>& document);

    QSize sizeHint() const override;

    void updateDocument();

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    QPointF mapToWorld(const QPointF& widgetPosition) const;
    QMatrix4x4 viewProjection() const;
    void uploadGeometry();
    void fitViewToScene();

    struct Geometry
    {
        QOpenGLVertexArrayObject vao;
        QOpenGLBuffer vbo{ QOpenGLBuffer::VertexBuffer };
        int instanceCount{ 0 };
    };

    const std::optional<domain::Document>& m_document;
    std::unique_ptr<QOpenGLShaderProgram> m_program;
    Geometry m_lines;

    geo::BoundingBox m_sceneBounds;
    // World point at the widget's centre.
    geo::Coordinate m_viewCenter;
    // World units per logical pixel; 0 until the view has been fitted.
    double m_viewScale{ 0.0 };
    // m_viewScale after the last fit, the reference for the zoom limits.
    double m_fittedViewScale{ 0.0 };
    QPointF m_lastMousePos;
};

}   // namespace fcad::app
