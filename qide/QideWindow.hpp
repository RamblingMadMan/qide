#ifndef QIDE_QIDEWINDOW_HPP
#define QIDE_QIDEWINDOW_HPP 1

#include <QVBoxLayout>
#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QSplitter>
#include <QTreeView>
#include <QFileSystemModel>

#include "QCEdit.hpp"

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
		void readSettings();

		QMenuBar m_menuBar;
		QMenu m_fileMenu, m_editMenu;;

		QAction m_openAction, m_saveAction, m_quitAction;
		QAction m_undoAction, m_redoAction;

		QToolBar m_toolBar;

		QDir m_projectDir;
		QFileSystemModel m_fsModel;
		QSplitter m_splitter;
		QTreeView m_treeView;
		QCEdit m_qcEdit;
		QHash<QString, QString> m_fileBufs;
};

#endif // !QIDE_QIDEWINDOW_HPP
