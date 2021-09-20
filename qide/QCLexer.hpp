#ifndef QIDE_QCLEXER_HPP
#define QIDE_QCLEXER_HPP 1

#include <QObject>
#include <QVector>

#include "QCToken.hpp"

class QCLexer: public QObject{
	Q_OBJECT

	Q_PROPERTY(bool skipSpaces READ skipSpaces WRITE setSkipSpaces NOTIFY skipSpacesChanged)
	Q_PROPERTY(bool skipNewLines READ skipNewLines WRITE setSkipNewLines NOTIFY skipNewLinesChanged)
	Q_PROPERTY(QVector<QCToken> tokens READ tokens NOTIFY tokensChanged)

	public:
		enum Mode{
			Normal, MultilineComment
		};

		Q_ENUM(Mode)

		using StrIter = QStringView::const_iterator;

		explicit QCLexer(QCToken::Location loc, QObject *parent = nullptr)
			: QObject(parent), m_curLoc{loc}{}

		explicit QCLexer(QObject *parent = nullptr): QCLexer({ 0, 0 }, parent){}

		QCToken::Location curLocation() const noexcept{
			return m_curLoc;
		}

		void setCurLocation(QCToken::Location loc){
			m_curLoc = loc;
		}

		bool skipSpaces() const noexcept{ return m_skipWs; }
		bool skipNewLines() const noexcept{ return m_skipNl; }

		const QVector<QCToken> &tokens() const noexcept{ return m_tokens; }

		const QCToken *closest(QCToken::Location loc) const{
			for(auto &&tok : m_tokens){
				auto &&tokLoc = tok.location();
				if(tokLoc.line != loc.line) continue;

				const auto tokLen = tok.str().size();

				if((loc.col >= tokLoc.col) && (loc.col <= (tokLoc.col + tokLen))){
					return &tok;
				}
			}

			return nullptr;
		}

		void setSkipSpaces(bool skip){
			if(skip == m_skipWs) return;
			m_skipWs = skip;
			emit skipSpacesChanged();
		}

		void setSkipNewLines(bool skip){
			if(skip == m_skipNl) return;
			m_skipNl = skip;
			emit skipNewLinesChanged();
		}

		int lex(StrIter beg, StrIter end);
		int lex(QStringView src){ return lex(std::cbegin(src), std::cend(src)); }

	public slots:
		void reset();

	signals:
		void skipSpacesChanged();
		void skipNewLinesChanged();
		void tokensChanged();

	private:
		QCToken lexNormal(StrIter it, StrIter end);
		QCToken lexMultilineComment(StrIter it, StrIter end);

		bool m_skipWs, m_skipNl;
		QCToken::Location m_curLoc;
		Mode m_mode = Normal;
		QVector<QCToken> m_tokens;
};

#endif // !QIDE_QCLEXER_HPP
