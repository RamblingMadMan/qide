#ifndef QIDE_QIDEEDITOR_HPP
#define QIDE_QIDEEDITOR_HPP 1

#include <QSplitter>
#include <QTreeView>
#include <QFileSystemModel>

class QHBoxLayout;

#include "QCEdit.hpp"

class QideEditor: public QWidget{
	Q_OBJECT

	Q_PROPERTY(QDir rootDir READ rootDir WRITE setRootDir NOTIFY rootDirChanged)
	Q_PROPERTY(QSplitter* splitter READ splitter)
	Q_PROPERTY(QCEdit* qcEdit READ qcEdit)
	Q_PROPERTY(QTreeView* treeView READ treeView)
	Q_PROPERTY(QHash<QString, QString> fileBuffers READ fileBuffers WRITE setFileBuffers NOTIFY fileBuffersChanged)

	public:
		explicit QideEditor(QWidget *parent_ = nullptr);

		const QDir &rootDir() const noexcept{ return m_rootDir; }

		void setRootDir(QDir dir);

		void setFileBuffers(const QHash<QString, QString> &fileBufs){
			// TODO: handle open buffers with modifications
			m_fileBufs = fileBufs;
			emit fileBuffersChanged();
		}

		QSplitter *splitter() noexcept{ return m_splitter; }

		QCEdit *qcEdit() noexcept{ return m_qcEdit; }
		const QCEdit *qcEdit() const noexcept{ return m_qcEdit; }

		QTreeView *treeView() noexcept{ return m_treeView; }
		const QTreeView *treeView() const noexcept{ return m_treeView; }

		const QFileSystemModel &fsModel() const noexcept{ return m_fsModel; }

		const QHash<QString, QString> &fileBuffers() const noexcept{ return m_fileBufs; }

	signals:
		void rootDirChanged();
		void fileBuffersChanged();

	private:
		QHBoxLayout *m_lay;
		QCEdit *m_qcEdit;
		QSplitter *m_splitter;
		QTreeView *m_treeView;
		QDir m_rootDir;
		QHash<QString, QString> m_fileBufs;
		QFileSystemModel m_fsModel;
};

#endif // !QIDE_QIDEEDITOR_HPP
