#include <QtMath>

#include <QVector3D>
#include <QDebug>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QTimer>

#include "qide/RendererGL.hpp"

#include "QideMapEditor.hpp"

/**
 *  @addtogroup MapEditorWindow
 *  Map editor implementation
 *  @{
 */

QideMapEditorWindow::QideMapEditorWindow(QWindow *parent)
	: QOpenGLWindow(QOpenGLWindow::NoPartialUpdate, parent)
	, m_cam(qide::Camera::Perspective{}, 3.14f, 16.f / 9.f, 0.001f, 500.f)
	, m_r(nullptr)
	, m_start(Clock::now())
	, m_frameStart(m_start)
	, m_lastMousePos()
	, m_movement(0.f, 0.f, 0.f)
{
}

QideMapEditorWindow::~QideMapEditorWindow(){}

void QideMapEditorWindow::initializeGL(){
	auto glGetProc = [c{context()}](const char *name){
		return c->getProcAddress(name);
	};

	m_cam.setProjMat(qide::Camera::Perspective{}, 3.14f, float(height()) / float(width()), 0.001f, 500.f);
	m_cam.setPosition({ 0.f, 0.f, -1.f });

	m_r = std::make_unique<qide::RendererGL43>(width(), height(), glGetProc, context());
	m_r->setDrawAxis(true);
}

void QideMapEditorWindow::resizeGL(int w, int h){
	m_cam.setProjMat(qide::Camera::Perspective{}, 3.14f, float(h) / float(w), 0.001f, 500.f);
	m_r->resize(w, h);
}

void QideMapEditorWindow::paintGL(){
	auto frameEnd = Clock::now();
	auto frameDt = frameEnd - m_frameStart;
	m_frameStart = frameEnd;

	auto dt = std::chrono::duration<float>(frameDt).count();

	auto forward_ = m_cam.forward() * m_movement.z;
	auto right_ = m_cam.right() * m_movement.x;
	auto up_ = m_cam.up() * m_movement.y;

	auto amnt = glm::normalize(forward_ + right_ + up_);

	if(glm::length(amnt) >= 0.999999f){
		amnt *= 2.f * dt;
		qDebug() << QVector3D(amnt.x, amnt.y, amnt.z);
		m_cam.translate(amnt);
	}

	m_r->present(m_cam);
}

void QideMapEditorWindow::mouseMoveEvent(QMouseEvent *event){
	event->accept();

	if(event->buttons() & Qt::MouseButton::RightButton){
		QPointF dV = (event->localPos() - m_lastMousePos);
		dV.setX(qDegreesToRadians(dV.x()));
		dV.setY(qDegreesToRadians(dV.y()));
		dV *= 90.f;

		m_cam.rotate({ 1.f, 0.f, 0.f }, dV.y());
		m_cam.rotate({ 0.f, 1.f, 0.f }, dV.x());

		m_lastMousePos = event->localPos();

		if(event->localPos().x() < 0.f){
			auto c = cursor();
			c.setPos(c.pos() + QPoint(width(), 0));
			setCursor(c);
		}
		else if(event->localPos().x() > width()){
			auto c = cursor();
			c.setPos(c.pos() - QPoint(width(), 0));
			setCursor(c);
		}

		if(event->localPos().y() < 0.f){
			auto c = cursor();
			c.setPos(c.pos() + QPoint(0, height()));
			setCursor(c);
		}
		else if(event->localPos().y() > height()){
			auto c = cursor();
			c.setPos(c.pos() - QPoint(0, height()));
			setCursor(c);
		}
	}
}

void QideMapEditorWindow::mousePressEvent(QMouseEvent *event){
	event->accept();

	if(event->button() == Qt::MouseButton::RightButton){
		//grabMouse();
		setKeyboardGrabEnabled(true);
		m_lastMousePos = event->localPos();
	}
}

void QideMapEditorWindow::mouseReleaseEvent(QMouseEvent *event){
	event->accept();

	if(event->button() == Qt::MouseButton::RightButton){
		//releaseMouse();
		setKeyboardGrabEnabled(false);
	}
}

void QideMapEditorWindow::keyPressEvent(QKeyEvent *event){
	event->accept();

	if(event->isAutoRepeat()) return;

	switch(event->key()){
		case Qt::Key_W: m_movement.z += 1.f; break;
		case Qt::Key_S: m_movement.z -= 1.f; break;

		case Qt::Key_A: m_movement.x -= 1.f; break;
		case Qt::Key_D: m_movement.x += 1.f; break;

		default: break;
	}
}

void QideMapEditorWindow::keyReleaseEvent(QKeyEvent *event){
	event->accept();

	if(event->isAutoRepeat()) return;

	switch(event->key()){
		case Qt::Key_W: m_movement.z -= 1.f; break;
		case Qt::Key_S: m_movement.z += 1.f; break;

		case Qt::Key_A: m_movement.x += 1.f; break;
		case Qt::Key_D: m_movement.x -= 1.f; break;

		default: break;
	}
}

/**
 * @}
 *
 * @addtogroup MapEditorWidget
 * Map editor widget implementation
 * @{
 */

QideMapEditor::QideMapEditor(QWidget *parent)
	: QWidget(parent)
{
	auto window = new QideMapEditorWindow;

	auto windowCont = QWidget::createWindowContainer(window, this);

	windowCont->setContentsMargins(0, 0, 0, 0);
	windowCont->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	windowCont->setEnabled(false);

	auto lay = new QHBoxLayout(this);
	lay->addWidget(windowCont);
	lay->setContentsMargins(0, 0, 0, 0);

	setContentsMargins(0, 0, 0, 0);
}

/**
 * @}
 */
