#ifndef QIDE_QCVM_HPP
#define QIDE_QCVM_HPP 1

#include <QObject>

class QCByteCode;

class QCVM: public QObject{
	Q_OBJECT

	Q_PROPERTY(QCByteCode* byteCode READ byteCode WRITE setByteCode NOTIFY byteCodeChanged)
	Q_PROPERTY(quint32 instructionIndex READ instructionIndex WRITE setInstructionIndex NOTIFY instructionIndexChanged)

	public:
		explicit QCVM(QObject *parent = nullptr);
		explicit QCVM(QCByteCode *bc, QObject *parent = nullptr);

		QCByteCode *byteCode() noexcept{ return m_bc; }
		quint32 instructionIndex() const noexcept{ return m_instrIdx; }

		void setByteCode(QCByteCode *bc);
		void setInstructionIndex(quint32 idx);

	signals:
		void byteCodeChanged();
		void instructionIndexChanged();

	private:
		QCByteCode *m_bc;
		quint32 m_instrIdx = 0;

		//static float anglemod(float angle) noexcept{ return  }
		//static float rint(float val){ return qRound(val); }
		//static float floor(float val){ return qFloor(val); }
		//static float ceil(float val){ return std::ceil(val); }
		//static float fabs(float val){ return qAbs(val); }
		//static char *ftos(float val){  }
};

#endif // !QIDE_QCVM_HPP
