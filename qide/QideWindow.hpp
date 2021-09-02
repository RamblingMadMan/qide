#ifndef QIDE_QIDEWINDOW_HPP
#define QIDE_QIDEWINDOW_HPP 1

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QDir>

class QTabWidget;

class QideEditor;
class QideGame;

class QideTab: public QWidget{
	Q_OBJECT

	public:
		explicit QideTab(QWidget *parent = nullptr);

		explicit QideTab(const QString &str, QWidget *parent = nullptr)
			: QideTab(parent)
		{
			setText(str);
		}

		explicit QideTab(const QIcon &icon, const QString &str, QWidget *parent = nullptr)
			: QideTab(parent)
		{
			setIcon(icon);
			setText(str);
		}

		void setIcon(const QIcon &icon);
		void setText(const QString &str);

		void setProgress(qreal p);
		void showProgress();
		void hideProgress();

	private:
		class QLabel *m_icon, *m_text;
		class QProgressBar *m_progress;
};

class QideWindow: public QMainWindow{
	Q_OBJECT

	Q_PROPERTY(QDir projectDir READ projectDir WRITE setProjectDir NOTIFY projectDirChanged)

	public:
		explicit QideWindow(QWidget *parent_ = nullptr);
		explicit QideWindow(QDir projectDir_, QWidget *parent_ = nullptr);

		const QDir &projectDir() const noexcept{ return m_projectDir; }

		void setProjectDir(QDir projectDir_);

	signals:
		void projectDirChanged();

	protected:
		 void closeEvent(QCloseEvent *event);

	private:
		void downloadFTEQW();

		void readSettings();

		QideEditor *m_editor;
		QideGame *m_game;
		QMenuBar m_menuBar;
		QMenu m_fileMenu, m_editMenu;
		QideTab *m_codeTab, *m_playTab;

		QAction m_openAction, m_saveAction, m_quitAction;
		QAction m_undoAction, m_redoAction;
		QAction m_buildAction, m_launchAction;

		QDir m_projectDir;
};

#endif // !QIDE_QIDEWINDOW_HPP
