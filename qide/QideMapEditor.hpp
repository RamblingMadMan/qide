#ifndef QIDE_QIDEMAPEDITOR_HPP
#define QIDE_QIDEMAPEDITOR_HPP 1

#include <QWidget>
#include <QOpenGLWindow>
#include <QOpenGLFunctions_4_3_Core>

class QideMapEditorWindow: public QOpenGLWindow, protected QOpenGLFunctions_4_3_Core{
	Q_OBJECT

	public:
		explicit QideMapEditorWindow(QWindow *parent = nullptr);

	protected:
		void initializeGL() override;
		void resizeGL(int w, int h) override;
		void paintGL() override;
};

class QideMapEditor: public QWidget{
	Q_OBJECT

	public:
		explicit QideMapEditor(QWidget *parent = nullptr);
};

#endif // !QIDE_QIDEMAPEDITOR_HPP
