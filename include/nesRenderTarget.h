#pragma once
#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions_4_5_Core>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLTexture>

#include "nes.h"


class NESRenderTarget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
	Q_OBJECT

public:
	NESRenderTarget(NES& nes, QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
	~NESRenderTarget();

protected:
	virtual void initializeGL() override;
	virtual void resizeGL(int w, int h) override;
	virtual void paintGL() override;
	virtual QSize minimumSizeHint() const override;

private:
	QOpenGLVertexArrayObject VAO;
	QOpenGLBuffer VBO;
	QOpenGLBuffer EBO;
	QOpenGLShaderProgram* shader;
	QOpenGLTexture* texture;
	
	NES& nes;
};
