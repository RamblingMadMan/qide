#include <cstring>

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
	if(bc.size() < int(sizeof(Header))){
		qDebug() << "invalid bytecode file, size too small for header";
		return;
	}

	Header header;
	std::memcpy(&header, bc.data(), sizeof(Header));

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

	if(header.skip != 0x0){
		qDebug() << "Wrong header skip value" << header.skip;
		return;
	}

	constexpr auto sectionDataSize = [](Section section) -> std::size_t{
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
	const auto numGlbs = sectionLen(GLOBALS);

	newInstrs.reserve(numInstrs);
	newDefs.reserve(numDefs);
	newFields.reserve(numFields);
	newFns.reserve(numFns);

	auto opMeta = QMetaEnum::fromType<Op>();

	for(std::size_t i = 0; i < numInstrs; i++){
		Instr instr;
		std::memcpy(&instr, stmtsData.data() + (i * sizeof(Instr)), sizeof(Instr));

		if(!opMeta.valueToKey(instr.op)){
			qDebug() << "Invalid statement opcode" << instr.op;
			return;
		}

		newInstrs.push_back(instr);
	}

	for(std::size_t i = 0; i < numDefs; i++){
		Def def;
		std::memcpy(&def, defsData.data() + (i * sizeof(Def)), sizeof(Def));

		if(qint32(def.nameIdx) >= strsData.size()){
			qDebug() << "Invalid name index" << def.nameIdx << "in def";
			return;
		}
		else if(def.globalIdx >= glbsData.size()){
			qDebug() << "Invalid global index" << def.globalIdx << "in def";
			return;
		}
		else if(qint32(def.nameIdx) >= strsData.size()){
			qDebug() << "Invalid name index" << def.nameIdx << "in def";
			return;
		}

		newDefs.push_back(def);
	}

	auto typeMeta = QMetaEnum::fromType<Type>();

	for(std::size_t i = 0; i < numFields; i++){
		Field field;
		std::memcpy(&field, fldsData.data() + (i * sizeof(Field)), sizeof(Field));

		if(!typeMeta.valueToKey(field.type)){
			qDebug() << "Invalid type" << field.type << "in field";
			return;
		}
		else if(field.offset >= glbsData.size()){
			qDebug() << "Invalid offset" << field.offset << "in field";
			return;
		}
		else if(qint32(field.nameIdx) >= strsData.size()){
			qDebug() << "Invalid name index" << field.nameIdx << "in field";
			return;
		}

		newFields.push_back(field);
	}

	for(std::size_t i = 0; i < numFns; i++){
		Function fn;
		std::memcpy(&fn, fncsData.data() + (i * sizeof(Function)), sizeof(Function));

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

	m_globals.resize(numGlbs);
	std::memcpy(m_globals.data(), glbsData.data(), glbsData.size());
	m_globals[0] = 0;

	m_bc = bc;
	emit byteCodeChanged();
}
