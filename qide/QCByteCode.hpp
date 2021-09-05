#ifndef QIDE_QCBYTECODE_HPP
#define QIDE_QCBYTECODE_HPP 1

#include <QObject>
#include <QByteArray>
#include <QVector>

class QCByteCode: public QObject{
	Q_OBJECT

	Q_PROPERTY(QByteArray byteCode READ byteCode WRITE setByteCode NOTIFY byteCodeChanged)
	Q_PROPERTY(QVector<Instr> instructions READ instructions)
	Q_PROPERTY(QVector<Def> definitions READ definitions)
	Q_PROPERTY(QVector<Field> fields READ fields)
	Q_PROPERTY(QVector<Function> functions READ functions)
	Q_PROPERTY(QByteArray strings READ strings)
	Q_PROPERTY(QVector<quint32> globals READ globals)

	public:
		enum Section{
			STATEMENTS = 0,
			DEFS = 1,
			FIELDS = 2,
			FUNCTIONS = 3,
			STRINGS = 4,
			GLOBALS = 5,

			SECTION_COUNT
		};

		Q_ENUM(Section)

		struct Header{
			quint32 ver; // 6
			quint16 crc;
			quint16 skip; // 0
			quint32 sectionData[SECTION_COUNT * 2];
		};

		enum Type: quint16{
			VOID = 0,
			STRING = 1,
			FLOAT = 2,
			VECTOR = 3,
			ENTITY = 4,
			FIELD = 5,
			FUNCTION = 6
		};

		Q_ENUM(Type)

		struct Def{
			Type type;
			quint16 globalIdx;
			quint32 nameIdx;
		};

		struct Field{
			Type type;
			quint16 offset;
			quint32 nameIdx;
		};

		struct Function{
			qint32 entryPoint;
			quint32 localIdx;
			quint32 numLocals;
			quint32 profile; // 0
			quint32 nameIdx;
			quint32 fileIdx;
			qint32 numArgs;
			quint8 argSizes[8];
		};

		enum Op: quint32{
			// Misc
			DONE = 0x00,
			STATE = 0x3C,
			GOTO = 0x3D,
			ADDRESS = 0X1E,
			RETURN = 0X2B,

			// Arithmetic
			MUL_F = 0X01,
			MUL_V = 0X02,
			MUL_FV = 0X03,
			MUL_VF = 0X04,
			DIV_F = 0X05,
			ADD_F = 0X06,
			ADD_V = 0X07,
			SUB_F = 0X08,
			SUB_V = 0X09,

			// Comparison
			EQ_F = 0X0A,
			EQ_V = 0X0B,
			EQ_S = 0X0C,
			EQ_E = 0X0D,
			EQ_FNC = 0X0E,
			NE_F = 0X0F,
			NE_V = 0X10,
			NE_S = 0X11,
			NE_E = 0X12,
			NE_FNC = 0X13,
			LE = 0X14,
			GE = 0X15,
			LT = 0X16,
			GT = 0X17,

			// Loading/Storing
			LOAD_F = 0x18,
			LOAD_V = 0x19,
			LOAD_S = 0x1A,
			LOAD_ENT = 0x1B,
			LOAD_FLD = 0x1C,
			LOAD_FNC = 0x1D,
			STORE_F = 0x1F,
			STORE_V = 0x20,
			STORE_S = 0x21,
			STORE_ENT = 0x22,
			STORE_FLD = 0x23,
			STORE_FNC = 0x24,
			STOREP_F = 0x25,
			STOREP_V = 0x26,
			STOREP_S = 0x27,
			STOREP_ENT = 0x28,
			STOREP_FLD = 0x29,
			STOREP_FNC = 0x2A,

			// If, Not
			NOT_F = 0X2C,
			NOT_V = 0X2D,
			NOT_S = 0X2E,
			NOT_ENT = 0X2F,
			NOT_FNC = 0X30,
			IF = 0X31,
			IFNOT = 0X32,

			// Function Calls
			CALL0 = 0X33,
			CALL1 = 0X34,
			CALL2 = 0X35,
			CALL3 = 0X36,
			CALL4 = 0X37,
			CALL5 = 0X38,
			CALL6 = 0X39,
			CALL7 = 0X3A,
			CALL8 = 0X3B,

			// Boolean Operations
			AND = 0X3E,
			OR = 0X3F,
			BITAND = 0X40,
			BITOR = 0X41
		};

		Q_ENUM(Op)

		struct alignas(16) Instr{
			Op op;
			quint32 args[3];
		};

		explicit QCByteCode(QObject *parent = nullptr);
		explicit QCByteCode(const QByteArray &bc, QObject *parent = nullptr);

		const QByteArray &byteCode() const noexcept{ return m_bc; }
		const QVector<Instr> &instructions() const noexcept{ return m_instrs; }
		const QVector<Def> &definitions() const noexcept{ return m_defs; }
		const QVector<Field> &fields() const noexcept{ return m_fields; }
		const QVector<Function> &functions() const noexcept{ return m_fns; }
		const QByteArray &strings() const noexcept{ return m_strs; }
		const QVector<quint32> &globals() const noexcept{ return m_globals; }

		void setByteCode(const QByteArray &bc);

	signals:
		void byteCodeChanged();

	private:
		QByteArray m_bc;
		QVector<Instr> m_instrs;
		QVector<Def> m_defs;
		QVector<Field> m_fields;
		QVector<Function> m_fns;
		QByteArray m_strs;
		QVector<quint32> m_globals;
};

Q_DECLARE_METATYPE(QCByteCode::Header)
Q_DECLARE_METATYPE(QCByteCode::Def)
Q_DECLARE_METATYPE(QCByteCode::Field)
Q_DECLARE_METATYPE(QCByteCode::Function)
Q_DECLARE_METATYPE(QCByteCode::Instr)

#endif // !QIDE_QCBYTECODE_HPP
