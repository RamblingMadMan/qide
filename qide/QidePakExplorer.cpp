#include <QFileInfo>
#include <QStringListModel>
#include <QVBoxLayout>
#include <QListView>
#include <QComboBox>
#include <QStandardItemModel>

#include "physfs.h"

#include "QidePakExplorer.hpp"

QidePakExplorer::QidePakExplorer(QWidget *parent_)
	: QWidget(parent_)
	, m_dirs(new QComboBox(this))
	, m_paks(new QComboBox(this))
	, m_model(new QStringListModel(this))
	, m_view(new QListView(this))
{
	m_view->setContentsMargins(0, 0, 0, 0);
	m_view->setModel(m_model);

	auto lay = new QVBoxLayout(this);
	lay->setSpacing(2);
	lay->setMargin(0);
	lay->addWidget(m_dirs);
	lay->addWidget(m_paks);
	lay->addWidget(m_view);

	refresh();
}

void QidePakExplorer::refresh(){
	auto subDirsPtr = PHYSFS_enumerateFiles("/");

	auto subDir = subDirsPtr;

	m_dirs->clear();

	while(*subDir != nullptr){
		auto dir = QString::fromUtf8(*subDir);
		m_dirs->addItem(dir);
		auto subDirAbs = "/" + std::string(*subDir);
		auto tmpPtr = PHYSFS_enumerateFiles(subDirAbs.c_str());
		if(!tmpPtr || !*tmpPtr){
			QStandardItemModel *model = qobject_cast<QStandardItemModel*>(m_dirs->model());
			bool disabled = true;
			QStandardItem *item = model->item(subDir - subDirsPtr);
			item->setFlags(disabled ? item->flags() & ~Qt::ItemIsEnabled
									: item->flags() | Qt::ItemIsEnabled);
		}
		PHYSFS_freeList(tmpPtr);
		++subDir;
	}

	PHYSFS_freeList(subDirsPtr);

	auto paksPtr = PHYSFS_enumerateFiles("/id1");

	auto pak = paksPtr;

	m_paks->clear();

	while(*pak != nullptr){
		m_paks->addItem(QString::fromUtf8(*pak));
		++pak;
	}

	PHYSFS_freeList(paksPtr);

	auto filesPtr = PHYSFS_enumerateFiles("/id1/pak0");

	auto file = filesPtr;

	QStringList files;

	while(*file != nullptr){
		files.append(QString::fromUtf8(*file));
		++file;
	}

	m_model->setStringList(files);

	PHYSFS_freeList(filesPtr);
}
