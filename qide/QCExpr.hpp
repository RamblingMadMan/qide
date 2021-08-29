#ifndef QIDE_QCEXPR_HPP
#define QIDE_QCEXPR_HPP 1

#include <QObject>

#include "QCToken.hpp"
#include "QCType.hpp"

class QCExpr: public QObject{
	Q_OBJECT

	public:
		using Location = QCToken::Location;

		enum Kind{
			VarDef, Ref,
			EndOfFile, Unknown,
			count
		};

		Q_ENUM(Kind)

		QCExpr()
			: QObject()
			, m_kind(Unknown)
			, m_tokStart(nullptr)
			, m_tokEnd(nullptr)
		{}

		QCExpr(const QCExpr &other)
			: QObject()
			, m_kind(other.m_kind)
			, m_tokStart(other.m_tokStart)
			, m_tokEnd(other.m_tokEnd)
		{}

		static QCExpr makeEOF(const QCToken *eof){ return QCExpr(EndOfFile, eof, eof); }

		static QCExpr makeVarDef(QCType ty, QString name, const QCToken *tokStart, const QCToken *tokEnd){
			return QCExpr(ty, name, tokStart, tokEnd);
		}

		QCExpr &operator=(const QCExpr &other){
			m_kind = other.m_kind;
			return *this;
		}

		Kind kind() const noexcept{ return m_kind; }

		const QCToken *begin() const noexcept{ return m_tokStart; }
		const QCToken *end() const noexcept{ return m_tokEnd; }

	private:
		QCExpr(Kind kind_, const QCToken *tokStart, const QCToken *tokEnd)
			: QObject(), m_kind(kind_), m_tokStart(tokStart), m_tokEnd(tokEnd){}

		QCExpr(QCType ty_, QString name_, const QCToken *tokStart, const QCToken *tokEnd)
			: QObject(), m_kind(VarDef), m_tokStart(tokStart), m_tokEnd(tokEnd)
		{
			m_values.push_back(QVariant::fromValue(ty_));
			m_values.push_back(name_);
		}

		Kind m_kind;
		const QCToken *m_tokStart, *m_tokEnd;
		QVector<QVariant> m_values;
};

Q_DECLARE_METATYPE(QCExpr)

#endif // !QIDE_QCEXPR_HPP
