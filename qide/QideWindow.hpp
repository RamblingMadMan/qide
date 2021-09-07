#ifndef QIDE_QIDEWINDOW_HPP
#define QIDE_QIDEWINDOW_HPP 1

#include <QMainWindow>
#include <QAbstractButton>
#include <QAbstractListModel>
#include <QDir>

class QPushButton;

class QTabWidget;

class QCVM;
class QideEditor;
class QideMapEditor;
class QideGame;
class QideCompiler;
class QideTabsWidget;

class QideTabsWidget: public QWidget{
	Q_OBJECT

	Q_PROPERTY(QPushButton* codeTab READ codeTab)
	Q_PROPERTY(QPushButton* mapTab READ mapTab)
	Q_PROPERTY(QPushButton* playTab READ playTab)
	Q_PROPERTY(QPushButton* selectedTab READ selectedTab NOTIFY selectedTabChanged)

	public:
		explicit QideTabsWidget(QWidget *parent = nullptr);

		QPushButton *codeTab() noexcept{ return m_codeTab; }
		QPushButton *mapTab() noexcept{ return m_mapTab; }
		QPushButton *playTab() noexcept{ return m_playTab; }
		QPushButton *selectedTab() noexcept{ return m_selected; }

	signals:
		void selectedTabChanged();

	private:
		QPushButton *m_codeTab = nullptr;
		QPushButton *m_mapTab = nullptr;
		QPushButton *m_playTab = nullptr;
		QPushButton *m_selected = nullptr;
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

		void writeSettings();
		void readSettings();
		void readProjSettings();

		QideTabsWidget *m_tabs;
		QideEditor *m_editor;
		QideMapEditor *m_mapEditor;
		QideGame *m_game;
		QideCompiler *m_comp;
		QCVM *m_vm;

		QAction *m_undoAction, *m_redoAction;

		QVector<QAction*> m_actions;

		QDir m_projectDir;
};

#endif // !QIDE_QIDEWINDOW_HPP
