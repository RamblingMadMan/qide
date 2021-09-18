#ifndef QIDE_QIDEMAPEDITOR_HPP
#define QIDE_QIDEMAPEDITOR_HPP 1

#include <memory>

#include <QWidget>
#include <QOpenGLWindow>

#include "qide/Camera.hpp"

namespace qide{
	class RendererGL43;
}

class QideMapEditorWindow: public QOpenGLWindow{
	Q_OBJECT

	public:
		explicit QideMapEditorWindow(QWindow *parent = nullptr);

		~QideMapEditorWindow();

	protected:
		void initializeGL() override;
		void resizeGL(int w, int h) override;
		void paintGL() override;

		void mouseMoveEvent(QMouseEvent *event) override;
		void mousePressEvent(QMouseEvent *event) override;
		void mouseReleaseEvent(QMouseEvent *event) override;

		void keyPressEvent(QKeyEvent *event) override;
		void keyReleaseEvent(QKeyEvent *event) override;

	private:
		qide::Camera m_cam;
		std::unique_ptr<qide::RendererGL43> m_r;
		QPointF m_lastMousePos;
};

class QideMapEditor: public QWidget{
	Q_OBJECT

	public:
		explicit QideMapEditor(QWidget *parent = nullptr);
};

#endif // !QIDE_QIDEMAPEDITOR_HPP
