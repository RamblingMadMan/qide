#include "fmt/core.h"

#include <QApplication>
#include <QSettings>
#include <QHeaderView>

#include "QideWindow.hpp"

QideWindow::QideWindow(QWidget *parent)
	: QMainWindow(parent)
	, m_menuBar(this)
	, m_fileMenu("File", this)
	, m_editMenu("Edit", this)
	, m_openAction(QIcon::fromTheme("document-open"), "Open", this)
	, m_saveAction(QIcon::fromTheme("document-save"), "Save", this)
	, m_quitAction(QIcon::fromTheme("application-exit"), "Quit", this)
	, m_undoAction(QIcon::fromTheme("edit-undo"), "Undo", this)
	, m_redoAction(QIcon::fromTheme("edit-redo"), "Redo", this)
{
	m_treeView.setSelectionMode(QAbstractItemView::SingleSelection);

	m_treeView.setModel(&m_fsModel);

	auto viewHeader = m_treeView.header();

	viewHeader->hideSection(1);
	viewHeader->hideSection(2);
	viewHeader->hideSection(3);
	viewHeader->hide();

	m_splitter.addWidget(&m_treeView);
	m_splitter.addWidget(&m_qcEdit);

	connect(
		m_treeView.selectionModel(), &QItemSelectionModel::selectionChanged,
		this, [this]{
			auto filePath = m_fsModel.filePath(m_treeView.currentIndex());
			//fmt::print(stderr, "loading file: {}\n", filePath.toStdString());
			m_qcEdit.loadFile(filePath);
		}
	);

	connect(&m_quitAction, &QAction::triggered, qApp, &QApplication::quit);
	connect(&m_undoAction, &QAction::triggered, &m_qcEdit, &QPlainTextEdit::undo);
	connect(&m_redoAction, &QAction::triggered, &m_qcEdit, &QPlainTextEdit::redo);

	connect(&m_qcEdit, &QPlainTextEdit::undoAvailable, &m_undoAction, &QAction::setEnabled);
	connect(&m_qcEdit, &QPlainTextEdit::redoAvailable, &m_redoAction, &QAction::setEnabled);

	m_saveAction.setDisabled(true);
	m_openAction.setDisabled(true);
	m_undoAction.setDisabled(true);
	m_redoAction.setDisabled(true);

	// file menu
	m_fileMenu.addAction(&m_openAction);
	m_fileMenu.addAction(&m_saveAction);
	m_fileMenu.addSeparator();
	m_fileMenu.addAction(&m_quitAction);

	// edit menu
	m_editMenu.addAction(&m_undoAction);
	m_editMenu.addAction(&m_redoAction);

	// menu bar
	m_menuBar.addMenu(&m_fileMenu);
	m_menuBar.addMenu(&m_editMenu);

	// tool bar buttons
	m_toolBar.addAction(&m_openAction);
	m_toolBar.addAction(&m_saveAction);
	m_toolBar.addSeparator();
	m_toolBar.addAction(&m_undoAction);
	m_toolBar.addAction(&m_redoAction);

	setMenuBar(&m_menuBar);
	addToolBar(Qt::TopToolBarArea, &m_toolBar);

	setCentralWidget(&m_splitter);

	readSettings();
}

QideWindow::QideWindow(QDir projectDir_, QWidget *parent_)
	: QideWindow(parent_)
{
	QSettings settings;

	settings.setValue("workDir", projectDir_.path());
	settings.setValue("currentFile", "src/defs.qc");

	readSettings();
}

void QideWindow::setProjectDir(QDir projectDir_){
	QString projPath = projectDir_.path();

	auto dirInfo = QFileInfo(projPath);
	if(!dirInfo.exists() || !dirInfo.isDir()){
		throw std::runtime_error("project directory doesn't exist or is not a directory");
	}

	QSettings settings;

	auto currentFilePath = projPath + "/src/defs.qc";

	settings.setValue("workDir", projPath);
	settings.setValue("currentFile", "src/defs.qc");

	m_fsModel.setRootPath(projectDir_.path());

	auto fsModelIdx = m_fsModel.index(projPath);
	auto fsFileIdx = m_fsModel.index(currentFilePath);

	m_treeView.setRootIndex(fsModelIdx);
	m_treeView.setCurrentIndex(fsFileIdx);
}

void QideWindow::closeEvent(QCloseEvent *event){
	(void)event;

	QSettings settings;

	settings.setValue("title", windowTitle());
	settings.setValue("geometry", saveGeometry());
	settings.setValue("state", saveState());
	settings.setValue("splitState", m_splitter.saveState());
	settings.setValue("fileBufs", QVariant::fromValue(m_qcEdit.fileBuffers()));
	settings.setValue("currentFile", m_qcEdit.fileDir().path());
}

void QideWindow::readSettings(){
	QSettings settings;

	setWindowTitle(settings.value("title", "QIDE").toString());

	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("state").toByteArray());

	m_splitter.restoreState(settings.value("splitState").toByteArray());
	m_qcEdit.setFileBuffers(settings.value("fileBufs").value<QHash<QString, QString>>());

	setProjectDir(settings.value("workDir").toString());
}
