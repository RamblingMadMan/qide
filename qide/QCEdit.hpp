#ifndef QIDE_QCEDIT_HPP
#define QIDE_QCEDIT_HPP

#include <QDir>
#include <QPlainTextEdit>

#include "QCHighlighter.hpp"
#include "QCParser.hpp"

class QCEdit: public QPlainTextEdit{
	Q_OBJECT

	public:
		explicit QCEdit(QWidget *parent = nullptr);
		explicit QCEdit(const QString &text, QWidget *parent = nullptr);

		bool loadFile(const QDir &dir);

		QDir fileDir() const noexcept{ return m_fileDir; }

	signals:
		void fileChanged();

	private:
		void reparse();

		QDir m_fileDir;
		QCParser m_parser;
		QCHighlighter m_highlighter;

		void setDefaultFont();
};

#endif // QIDE_QCEDITOR_HPP
