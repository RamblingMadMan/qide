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
#include "QideSearchBar.hpp"
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

	setStyleSheet(
		QString("background-color: %1;").arg(quakeDarkGrey.name(QColor::HexRgb))
	);

	m_treeView->setStyleSheet("border-radius: 0;");

	auto editWidget = new QWidget(m_splitter);
	editWidget->setContentsMargins(0, 0, 0, 0);

	auto editLay = new QVBoxLayout(editWidget);
	editLay->setMargin(0);
	editLay->setSpacing(0);
	editLay->setContentsMargins(0, 0, 0, 0);

	auto searchBar = new QideSearchBar(this);

	connect(searchBar, &QideSearchBar::textChanged, [this]{
		// TODO: update results and show drop down
	});

	connect(this, &QideEditor::opacityChanged, searchBar, [searchBar](qreal a){
		auto barColor = quakeDarkGrey.darker(150);
		barColor.setAlpha(a);

		searchBar->setStyleSheet(
			QString("background-color: %1;").arg(barColor.name(QColor::HexRgb))
		);
	});

	editLay->addWidget(searchBar);
	editLay->addWidget(m_qcEdit);
	editWidget->setLayout(editLay);

	m_splitter->setContentsMargins(0, 0, 0, 0);
	m_splitter->addWidget(m_treeView);
	m_splitter->addWidget(editWidget);

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

	m_lay->setContentsMargins(0, 0, 0, 0);
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

	emit opacityChanged(a);
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
