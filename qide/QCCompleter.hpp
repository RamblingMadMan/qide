#ifndef QIDE_QCCOMPLETER_HPP
#define QIDE_QCCOMPLETER_HPP 1

#include <cassert> // fuzz actually needs this in debug builds
#include "rapidfuzz/fuzz.hpp"

#include <QObject>
#include <QWidget>
#include <QListView>
#include <QWidget>
#include <QStringListModel>

class QKeyEvent;
class QCEdit;

class QCCompleter: public QWidget{
	Q_OBJECT

	public:
		explicit QCCompleter(QCEdit *qcEdit = nullptr, QWidget *parent = nullptr);

		QStringList complete(const QString &tok);

		void setQcEdit(QCEdit *qcEdit);

		void addChoice(const QString &choice){
			m_choices.append(choice);
			emit choicesChanged();
		}

		void setChoices(QStringList choices_){
			m_choices = choices_;
			emit choicesChanged();
		}

		const QStringList &choices() const noexcept{ return m_choices; }

		QCEdit *qcEdit() noexcept{ return m_qcEdit; }

	signals:
		void qcEditChanged();
		void choicesChanged();

	public slots:
		void closePopup();
		void completeAtCursor(bool forceShow = false);

	protected:
		void keyPressEvent(QKeyEvent *ev) override;

	private:
		QCEdit *m_qcEdit;
		QListView m_listView;
		QStringListModel m_model;
		QStringList m_choices;
};

#endif // !QIDE_QCCOMPLETER_HPP
