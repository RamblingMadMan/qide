#include <cstring>
#include <span>

#include <QDataStream>

#include "QCByteCode.hpp"
#include "QCVM.hpp"

QCVM::QCVM(QCByteCode *bc, QObject *parent)
	: QObject(parent)
{
	setByteCode(bc);
}

QCVM::QCVM(QObject *parent)
	: QObject(parent)
	, m_bc(new QCByteCode(this))
{}

void QCVM::setByteCode(QCByteCode *bc){
	if(bc == m_bc) return;

	bc->setParent(this);
	m_bc = bc;
	m_instrIdx = 0;
	emit byteCodeChanged();
	emit instructionIndexChanged();
}

void QCVM::setInstructionIndex(quint32 idx){
	if(idx == m_instrIdx || qint32(idx) >= m_bc->instructions().size()){
		return;
	}

	m_instrIdx = idx;
	emit instructionIndexChanged();
}
