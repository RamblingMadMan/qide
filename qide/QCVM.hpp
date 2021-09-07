#ifndef QIDE_QCVM_HPP
#define QIDE_QCVM_HPP 1

#include <variant>

#include <QObject>
#include <QVector>
#include <QHash>

#include "QCByteCode.hpp"

class QCVMFunction: public QObject{
	Q_OBJECT

	public:
		explicit QCVMFunction(QObject *parent = nullptr);

		QCVMFunction(QCByteCode *bc, qint32 fnIdx, QObject *parent = nullptr)
			: QCVMFunction(parent)
		{
			loadByteCode(bc, fnIdx);
		}

		void loadByteCode(QCByteCode *bc, qint32 fnIdx);

		const QString &name() const noexcept{ return m_name; }
		const QString &fileName() const noexcept{ return m_fileName; }

		QVariant call(QVector<QVariant> args = {});

	private:
		QCByteCode *m_bc;
		const QCByteCode::Function *m_fn;
		QString m_name, m_fileName;
		const QCByteCode::Instr *m_first;
};

class QCVM: public QObject{
	Q_OBJECT

	Q_PROPERTY(QCByteCode* byteCode READ byteCode WRITE setByteCode NOTIFY byteCodeChanged)
	Q_PROPERTY(QList<QCVMFunction*> fns READ fns NOTIFY fnsChanged)

	public:
		explicit QCVM(QObject *parent = nullptr);
		explicit QCVM(QCByteCode *bc, QObject *parent = nullptr);

		QCByteCode *byteCode() noexcept{ return m_bc; }
		QList<QCVMFunction*> fns() noexcept{ return m_fns.values(); }

		void setByteCode(QCByteCode *bc);

		QCVMFunction *getFn(const QString &name){
			auto it = m_fns.find(name);
			if(it != m_fns.end()) return *it;
			return nullptr;
		}

	public slots:
		void updateBytecode();

	signals:
		void byteCodeChanged();
		void fnsChanged();

	private:
		QCByteCode *m_bc;

		QHash<QString, QCVMFunction*> m_fns;

		//static float anglemod(float angle) noexcept{ return  }
		//static float rint(float val){ return qRound(val); }
		//static float floor(float val){ return qFloor(val); }
		//static float ceil(float val){ return std::ceil(val); }
		//static float fabs(float val){ return qAbs(val); }
		//static char *ftos(float val){  }
};

#endif // !QIDE_QCVM_HPP
