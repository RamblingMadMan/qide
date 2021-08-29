#ifndef QIDE_QCTOKEN_HPP
#define QIDE_QCTOKEN_HPP 1

#include <QObject>
#include <QMetaEnum>
#include <QString>

class QCToken: public QObject{
	Q_OBJECT

	public:
		struct Location{
			int line, col;

			inline friend bool operator<(const Location &lhs, const Location &rhs) noexcept{
				if(lhs.line == rhs.line) return lhs.col < rhs.col;
				else return lhs.line < rhs.line;
			}
		};

		enum Kind{
			Type, Id, Number, String, Term, GlobalId, Comment, Op, EndOfFile, Unknown,
			count
		};

		Q_ENUM(Kind)

		QCToken(Kind kind_, QStringView str_, Location loc = { 0, 0 })
			: QObject(), m_kind(kind_), m_str(str_), m_loc(loc){}

		QCToken(const QCToken &other)
			: QObject()
			, m_kind(other.m_kind)
			, m_str(other.m_str)
			, m_loc(other.m_loc)
		{}
		//QCToken(QCToken&&) noexcept = default;

		QCToken &operator=(const QCToken &other){
			m_kind = other.m_kind;
			m_str = other.m_str;
			m_loc = other.m_loc;
			return *this;
		}

		//QCToken &operator=(QCToken&&) noexcept = default;

		Kind kind() const noexcept{ return m_kind; }
		QStringView str() const noexcept{ return m_str; }
		Location location() const noexcept{ return m_loc; }

	private:
		Kind m_kind;
		QStringView m_str;
		Location m_loc;
};

#endif // !QIDE_QCTOKEN_HPP
