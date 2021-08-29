#ifndef QIDE_QCLEXER_HPP
#define QIDE_QCLEXER_HPP 1

#include <QObject>

#include "QCToken.hpp"

class QCLexer: public QObject{
	Q_OBJECT

	Q_PROPERTY(QString src READ src WRITE setSrc NOTIFY srcChanged)

	public:
		enum Mode{
			Normal, MultilineComment
		};

		Q_ENUM(Mode)

		explicit QCLexer(const QString &src = QString(), QCToken::Location loc = { 0, 0 })
			: QObject(), m_curLoc{loc}, m_src(src), m_rem(m_src){}

		void reset();

		QCToken lex();

		bool hasTokens() const noexcept{ return !m_rem.isEmpty(); }

		const QString &src() const noexcept{
			return m_src;
		}

		QCToken::Location curLocation() const noexcept{
			return m_curLoc;
		}

		void setSrc(const QString &src, bool resetLocation = true){
			m_src = src;
			m_rem = m_src;
			emit srcChanged();

			if(resetLocation){
				m_curLoc = { 0, 0 };
			}
		}

		void setCurLocation(QCToken::Location loc){
			m_curLoc = loc;
		}

	signals:
		void srcChanged();

	private:
		QCToken lexNormal(QStringView::iterator it, QStringView::iterator end);
		QCToken lexMultilineComment(QStringView::iterator it, QStringView::iterator end);

		QCToken::Location m_curLoc;
		QString m_src;
		QStringView m_rem;
		Mode m_mode = Normal;
};

#endif // !QIDE_QCLEXER_HPP
