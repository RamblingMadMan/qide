#include "fmt/core.h"

#include <QSettings>

#include "QideWindow.hpp"

QideWindow::QideWindow(QWidget *parent): QMainWindow(parent), m_menuBar(this), m_fileMenu("File", this){
	m_treeView.setSelectionMode(QAbstractItemView::SingleSelection);

	m_treeView.setModel(&m_fsModel);

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

	m_menuBar.addMenu(&m_fileMenu);

	setMenuBar(&m_menuBar);

	setCentralWidget(&m_splitter);

	readSettings();
}

QideWindow::QideWindow(QDir projectDir_, QWidget *parent_)
	: QMainWindow(parent_)
	, m_menuBar(this)
	, m_fileMenu("File", this)
	, m_projectDir(std::move(projectDir_))
{
	auto dirInfo = QFileInfo(projectDir_.path());
	if(!dirInfo.exists() || !dirInfo.isDir()){
		throw std::runtime_error("project directory doesn't exist or is not a directory");
	}

	QSettings settings;

	settings.setValue("title", "QIDE");
	settings.setValue("workDir", projectDir_.path());
	settings.setValue("currentFile", "src/defs.qc");

	m_treeView.setSelectionMode(QAbstractItemView::SingleSelection);

	m_treeView.setModel(&m_fsModel);

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

	m_menuBar.addMenu(&m_fileMenu);

	setMenuBar(&m_menuBar);

	setCentralWidget(&m_splitter);

	readSettings();
}

void QideWindow::closeEvent(QCloseEvent *event){
	(void)event;

	QSettings settings;

	settings.setValue("title", windowTitle());
	settings.setValue("state", saveState());
	settings.setValue("geometry", saveGeometry());
	settings.setValue("currentFile", m_qcEdit.fileDir().path());
	settings.setValue("fileBufs", QVariant::fromValue(m_qcEdit.fileBuffers()));
}

void QideWindow::readSettings(){
	QSettings settings;

	setWindowTitle(settings.value("title").toString());
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("state").toByteArray());

	auto fileBufs = settings.value("fileBufs");
	if(fileBufs.isValid()){
		m_qcEdit.setFileBuffers(fileBufs.value<QHash<QString, QString>>());
	}

	auto workDir = settings.value("workDir").toString();
	auto srcDir = workDir + "/src";

	m_fsModel.setRootPath(workDir);

	auto currentFilePath = settings.value("currentFile").toString();

	auto fsModelIdx = m_fsModel.index(srcDir);
	auto fsFileIdx = m_fsModel.index(currentFilePath);

	m_treeView.setRootIndex(fsModelIdx);
	m_treeView.setCurrentIndex(fsFileIdx);

	//m_qcEdit.loadFile(QDir(currentFilePath));
}
