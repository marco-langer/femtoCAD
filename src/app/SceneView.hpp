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

class SceneView
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
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    void updateTransform();
    void resetViewToSceneBounds();

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
    QMatrix4x4 m_projMatrix;
    QMatrix4x4 m_viewMatrix;
    float m_zoom{ 1.0F };
    QPointF m_pan;
    QPointF m_lastMousePos;
};
