#ifndef QIDE_QCLEXER_HPP
#define QIDE_QCLEXER_HPP 1

#include <QObject>

#include "QCToken.hpp"

class QCLexer: public QObject{
	Q_OBJECT

	Q_PROPERTY(QVector<QCToken> tokens READ tokens NOTIFY tokensChanged)

	public:
		enum Mode{
			Normal, MultilineComment
		};

		Q_ENUM(Mode)

		using StrIter = QStringView::const_iterator;

		explicit QCLexer(QCToken::Location loc = { 0, 0 })
			: QObject(), m_curLoc{loc}{}

		void reset();

		int lex(StrIter beg, StrIter end);

		int lex(QStringView src){ return lex(std::cbegin(src), std::cend(src)); }

		QCToken::Location curLocation() const noexcept{
			return m_curLoc;
		}

		void setCurLocation(QCToken::Location loc){
			m_curLoc = loc;
		}

		const QVector<QCToken> &tokens() const noexcept{ return m_tokens; }

	signals:
		void tokensChanged();

	private:
		QCToken lexNormal(StrIter it, StrIter end);
		QCToken lexMultilineComment(StrIter it, StrIter end);

		QCToken::Location m_curLoc;
		Mode m_mode = Normal;
		QVector<QCToken> m_tokens;
};

#endif // !QIDE_QCLEXER_HPP
