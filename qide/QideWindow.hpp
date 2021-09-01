#ifndef QIDE_QIDEWINDOW_HPP
#define QIDE_QIDEWINDOW_HPP 1

#include <QVBoxLayout>
#include <QMainWindow>
#include <QMenuBar>
#include <QSplitter>
#include <QTreeView>
#include <QFileSystemModel>

#include "QCEdit.hpp"

class QideWindow: public QMainWindow{
	Q_OBJECT

	public:
		explicit QideWindow(QWidget *parent_ = nullptr);
		explicit QideWindow(QDir projectDir_, QWidget *parent_ = nullptr);

	protected:
		 void closeEvent(QCloseEvent *event);

	private:
		void readSettings();

		QMenuBar m_menuBar;
		QMenu m_fileMenu;

		QDir m_projectDir;
		QFileSystemModel m_fsModel;
		QSplitter m_splitter;
		QTreeView m_treeView;
		QCEdit m_qcEdit;
		QHash<QString, QString> m_fileBufs;
};

#endif // !QIDE_QIDEWINDOW_HPP
