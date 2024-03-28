#include "NESRenderTarget.h"

#include <vector>


NESRenderTarget::NESRenderTarget(NES& nes, QWidget* parent, Qt::WindowFlags flags)
	: QOpenGLWidget{parent, flags}
	, shader{ nullptr }
	, texture{ nullptr }
	, nes{ nes }
{
	
}

NESRenderTarget::~NESRenderTarget()
{
	makeCurrent();
	VAO.destroy();
	VBO.destroy();
	EBO.destroy();
	delete shader;
	delete texture;
	doneCurrent();
}

void NESRenderTarget::initializeGL()
{
	initializeOpenGLFunctions();

	// vertex array
	std::vector<GLfloat> vertices =
	{
		// positions	// texture coords
		-1.0f,  1.0f,	0.0f, 1.0f,	// top left
		 1.0f,  1.0f,	1.0f, 1.0f,	// top right
		-1.0f, -1.0f,	0.0f, 0.0f,	// bottom left
		 1.0f, -1.0f,	1.0f, 0.0f	// bottom right
	};

	// index array
	std::vector<GLuint> indices =
	{
		0, 1, 2,
		2, 3, 1
	};


	VAO.create();
	VAO.bind();

	VBO = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	VBO.create();
	VBO.bind();
	VBO.setUsagePattern(QOpenGLBuffer::StaticDraw);
	VBO.allocate(vertices.data(), vertices.size() * sizeof(GLfloat));

	shader = new QOpenGLShaderProgram(this);
	shader->addShaderFromSourceFile(QOpenGLShader::Vertex, "src\\shader\\nesRenderTarget.vert");
	shader->addShaderFromSourceFile(QOpenGLShader::Fragment, "src\\shader\\nesRenderTarget.frag");
	shader->link();
	shader->setAttributeBuffer(0, GL_FLOAT, 0 * sizeof(GLfloat), 2, 4 * sizeof(GLfloat));
	shader->enableAttributeArray(0);
	shader->setAttributeBuffer(1, GL_FLOAT, 2 * sizeof(GLfloat), 2, 4 * sizeof(GLfloat));
	shader->enableAttributeArray(1);

	EBO = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
	EBO.create();
	EBO.bind();
	EBO.setUsagePattern(QOpenGLBuffer::StaticDraw);
	EBO.allocate(indices.data(), indices.size() * sizeof(GLfloat));
	
	VAO.release();
	VBO.release();
	EBO.release();

	texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
	texture->create();
	texture->bind();
	texture->setMinificationFilter(QOpenGLTexture::Nearest);
	texture->setMagnificationFilter(QOpenGLTexture::Nearest);
	//texture->setData(QImage("resources\\texture\\piston_texture.png"));
	texture->release();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void NESRenderTarget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}

void NESRenderTarget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT);

	nes.clock();

	QImage image(
		nes.getScreenBuffer(),
		nes.getScreenWidth(),
		nes.getScreenHeight(),
		QImage::Format_RGB888);

	

	VAO.bind();
	shader->bind();
	texture->bind();
	texture->setData(image);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	VAO.release();
	shader->release();
	texture->release();

	update();
}

QSize NESRenderTarget::minimumSizeHint() const
{
	return QSize(nes.getScreenWidth(), nes.getScreenHeight());
}
