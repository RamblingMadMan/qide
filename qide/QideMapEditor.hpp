#ifndef QIDE_QIDEMAPEDITOR_HPP
#define QIDE_QIDEMAPEDITOR_HPP 1

#include <memory>
#include <chrono>

#include <QTime>
#include <QWidget>
#include <QOpenGLWidget>

#include "qide/Camera.hpp"

namespace qide{
	class RendererGL43;
}

class QideMapEditorWindow: public QOpenGLWidget{
	Q_OBJECT

	public:
		using Clock = std::chrono::high_resolution_clock;

		explicit QideMapEditorWindow(QWidget *parent = nullptr);

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
		Clock::time_point m_start, m_frameStart;
		QPointF m_lastMousePos;
		qide::Vec3 m_movement;
};

class QideMapEditor: public QWidget{
	Q_OBJECT

	public:
		explicit QideMapEditor(QWidget *parent = nullptr);
};

#endif // !QIDE_QIDEMAPEDITOR_HPP
