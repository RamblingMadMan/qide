#ifndef QIDE_QIDEWINDOW_HPP
#define QIDE_QIDEWINDOW_HPP 1

#include <QMainWindow>
#include <QAbstractListModel>
#include <QMenuBar>
#include <QToolBar>
#include <QDir>

class QTabWidget;

class QideEditor;
class QideGame;
class QideCompiler;
class QideTabsWidget;

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

class QideTabsWidget: public QWidget{
	Q_OBJECT

	Q_PROPERTY(QideTab* codeTab READ codeTab)
	Q_PROPERTY(QideTab* playTab READ playTab)
	Q_PROPERTY(QideTab* selectedTab READ selectedTab NOTIFY selectedTabChanged)

	public:
		explicit QideTabsWidget(QWidget *parent = nullptr);

		QideTab *codeTab() noexcept{ return m_codeTab; }
		QideTab *playTab() noexcept{ return m_playTab; }
		QideTab *selectedTab() noexcept{ return m_selected; }

	signals:
		void selectedTabChanged();

	private:
		QideTab *m_codeTab = nullptr;
		QideTab *m_playTab = nullptr;
		QideTab *m_selected = nullptr;
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
		struct Ctor{};

		QideWindow(Ctor, QWidget *parent);

		void downloadFTEQW();

		void writeSettings();
		void readSettings();
		void readProjSettings();

		QideTabsWidget *m_tabs;
		QideEditor *m_editor;
		QideGame *m_game;
		QideCompiler *m_comp;
		QMenuBar m_menuBar;
		QMenu m_fileMenu, m_editMenu;

		QAction *m_undoAction, *m_redoAction;

		QVector<QAction*> m_actions;

		QDir m_projectDir;
};

#endif // !QIDE_QIDEWINDOW_HPP
