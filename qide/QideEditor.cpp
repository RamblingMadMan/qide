#include <QHeaderView>
#include <QHBoxLayout>

#include "QideEditor.hpp"

QideEditor::QideEditor(QWidget *parent)
	: QWidget(parent)
	, m_lay(new QHBoxLayout(this))
	, m_qcEdit(new QCEdit(this))
	, m_splitter(new QSplitter(this))
	, m_treeView(new QTreeView(this))
{
	setContentsMargins(0, 0, 0, 0);

	m_lay->setContentsMargins(0, 0, 0, 0);
	m_lay->addWidget(m_splitter);

	setLayout(m_lay);

	m_qcEdit->setContentsMargins(0, 0, 0, 0);

	m_treeView->setContentsMargins(0, 0, 0, 0);
	m_treeView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_treeView->setModel(&m_fsModel);

	auto viewHeader = m_treeView->header();

	viewHeader->hideSection(1);
	viewHeader->hideSection(2);
	viewHeader->hideSection(3);
	viewHeader->hide();

	m_splitter->setContentsMargins(0, 0, 0, 0);
	m_splitter->addWidget(m_treeView);
	m_splitter->addWidget(m_qcEdit);

	connect(
		m_treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
		this, [this]{
			auto filePath = m_fsModel.filePath(m_treeView->currentIndex());
			m_qcEdit->loadFile(filePath);
		}
	);
}

void QideEditor::setRootDir(QDir dir){
	QString path = dir.path();

	auto dirInfo = QFileInfo(path);
	if(!dirInfo.exists() || !dirInfo.isDir()){
		throw std::runtime_error("project directory doesn't exist or is not a directory");
	}

	auto filePath = path + "/src/defs.qc";

	m_fsModel.setRootPath(path);

	auto dirModelIdx = m_fsModel.index(path);
	auto fileModelIdx = m_fsModel.index(filePath);

	m_treeView->setRootIndex(dirModelIdx);
	m_treeView->setCurrentIndex(fileModelIdx);
}
