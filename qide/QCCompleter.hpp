#ifndef QIDE_QCCOMPLETER_HPP
#define QIDE_QCCOMPLETER_HPP 1

#include <cassert> // fuzz actually needs this in debug builds
#include "rapidfuzz/fuzz.hpp"

#include <QObject>
#include <QWidget>
#include <QListView>
#include <QWidget>
#include <QStringListModel>

class QCCompleter: public QObject{
	Q_OBJECT

	public:
		explicit QCCompleter(class QCEdit *qcEdit = nullptr, QObject *parent = nullptr);

		QStringList complete(const QString &tok);

		void setQcEdit(class QCEdit *qcEdit);

		void addChoice(const QString &choice){
			m_choices.append(choice);
			emit choicesChanged();
		}

		void setChoices(QStringList choices_){
			m_choices = std::move(choices_);
			emit choicesChanged();
		}

		const QStringList &choices() const noexcept{ return m_choices; }

		class QCEdit *qcEdit() noexcept{ return m_qcEdit; }

	signals:
		void qcEditChanged();
		void choicesChanged();

	public slots:
		void closePopup();
		void completeAtCursor();

	private:
		class QCEdit *m_qcEdit;
		QWidget m_popup;
		QListView m_listView;
		QStringListModel m_model;
		QStringList m_choices;
};

#endif // !QIDE_QCCOMPLETER_HPP
