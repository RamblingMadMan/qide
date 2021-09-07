#ifndef QIDE_QCTOKEN_HPP
#define QIDE_QCTOKEN_HPP 1

#include <QObject>
#include <QMetaEnum>
#include <QString>

inline const QStringList qcKeywords = {
	"var", "return", "typedef", "for", "if", "else"
};

class QCToken: public QObject{
	Q_OBJECT

	Q_PROPERTY(Kind kind READ kind WRITE setKind NOTIFY kindChanged)
	Q_PROPERTY(QStringView str READ str WRITE setStr NOTIFY strChanged)
	Q_PROPERTY(Location location READ location WRITE setLocation NOTIFY locationChanged)

	public:
		struct Location{
			int line, col;

			inline friend bool operator<(const Location &lhs, const Location &rhs) noexcept{
				if(lhs.line == rhs.line) return lhs.col < rhs.col;
				else return lhs.line < rhs.line;
			}

			inline friend bool operator==(const Location &lhs, const Location &rhs) noexcept{
				return (lhs.line == rhs.line) && (lhs.col == rhs.col);
			}

			inline friend bool operator!=(const Location &lhs, const Location &rhs) noexcept{
				return (lhs.line != rhs.line) || (lhs.col != rhs.col);
			}
		};

		enum Kind{
			Space,
			NewLine,
			Type,
			Keyword,
			Id,
			Number,
			String,
			Term,
			GlobalId,
			Comment,
			Op,
			EndOfFile,
			Unknown,
			count
		};

		Q_ENUM(Kind)

		QCToken(Kind kind_, QStringView str_, Location loc, QObject *parent = nullptr)
			: QObject(parent), m_kind(kind_), m_str(str_), m_loc(loc){}

		explicit QCToken(QObject *parent = nullptr)
			: QCToken(count, QStringLiteral(""), { 0, 0 }, parent){}

		QCToken(const QCToken &other)
			: QObject()
			, m_kind(other.m_kind)
			, m_str(other.m_str)
			, m_loc(other.m_loc)
		{}

		QCToken &operator=(const QCToken &other){
			m_kind = other.m_kind;
			m_str = other.m_str;
			m_loc = other.m_loc;
			return *this;
		}

		Kind kind() const noexcept{ return m_kind; }
		QStringView str() const noexcept{ return m_str; }
		const Location &location() const noexcept{ return m_loc; }

		void setKind(Kind kind_){ m_kind = kind_; emit kindChanged(); }
		void setStr(QStringView str_){ m_str = str_; emit strChanged(); }
		void setLocation(const Location &loc){ m_loc = loc; emit locationChanged(); }

	signals:
		void kindChanged();
		void strChanged();
		void locationChanged();

	private:
		Kind m_kind;
		QStringView m_str;
		Location m_loc;
};

#endif // !QIDE_QCTOKEN_HPP
