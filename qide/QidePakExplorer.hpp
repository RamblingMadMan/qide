#ifndef QIDEPAKEXPLORER_HPP
#define QIDEPAKEXPLORER_HPP

#include <QObject>
#include <QWidget>

class QComboBox;
class QStringListModel;
class QListView;

class QidePakExplorer : public QWidget{
	Q_OBJECT

	public:
		explicit QidePakExplorer(QWidget *parent_ = nullptr);

	public slots:
		void refresh();

	private:
		QComboBox *m_dirs, *m_paks;
		QStringListModel *m_model;
		QListView *m_view;
		QString m_path;
};

#endif // QIDEPAKEXPLORER_HPP
