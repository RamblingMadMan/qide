#ifndef QIDE_QIDEEDITOR_HPP
#define QIDE_QIDEEDITOR_HPP 1

#include <QSplitter>
#include <QTreeView>
#include <QFileSystemModel>

class QHBoxLayout;
class QFileSystemModel;

class QCEdit;

class QideEditor: public QWidget{
	Q_OBJECT

	Q_PROPERTY(QDir rootDir READ rootDir WRITE setRootDir NOTIFY rootDirChanged)
	Q_PROPERTY(QSplitter* splitter READ splitter)
	Q_PROPERTY(QCEdit* qcEdit READ qcEdit)
	Q_PROPERTY(QTreeView* treeView READ treeView)
	Q_PROPERTY(QFileSystemModel* fsModel READ fsModel)

	public:
		explicit QideEditor(QWidget *parent_ = nullptr);

		const QDir &rootDir() const noexcept{ return m_rootDir; }

		void saveCurrent();

		void setRootDir(QDir dir);

		QSplitter *splitter() noexcept{ return m_splitter; }

		QCEdit *qcEdit() noexcept{ return m_qcEdit; }
		const QCEdit *qcEdit() const noexcept{ return m_qcEdit; }

		QTreeView *treeView() noexcept{ return m_treeView; }
		const QTreeView *treeView() const noexcept{ return m_treeView; }

		QFileSystemModel *fsModel() noexcept{ return m_fsModel; }
		const QFileSystemModel *fsModel() const noexcept{ return m_fsModel; }

	signals:
		void rootDirChanged();
		void fileBufferChanged(const QString &filePath);

	private:
		QHBoxLayout *m_lay;
		QSplitter *m_splitter;
		QCEdit *m_qcEdit;
		QTreeView *m_treeView;
		QFileSystemModel *m_fsModel;
		QDir m_rootDir;
};

#endif // !QIDE_QIDEEDITOR_HPP
