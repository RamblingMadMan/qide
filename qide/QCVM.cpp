#include <cstring>

#include <QDebug>
#include <QDataStream>

#include "QCByteCode.hpp"
#include "QCVM.hpp"

QCByteCode::Type execByteCode(QCByteCode *bc, const QCByteCode::Instr *instr){
	switch(instr->op){
		// misc
		case QCByteCode::DONE:
		case QCByteCode::STATE:
		case QCByteCode::GOTO:
		case QCByteCode::ADDRESS:
		case QCByteCode::RETURN:

		// Arithmetic
		case QCByteCode::MUL_F:
		case QCByteCode::MUL_V:
		case QCByteCode::MUL_FV:
		case QCByteCode::MUL_VF:
		case QCByteCode::DIV_F:
		case QCByteCode::ADD_F:
		case QCByteCode::ADD_V:
		case QCByteCode::SUB_F:
		case QCByteCode::SUB_V:

		// Comparison
		case QCByteCode::EQ_F:
		case QCByteCode::EQ_V:
		case QCByteCode::EQ_S:
		case QCByteCode::EQ_E:
		case QCByteCode::EQ_FNC:
		case QCByteCode::NE_F:
		case QCByteCode::NE_V:
		case QCByteCode::NE_S:
		case QCByteCode::NE_E:
		case QCByteCode::NE_FNC:
		case QCByteCode::LE:
		case QCByteCode::GE:
		case QCByteCode::LT:
		case QCByteCode::GT:

		// Loading/Storing
		case QCByteCode::LOAD_F:
		case QCByteCode::LOAD_V:
		case QCByteCode::LOAD_S:
		case QCByteCode::LOAD_ENT:
		case QCByteCode::LOAD_FLD:
		case QCByteCode::LOAD_FNC:
		case QCByteCode::STORE_F:
		case QCByteCode::STORE_V:
		case QCByteCode::STORE_S:
		case QCByteCode::STORE_ENT:
		case QCByteCode::STORE_FLD:
		case QCByteCode::STORE_FNC:
		case QCByteCode::STOREP_F:
		case QCByteCode::STOREP_V:
		case QCByteCode::STOREP_S:
		case QCByteCode::STOREP_ENT:
		case QCByteCode::STOREP_FLD:
		case QCByteCode::STOREP_FNC:

		// If, Not
		case QCByteCode::NOT_F:
		case QCByteCode::NOT_V:
		case QCByteCode::NOT_S:
		case QCByteCode::NOT_ENT:
		case QCByteCode::NOT_FNC:
		case QCByteCode::IF:
		case QCByteCode::IFNOT:

		// Function Calls
		case QCByteCode::CALL0:
		case QCByteCode::CALL1:
		case QCByteCode::CALL2:
		case QCByteCode::CALL3:
		case QCByteCode::CALL4:
		case QCByteCode::CALL5:
		case QCByteCode::CALL6:
		case QCByteCode::CALL7:
		case QCByteCode::CALL8:

		// Boolean Operations
		case QCByteCode::AND:
		case QCByteCode::OR:
		case QCByteCode::BITAND:
		case QCByteCode::BITOR:

		default:
			qDebug() << "unimplemented instruction" << instr->op;
			return QCByteCode::Type(-1);
	}
}

QCVMFunction::QCVMFunction(QObject *parent)
	: QObject(parent)
	, m_bc(nullptr)
{}

void QCVMFunction::loadByteCode(QCByteCode *bc, qint32 fnIdx){
	auto fn = &bc->functions().at(fnIdx);

	auto nameCStr = bc->strings().data() + fn->nameIdx;
	auto fileCStr = bc->strings().data() + fn->fileIdx;

	m_name = QString::fromUtf8(nameCStr);
	m_fileName = QString::fromUtf8(fileCStr);

	if(fn->entryPoint < 0){
		m_first = nullptr;
	}
	else{
		m_first = &bc->instructions().at(fn->entryPoint);
	}
}

QVariant QCVMFunction::call(QVector<QVariant> args){
	if(args.size() != m_fn->numArgs){
		qDebug() << "wrong number of arguments to" << m_name;
		return QVariant();
	}

	if(!m_first){
		qDebug() << "Built-in VM functions not implemented";
		return QVariant();
	}

	auto it = m_first;
	auto end = m_bc->instructions().end();

	using BCType = QCByteCode::Type;

	while(it != end){
		auto res = execByteCode(m_bc, it);
		switch(res){
			case BCType::VOID: break;

			case BCType(-1):{
				qDebug() << "error executing function" << m_name;
				return QVariant();
			}

			default:{
				qDebug() << "unimplemented VM result type" << res << "in" << m_name;
				return QVariant();
			}
		}

		++it;
	}

	return QVariant();
}

QCVM::QCVM(QCByteCode *bc, QObject *parent)
	: QObject(parent)
{
	setByteCode(bc);
}

QCVM::QCVM(QObject *parent)
	: QObject(parent)
	, m_bc(new QCByteCode(this))
{
	connect(m_bc, &QCByteCode::byteCodeChanged, this, &QCVM::updateBytecode);
}

void QCVM::setByteCode(QCByteCode *bc){
	if(bc == m_bc) return;

	if(bc){
		connect(bc, &QCByteCode::byteCodeChanged, this, &QCVM::updateBytecode);
		bc->setParent(this);
	}

	if(m_bc){
		disconnect(m_bc, &QCByteCode::byteCodeChanged, this, &QCVM::updateBytecode);
	}

	m_bc = bc;

	emit byteCodeChanged();

	if(m_bc) updateBytecode();
}

void QCVM::updateBytecode(){
	for(int i = 0; i < m_bc->functions().size(); i++){
		auto fn = &m_bc->functions()[i];
		auto name = QString::fromUtf8(m_bc->strings().data() + fn->nameIdx);
		//auto file = QString::fromUtf8(m_bc->strings().data() + fn->fileIdx);

		m_fns.insert(name, new QCVMFunction(m_bc, i, this));
	}

	emit fnsChanged();
}
