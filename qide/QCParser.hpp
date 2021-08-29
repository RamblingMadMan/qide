#ifndef QIDE_QCPARSER_HPP
#define QIDE_QCPARSER_HPP 1

#include <optional>
#include <functional>

#include <QVector>
#include <QMap>

#include "QCToken.hpp"
#include "QCExpr.hpp"
#include "QCType.hpp"

class QCParser: public QObject{
	Q_OBJECT

	Q_PROPERTY(QVector<QCExpr> results READ results NOTIFY resultsChanged)

	public:
		using Location = QCToken::Location;

		QCParser();

		void reset();

		// returns false on error
		bool parse(const QCToken *beg, const QCToken *end);

		const QVector<QCExpr> results() const noexcept{ return m_results; }

		const QCExpr *atLocation(Location loc) const{
			auto res = m_locMap.find(loc);
			if(res != m_locMap.end()) return res.value();
			else return nullptr;
		}

	signals:
		void resultsChanged();

	private:
		using ParseResult = std::variant<bool, QCExpr>;

		ParseResult parseDef(QCType ty, const QCToken *exprStart, const QCToken *beg, const QCToken *end);
		ParseResult parseToplevel(const QCToken *beg, const QCToken *end);

		QVector<QCExpr> m_results;
		std::function<ParseResult(const QCToken *beg, const QCToken *end)> m_parseFn;
		QMap<Location, QCExpr*> m_locMap;
};

#endif // !QIDE_QCPARSER_HPP
