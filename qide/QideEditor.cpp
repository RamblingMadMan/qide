#include <QHeaderView>
#include <QHBoxLayout>
#include <QPainter>

#include "QCEdit.hpp"
#include "QideEditor.hpp"

#include "QuakeColors.hpp"

QideEditor::QideEditor(QWidget *parent)
	: QWidget(parent)
	, m_lay(new QHBoxLayout(this))
	, m_splitter(new QSplitter)
	, m_qcEdit(new QCEdit(m_splitter))
	, m_treeView(new QTreeView(m_splitter))
	, m_fsModel(new QFileSystemModel(this))
{
	setContentsMargins(0, 0, 0, 0);

	m_lay->setContentsMargins(0, 0, 0, 0);
	m_lay->addWidget(m_splitter);

	m_splitter->setContentsMargins(0, 0, 0, 0);
	m_splitter->addWidget(m_treeView);
	m_splitter->addWidget(m_qcEdit);

	m_qcEdit->setContentsMargins(0, 0, 0, 0);

	m_treeView->setContentsMargins(0, 0, 0, 0);
	m_treeView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_treeView->setModel(m_fsModel);

	auto viewHeader = m_treeView->header();

	viewHeader->hideSection(1);
	viewHeader->hideSection(2);
	viewHeader->hideSection(3);
	viewHeader->hide();

	connect(
		m_treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
		this, [this]{
			auto filePath = QDir(m_fsModel->filePath(m_treeView->currentIndex())).absolutePath();

			m_qcEdit->loadFile(filePath);
		}
	);

	setOpacity(1.0);
}

void QideEditor::saveCurrent(){
	auto filePath = m_qcEdit->fileDir().absolutePath();
	QFile outFile(filePath);
	if(!outFile.open(QFile::WriteOnly)){
		return;
	}

	outFile.write(m_qcEdit->toPlainText().toUtf8());
}

void QideEditor::setRootDir(QDir dir){
	QString path = dir.path();

	auto dirInfo = QFileInfo(path);
	if(!dirInfo.exists() || !dirInfo.isDir()){
		throw std::runtime_error("project directory doesn't exist or is not a directory");
	}

	auto filePath = path + "/src/defs.qc";

	m_fsModel->setRootPath(path);

	auto dirModelIdx = m_fsModel->index(path);
	auto fileModelIdx = m_fsModel->index(filePath);

	m_treeView->setRootIndex(dirModelIdx);
	m_treeView->setCurrentIndex(fileModelIdx);
}

void QideEditor::setOpacity(qreal a){
	auto p = palette();

	QPalette::ColorRole roles[] = {
		QPalette::Window,
		QPalette::Base
	};

	for(auto role : roles){
		auto col = p.color(role);
		col.setAlphaF(a);
		p.setColor(role, col);
	}

	m_treeView->setPalette(p);
	setPalette(p);
}
