#include <cstring>
#include <span>

#include <QtEndian>
#include <QDebug>
#include <QMetaEnum>
#include <QDataStream>

#include "QCByteCode.hpp"

QCByteCode::QCByteCode(QObject *parent)
	: QObject(parent)
{}

QCByteCode::QCByteCode(const QByteArray &bc, QObject *parent)
	: QCByteCode(parent)
{
	setByteCode(bc);
}

void QCByteCode::setByteCode(const QByteArray &bc){
	Header header;

	QDataStream in(bc);
	in.readRawData((char*)&header.ver, sizeof(header.ver));
	in.readRawData((char*)&header.crc, sizeof(header.crc));
	in.readRawData((char*)&header.skip, sizeof(header.skip));
	in.readRawData((char*)&header.sectionData, SECTION_COUNT * sizeof(quint32) * 2);

	auto sectionOff = [&](Section section){
		return header.sectionData[(section * 2)];
	};

	auto sectionLen = [&](Section section){
		return header.sectionData[(section * 2) + 1];
	};

	if(header.ver != 0x06){
		qDebug() << "Wrong header version" << header.ver;
		return;
	}

	if(header.skip != 0){
		qDebug() << "Wrong header skip value" << header.skip;
		return;
	}

	constexpr auto sectionDataSize = [](Section section){
		switch(section){
			case STATEMENTS: return 16;
			case DEFS: return 8;
			case FIELDS: return 8;
			case FUNCTIONS: return 36;
			case GLOBALS: return 4;
			default: return 1;
		}
	};

	auto sectionData = [&](Section section){
		auto off = sectionOff(section);
		auto len = sectionLen(section) * sectionDataSize(section);
		return QByteArray::fromRawData(bc.data() + off, len);
	};

	const auto stmtsData = sectionData(STATEMENTS);
	const auto defsData = sectionData(DEFS);
	const auto fldsData = sectionData(FIELDS);
	const auto fncsData = sectionData(FUNCTIONS);
	const auto strsData = sectionData(STRINGS);
	const auto glbsData = sectionData(GLOBALS);

	QVector<Instr> newInstrs;
	QVector<Def> newDefs;
	QVector<Field> newFields;
	QVector<Function> newFns;

	const auto numInstrs = sectionLen(STATEMENTS);
	const auto numDefs = sectionLen(DEFS);
	const auto numFields = sectionLen(FIELDS);
	const auto numFns = sectionLen(FUNCTIONS);

	newInstrs.reserve(numInstrs);
	newDefs.reserve(numDefs);
	newFields.reserve(numFields);
	newFns.reserve(numFns);

	auto opMeta = QMetaEnum::fromType<Op>();

	QDataStream stmtIn(stmtsData);

	for(std::size_t i = 0; i < numInstrs; i++){
		Instr instr;
		stmtIn.readRawData((char*)&instr.op, sizeof(instr.op));

		if(!opMeta.valueToKey(instr.op)){
			qDebug() << "Invalid statement opcode" << instr.op;
			return;
		}

		stmtIn.readRawData((char*)instr.args, std::size(instr.args) * sizeof(*instr.args));

		newInstrs.push_back(instr);
	}

	QDataStream defIn(defsData);

	for(std::size_t i = 0; i < numDefs; i++){
		Def def;
		defIn.readRawData((char*)&def.type, sizeof(def.type));
		defIn.readRawData((char*)&def.globalIdx, sizeof(def.globalIdx));
		defIn.readRawData((char*)&def.nameIdx, sizeof(def.nameIdx));

		if(qint32(def.nameIdx) >= strsData.size()){
			qDebug() << "Invalid name index" << def.nameIdx << "in def";
			return;
		}

		newDefs.push_back(def);
	}

	auto typeMeta = QMetaEnum::fromType<Type>();

	QDataStream fldIn(fldsData);

	for(std::size_t i = 0; i < numFields; i++){
		Field field;
		fldIn.readRawData((char*)&field.type, sizeof(field.type));
		fldIn.readRawData((char*)&field.offset, sizeof(field.offset));
		fldIn.readRawData((char*)&field.nameIdx, sizeof(field.nameIdx));

		if(!typeMeta.valueToKey(field.type)){
			qDebug() << "Invalid type" << field.type << "in field";
			return;
		}
		else if(qint32(field.nameIdx) >= strsData.size()){
			qDebug() << "Invalid name index" << field.nameIdx << "in field";
			return;
		}

		newFields.push_back(field);
	}

	QDataStream fncIn(fncsData);

	for(std::size_t i = 0; i < numFns; i++){
		Function fn;
		fncIn.readRawData((char*)&fn.entryPoint, sizeof(fn.entryPoint));
		fncIn.readRawData((char*)&fn.localIdx, sizeof(fn.localIdx));
		fncIn.readRawData((char*)&fn.numLocals, sizeof(fn.numLocals));
		fncIn.readRawData((char*)&fn.profile, sizeof(fn.profile));
		fncIn.readRawData((char*)&fn.nameIdx, sizeof(fn.nameIdx));
		fncIn.readRawData((char*)&fn.fileIdx, sizeof(fn.fileIdx));
		fncIn.readRawData((char*)&fn.numArgs, sizeof(fn.numArgs));
		fncIn.readRawData((char*)fn.argSizes, std::size(fn.argSizes));

		if(fn.entryPoint > 0 && fn.entryPoint >= stmtsData.size()){
			qDebug() << "Invalid function entry point" << fn.entryPoint;
			return;
		}
		else if(qint32(fn.nameIdx) >= strsData.size()){
			qDebug() << "Invalid function name index" << fn.nameIdx;
			return;
		}
		else if(qint32(fn.fileIdx) >= strsData.size()){
			qDebug() << "Invalid function file index" << fn.fileIdx;
			return;
		}

		newFns.push_back(fn);
	}

	m_instrs = newInstrs;
	m_defs = newDefs;
	m_fields = newFields;
	m_fns = newFns;
	m_strs = strsData;

	std::memcpy(m_globals.data(), glbsData.data(), glbsData.size());

	m_bc = bc;
	emit byteCodeChanged();
}
