#ifndef QIDE_QCTYPE_HPP
#define QIDE_QCTYPE_HPP 1

#include <stdexcept>
#include <memory>
#include <optional>

#include <QMetaEnum>
#include <QStringList>

inline const QStringList qcBasicTypes = {
	"void", "int", "float", "vector", "string", "entity"
};

class QCType: public QObject{
	Q_OBJECT

	Q_PROPERTY(Value value READ value NOTIFY valueChanged)
	Q_PROPERTY(QVariant inner READ inner NOTIFY innerChanged)
	Q_PROPERTY(QString str READ str NOTIFY strChanged)

	public:
		enum Value{
			Void, Int, Float, Vector, String, Entity,
			Field, Array,
			count
		};

		Q_ENUM(Value);

		QCType(): QObject(), m_val(count), m_inner(), m_str(){}

		QCType(Value val, std::optional<QCType> inner = std::nullopt)
			: QObject()
			, m_val(val)
			, m_inner(inner ? QVariant::fromValue(*inner) : QVariant())
			, m_str(genStr(val, std::move(inner)))
		{}

		QCType(const QCType &other)
			: QObject()
			, m_val(other.m_val)
			, m_inner(other.m_inner)
			, m_str(other.m_str)
		{}

		QCType(QCType &&other) noexcept
			: QObject()
			, m_val(other.m_val)
			, m_inner(std::move(other.m_inner))
			, m_str(std::move(other.m_str))
		{}

		QCType &operator=(const QCType &other){
			m_val = other.m_val;
			m_inner = other.m_inner;
			m_str = other.m_str;
			return *this;
		}

		QCType &operator=(QCType &&other) noexcept{
			m_val = other.m_val;
			m_inner = std::move(other.m_inner);
			m_str = std::move(other.m_str);
			return *this;
		}

		static std::optional<QCType> fromStr(QStringView str){
			if(str.isEmpty() || str == QStringLiteral(".")) return std::nullopt;

			else if(str.startsWith(QStringLiteral("."))) return QCType(Field, fromStr(str.right(str.size() - 1)));

			else if(str == QStringLiteral("void")) return Void;
			else if(str == QStringLiteral("int")) return Int;
			else if(str == QStringLiteral("float")) return Float;
			else if(str == QStringLiteral("vector")) return Vector;
			else if(str == QStringLiteral("string")) return String;
			else if(str == QStringLiteral("entity")) return Entity;

			else return std::nullopt;
		}

		Value value() const noexcept{ return m_val; }
		const QVariant &inner() const noexcept{ return m_inner; }
		const QString &str() const noexcept{ return m_str; }

	signals:
		void valueChanged();
		void innerChanged();
		void strChanged();

	private:
		QString genStr(Value val, std::optional<QCType> inner){
			switch(val){
				case Field:{
					if(!inner) throw std::runtime_error("field must have field type specified");
					return QString(".") + inner->m_str;
				}

				case Array:{
					if(!inner) throw std::runtime_error("array must have array type specified");
					return inner->m_str + QString("[]");
				}

				default:{
					static auto meta = QMetaEnum::fromType<Value>();
					return QString(meta.valueToKey(val)).toLower();
				}
			}
		}

		Value m_val;
		QVariant m_inner;
		QString m_str;
};

Q_DECLARE_METATYPE(QCType)

#endif // QIDE_QCTYPE_HPP
