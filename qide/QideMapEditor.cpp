#include <QDebug>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QHBoxLayout>

#include "qide/RendererGL.hpp"

#include "QideMapEditor.hpp"

/**
 *  @addtogroup MapEditorWindow
 *  Map editor implementation
 *  @{
 */

QideMapEditorWindow::QideMapEditorWindow(QWindow *parent)
	: QOpenGLWindow(QOpenGLWindow::NoPartialUpdate, parent)
	, m_cam(qide::Camera::Perspective{}, 92.f, 16.f / 9.f, 0.001f, 500.f)
	, m_r(nullptr)
{
}

QideMapEditorWindow::~QideMapEditorWindow(){}

void QideMapEditorWindow::initializeGL(){
	auto glGetProc = [c{context()}](const char *name){
		return c->getProcAddress(name);
	};

	m_cam.setProjMat(qide::Camera::Perspective{}, 92.f, float(width()) / float(height()), 0.001f, 500.f);

	m_r = std::make_unique<qide::RendererGL43>(width(), height(), glGetProc, context());
	m_r->setDrawAxis(true);
}

void QideMapEditorWindow::resizeGL(int w, int h){
	m_cam.setProjMat(qide::Camera::Perspective{}, 92.f, float(width()) / float(height()), 0.001f, 500.f);
	m_r->resize(w, h);
}

void QideMapEditorWindow::paintGL(){
	m_r->present(m_cam);
}

void QideMapEditorWindow::mouseMoveEvent(QMouseEvent *event){
	event->accept();

	if(event->buttons() & Qt::MouseButton::RightButton){
		auto dV = event->localPos() - m_lastMousePos;
		// TODO: move camera

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
}

void QideMapEditorWindow::keyReleaseEvent(QKeyEvent *event){

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
