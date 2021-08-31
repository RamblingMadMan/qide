#ifndef QIDE_QIDEWINDOW_HPP
#define QIDE_QIDEWINDOW_HPP 1

#include <QMainWindow>
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

		QDir m_projectDir;
		QFileSystemModel m_fsModel;
		QSplitter m_splitter;
		QTreeView m_treeView;
		QCEdit m_qcEdit;
};

#endif // !QIDE_QIDEWINDOW_HPP
