#include <QDebug>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QPainter>
#include <QShortcut>
#include <QToolBar>
#include <QToolButton>
#include <QLineEdit>

#include "QCEdit.hpp"
#include "QCCompleter.hpp"
#include "QideEditor.hpp"

#include "QuakeColors.hpp"

QideFSModel::QideFSModel(QObject *parent_)
	: QFileSystemModel(parent_){}

QVariant QideFSModel::data(const QModelIndex &index, int role) const{
	auto oldData = QFileSystemModel::data(index, role);

	if(role != Qt::DisplayRole){ return oldData; }

	// TODO: add asterisk to modified file names

	return oldData;
}

QideEditor::QideEditor(QWidget *parent)
	: QWidget(parent)
	, m_lay(new QVBoxLayout(this))
	, m_splitter(new QSplitter)
	, m_qcEdit(new QCEdit(m_splitter))
	, m_treeView(new QTreeView(m_splitter))
	, m_fsModel(new QideFSModel(this))
{
	setAttribute(Qt::WA_TranslucentBackground);
	setContentsMargins(0, 0, 0, 0);

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

	// Search bar
	auto searchBar = new QToolBar(this);
	searchBar->setContentsMargins(0, 0, 0, 0);

	searchBar->setStyleSheet(
		"QToolBar {"
			"padding: 0 0 0 0;"
		"}"

		"QLineEdit {"
			"padding: 0 0 0 5px;"
			"border-radius: 0;"
		"}"
	);

	auto searchEntry = new QLineEdit(searchBar);
	searchEntry->setContentsMargins(0, 0, 0, 0);
	searchEntry->setPlaceholderText("Search...");

	auto searchIcon = QImage(":/img/ui/search.svg");
	searchIcon.invertPixels();

	auto searchAction = new QAction(searchBar);
	searchAction->setIcon(QIcon(QPixmap::fromImage(searchIcon)));
	searchAction->setToolTip("Search");

	connect(searchEntry, &QLineEdit::textChanged, [=]{
		if(searchEntry->text().isEmpty()){
			searchAction->setEnabled(false);
		}
		else{
			searchAction->setEnabled(true);
		}
	});

	connect(searchEntry, &QLineEdit::returnPressed, [=]{
		if(searchAction->isEnabled()){
			searchAction->trigger();
		}
	});

	connect(searchAction, &QAction::triggered, [this]{
		// TODO: search m_qcEdit
	});

	searchAction->setEnabled(false);

	searchBar->addWidget(searchEntry);
	searchBar->addAction(searchAction);

	m_lay->setContentsMargins(0, 0, 0, 0);
	m_lay->addWidget(searchBar);
	m_lay->addWidget(m_splitter);

	setOpacity(1.0);
}

void QideEditor::saveCurrent(){
	m_qcEdit->saveFile();
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

	auto completerP = p;

	for(auto role : roles){
		auto col = p.color(role);
		col.setAlphaF(1.f);
		completerP.setColor(role, col);
	}

	m_qcEdit->completer()->setPalette(completerP);
}

void QideEditor::showCompleter(){
	m_qcEdit->showCompleter();
}

void QideEditor::hideCompleter(){
	m_qcEdit->hideCompleter();
}

void QideEditor::setFont(const QFont &fnt){
	m_qcEdit->setFont(fnt);
}
