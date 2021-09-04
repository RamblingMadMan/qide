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

	Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
	Q_PROPERTY(QVector<QCExpr> results READ results NOTIFY resultsChanged)

	public:
		using Location = QCToken::Location;

		explicit QCParser(QObject *parent = nullptr);

		void setTitle(QString str);

		// returns negative on error
		int parse(const QCToken *beg, const QCToken *end);

		const QString &title() const noexcept{ return m_title; }

		const QVector<QCExpr> &results() const noexcept{ return m_results; }

		const QCExpr *atLocation(Location loc) const{
			auto res = m_locMap.find(loc);
			if(res != m_locMap.end()){
				return &m_results[res.value()];
			}
			else{
				return nullptr;
			}
		}

	public slots:
		void reset();

	signals:
		void titleChanged();
		void resultsChanged();

	private:
		using ParseResult = std::variant<bool, QCExpr>;

		ParseResult parseDef(QCType ty, const QCToken *exprStart, const QCToken *beg, const QCToken *end);
		ParseResult parseToplevel(const QCToken *beg, const QCToken *end);

		QString m_title;
		QVector<QCExpr> m_results;

		std::function<ParseResult(const QCToken *beg, const QCToken *end)> m_parseFn;
		QMap<Location, int> m_locMap;
};

#endif // !QIDE_QCPARSER_HPP
