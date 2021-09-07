#include <QHBoxLayout>
#include <QOpenGLFunctions>

#include "QideMapEditor.hpp"

/**
 *  @addtogroup MapEditorWindow
 *  Map editor implementation
 *  @{
 */

QideMapEditorWindow::QideMapEditorWindow(QWindow *parent)
	: QOpenGLWindow(QOpenGLWindow::NoPartialUpdate, parent)
	, QOpenGLFunctions_4_3_Core()
{
}

void QideMapEditorWindow::initializeGL(){
	Q_ASSERT(initializeOpenGLFunctions());

	const float bgBrightness = 3.f/9.f;
	glClearColor(bgBrightness, bgBrightness, bgBrightness, 1.f);
}

void QideMapEditorWindow::resizeGL(int w, int h){
	glViewport(0, 0, w, h);
}

void QideMapEditorWindow::paintGL(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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

	auto lay = new QHBoxLayout(this);
	lay->addWidget(windowCont);
	lay->setContentsMargins(0, 0, 0, 0);

	setContentsMargins(0, 0, 0, 0);
}


/**
 * @}
 */
