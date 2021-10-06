#include "QCParser.hpp"
#include "QCProgs.hpp"

QCProgs::QCProgs(QObject *parent) : QObject(parent){}

QCParser *QCProgs::insert(const QString &name){
	auto res = m_parsers.find(name);
	if(res != m_parsers.end()){
		return nullptr;
	}

	auto ptr = new QCParser(this);
	ptr->setTitle(name);
	m_parsers.insert(name, ptr);
	return ptr;
}

QCParser *QCProgs::get(const QString &name){
	auto res = m_parsers.find(name);
	if(res != m_parsers.end()){
		return *res;
	}

	return nullptr;
}
