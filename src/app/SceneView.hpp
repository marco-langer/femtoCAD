#pragma once

#include "core/BoundingBox.hpp"
#include "core/Workspace.hpp"

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QPointF>

#include <memory>

class SceneView final
    : public QOpenGLWidget
    , protected QOpenGLFunctions
{
    Q_OBJECT

signals:
    void mousePositionChanged(const QPointF&);

public:
    SceneView(Workspace& workspace);

    QSize sizeHint() const override;

    void updateWorkspace();

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
        explicit Geometry(GLenum type)
            : primitiveType{ type }
        {}

        QOpenGLVertexArrayObject vao;
        QOpenGLBuffer vbo{ QOpenGLBuffer::VertexBuffer };
        int vertexCount{ 0 };
        const GLenum primitiveType;
    };

    Workspace& m_workspace;
    std::unique_ptr<QOpenGLShaderProgram> m_program;
    Geometry m_lines{ GL_LINES };

    BoundingBox m_sceneBounds;
    // World point at the widget's centre.
    Coordinate m_viewCenter;
    // World units per logical pixel; 0 until the view has been fitted.
    double m_viewScale{ 0.0 };
    // m_viewScale after the last fit, the reference for the zoom limits.
    double m_fittedViewScale{ 0.0 };
    QPointF m_lastMousePos;
};
