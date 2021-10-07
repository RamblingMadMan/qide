#ifndef QIDE_QIDESEARCHBAR_HPP
#define QIDE_QIDESEARCHBAR_HPP 1

#include <QWidget>

class QLineEdit;
class QListView;

class QideSearchBar: public QWidget{
	Q_OBJECT

	public:
		explicit QideSearchBar(QWidget *parent = nullptr);

		QString text() const;
		QStringList results() const{ return m_results; }

		void setText(const QString &text_);
		void setResults(const QStringList &results_);

	public slots:
		void showResults();
		void hideResults();

	signals:
		void textChanged();
		void resultsChanged();
		void triggered();

	protected:
		void paintEvent(QPaintEvent* event);

	private:
		QLineEdit *m_entry;
		QListView *m_dropDown;
		QStringList m_results;
};

#endif // !QIDE_QIDESEARCHBAR_HPP
