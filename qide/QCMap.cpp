#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QHash>
#include <QException>

#include "QCMap.hpp"
#include "QCLexer.hpp"

class QCMapException: public QException{
	public:
		QCMapException(const QString &msg)
			: QException()
			, m_msg(msg.toStdString()){}

		void raise() const override { throw *this; }
		QCMapException *clone() const override { return new QCMapException(*this); }

		const char *what() const noexcept override{ return m_msg.c_str(); }

	private:
		std::string m_msg;
};

QCMap::QCMap(QObject *parent)
	: QObject(parent){}

QCMap::QCMap(const QString &filePath, QObject *parent)
	: QCMap(parent)
{
	loadFile(filePath);
}

using TokIter = QVector<QCToken>::ConstIterator;

TokIter skipSpaces(TokIter it, TokIter end, bool skipNewline = false){
	while(it != end){
		if(it->kind() == QCToken::NewLine){
			if(!skipNewline) break;
		}
		else if(it->kind() != QCToken::Space){
			break;
		}

		++it;
	}

	return it;
}

QCMap::TokIt QCMap::parseFace(TokIt beg, TokIt end, Face *ret){
	QVector3D point;

	auto it = beg;

	// Parse points

	while(true){
		for(int i = 0; i < 3; i++){
			it = skipSpaces(it, end);

			if(it->kind() != QCToken::Number){
				throw QCMapException("invalid face point value");
			}

			point[i] = it->str().toInt();

			++it;
		}

		it = skipSpaces(it, end);

		if(it->str() != u")"){
			throw QCMapException(QString("invalid token '%1' in point value; expected ')'").arg(it->str()));
		}

		ret->points.push_back(point);

		it = skipSpaces(++it, end);

		if(it->str() != u"(") break;
	}

	// Parse material name

	if(it->kind() != QCToken::Id){
		throw QCMapException(QString("invalid material name '%1'").arg(it->str()));
	}

	ret->materialName = it->str().toString();

	++it;

	// Parse other fields

	qreal *const valPtr[5] = {
		&ret->xOff, &ret->yOff,
		&ret->rot,
		&ret->xScale, &ret->yScale
	};

	for(int i = 0; i < 5; i++){
		it = skipSpaces(it, end);

		if(it->kind() != QCToken::Number){
			throw QCMapException(QString("invalid token '%1' in texture values").arg(it->str()));
		}

		*valPtr[i] = it->str().toInt();

		++it;
	}

	return it;
}

QCMap::TokIt QCMap::parseBrush(TokIt beg, TokIt end, Entity *ent){
	auto it = skipSpaces(beg, end, true);

	if(it == end){
		throw QCMapException("invalid brush definition");
	}

	auto brush = &ent->brush.value();

	while(it != end){
		if(it->str() == u"("){ // face def
			Face face;
			it = parseFace(++it, end, &face);
			brush->faces.push_back(face);
		}
		else if(it->str() == u"}"){ // end of brush definition
			++it;
			break;
		}
		else{
			throw QCMapException(QString("invalid token '%1' in brush definition").arg(it->str()));
		}
	}

	return it;
}

QCMap::TokIt QCMap::parseEntity(TokIt beg, TokIt end){
	auto it = beg;

	Entity ent;

	while(true){
		it = skipSpaces(it, end, true);

		if(it == end){
			throw QCMapException("invalid entity definition");
		}

		if(it->str() == u"{"){ // brush def
			if(ent.brush){
				throw QCMapException("multiple brushed defined for entity");
			}

			it = parseBrush(++it, end, &ent);
		}
		else if(it->str() == u"}"){
			++it;
			break;
		}
		else if(it->kind() == QCToken::String){
			auto key = it->str();

			it = skipSpaces(++it, end);

			if(it->kind() != QCToken::String){
				throw QCMapException(QString("invalid token '%1' in key value pair").arg(it->str()));
			}

			auto val = it->str();

			ent.keys[key.toString()] = val.toString();

			it = skipSpaces(++it, end);
		}
		else{
			throw QCMapException(QString("invalid token '%1' in entity definition").arg(it->str()));
		}
	}

	if(!ent.keys.contains("classname")){
		throw QCMapException("no classname given in entity definition");
	}

	if(!ent.keys.contains("origin")){
		throw QCMapException("no origin given in entity definition");
	}

	m_ents.push_back(ent);

	return it;
}

QCMap::TokIt QCMap::parseMap(TokIt beg, TokIt end){
	auto it = skipSpaces(beg, end, true);

	if(it == end){
		qDebug() << "empty map data";
		return it;
	}

	while(it != end){
		if(it->str() == u'{'){ // entity definition
			it = skipSpaces(parseEntity(++it, end), end);
		}
		else{
			throw QCMapException(QString("invalid token '%1' in map file").arg(it->str()));
		}
	}

	return it;
}

void QCMap::loadStr(const QString &str){
	QCLexer lexer;
	lexer.setSkipNewLines(false);
	lexer.setSkipSpaces(false);
	lexer.lex(str.begin(), str.end());

	auto &&toks = lexer.tokens();

	parseMap(toks.begin(), toks.end());
}

void QCMap::loadFile(const QString &filePath){
	auto fileInfo = QFileInfo(filePath);

	if(!fileInfo.exists() || !fileInfo.isFile()){
		qDebug() << "Map file path" << filePath << "is invalid";
		return;
	}

	QFile file(filePath);
	if(!file.open(QFile::ReadOnly)){
		qDebug() << "Could not open map file" << filePath;
		return;
	}

	auto mapStr = QString::fromUtf8(file.readAll());

	return loadStr(mapStr);
}
