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
			VarDef,
			Ref,
			EndOfFile,
			Unknown,
			KindCount
		};

		Q_ENUM(Kind)

		struct VarInfo{
			QCType type;
			QString name;
		};

		explicit QCExpr(QObject *parent_ = nullptr)
			: QObject(parent_)
			, m_kind(Unknown)
			, m_tokStart(nullptr)
			, m_tokEnd(nullptr)
		{}

		void setKind(Kind kind_){
			m_kind = kind_;
		}

		void setTokRange(const QCToken *start, const QCToken *end_){
			m_tokStart = start;
			m_tokEnd = end_;
		}

		static QCExpr *makeEOF(const QCToken *eof, QObject *parent = nullptr){
			return new QCExpr(EndOfFile, eof, eof, parent);
		}

		static QCExpr *makeVarDef(QCType ty, QString name, const QCToken *tokStart, const QCToken *tokEnd, QObject *parent = nullptr){
			return new QCExpr(ty, name, tokStart, tokEnd, parent);
		}

		Kind kind() const noexcept{ return m_kind; }

		const QCToken *begin() const noexcept{ return m_tokStart; }
		const QCToken *end() const noexcept{ return m_tokEnd; }

		QStringView str() const noexcept{
			return QStringView(m_tokStart->str().cbegin(), (m_tokEnd - 1)->str().cend());
		}

		QString name() const noexcept{
			if(m_kind == VarDef){
				return m_values[1].toString();
			}
			else{
				return "";
			}
		}

	private:
		QCExpr(Kind kind_, const QCToken *tokStart, const QCToken *tokEnd, QObject *parent)
			: QObject(parent), m_kind(kind_), m_tokStart(tokStart), m_tokEnd(tokEnd){}

		QCExpr(QCType ty_, QString name_, const QCToken *tokStart, const QCToken *tokEnd, QObject *parent)
			: QObject(parent), m_kind(VarDef), m_tokStart(tokStart), m_tokEnd(tokEnd)
		{
			m_values.push_back(QVariant::fromValue(ty_));
			m_values.push_back(name_);
		}

		Kind m_kind;
		const QCToken *m_tokStart, *m_tokEnd;
		QVector<QVariant> m_values;
};

#endif // !QIDE_QCEXPR_HPP
