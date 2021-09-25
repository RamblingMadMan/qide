#include <QDebug>
#include <QVector>

#include "fmt/format.h"

#include "QCParser.hpp"

template<typename ... Fns>
struct Overload: Fns...{
	public:
		template<typename ... UFns>
		Overload(UFns &&... fns): Fns(std::forward<UFns>(fns))...{}

		using Fns::operator()...;
};

template<typename ... Fns>
Overload(Fns&&...) -> Overload<Fns...>;

template<typename ... Fns>
auto overload(Fns &&... fns){ return Overload{ std::forward<Fns>(fns)... }; }

QCParser::QCParser(QObject *parent): QObject(parent){}

void QCParser::reset(){
	m_results.clear();
	m_locMap.clear();
	m_parseFn = [this](auto&&...args){ return parseToplevel(std::forward<decltype(args)>(args)...); };
}

void QCParser::setTitle(QString str){
	m_title = std::move(str);
	emit titleChanged();
}

static inline const QCToken *skipKinds(const QCToken *it, const QCToken *end, const QVector<QCToken::Kind> &kinds){
	while(it != end && kinds.contains(it->kind())){
		++it;
	}

	return it;
}

static inline const QCToken *skipCommentsAndSpaces(const QCToken *it, const QCToken *end){ // ... AndNewLines
	return skipKinds(it, end, { QCToken::Comment, QCToken::NewLine, QCToken::Space });
}

int QCParser::parse(const QCToken *beg, const QCToken *const end){
	if(!beg || !end) return true;

	beg = skipCommentsAndSpaces(beg, end);

	int origLen = m_results.size();

	while(beg != end){
		auto res = m_parseFn(beg, end);

		if(auto expr = std::get_if<QCExpr>(&res)){
			m_locMap[expr->begin()->location()] = m_results.size();
			beg = expr->end();
			m_results.push_back(*expr);
		}
		else{
			break;
		}
	}

	auto res = m_results.size() - origLen;

	if(res > 0) emit resultsChanged();

	return res;
}

std::variant<bool, QCExpr> QCParser::parseDecl(QCType ty, const QCToken *exprStart, const QCToken *beg, const QCToken *end){
	beg = skipCommentsAndSpaces(beg, end);

	if(beg == end){
		m_parseFn = [=](auto &&... args){ return parseDecl(ty, exprStart, std::forward<decltype(args)>(args)...); };
		return true;
	}

	auto it = beg;

	if(it->kind() != QCToken::Id){
		qDebug() << QString("[ERROR] Expected Id but got %1 '%2'").arg(it->kind()).arg(it->str().toString());
		return false;
	}

	const auto name = it->str();

	++it;

	auto parseNamedDef = [ty, exprStart, name](const QCToken *beg_, const QCToken *end_) -> ParseResult{
		beg_ = skipKinds(beg_, end_, { QCToken::Comment });

		if(beg_ == end_){
			return true;
		}

		auto it = beg_;

		if(it->kind() == QCToken::Term){
			return QCExpr::makeVarDef(ty, name.toString(), exprStart, ++it);
		}

		return false;
	};

	if(it == end){
		m_parseFn = std::move(parseNamedDef);
		return true;
	}
	else{
		return parseNamedDef(it, end);
	}
}

std::variant<bool, QCExpr> QCParser::parseToplevel(const QCToken *beg, const QCToken *end){
	beg = skipKinds(beg, end, { QCToken::Term, QCToken::Comment, QCToken::Space, QCToken::NewLine }); // find first non-empty statement

	auto it = beg;

	if(it == end){
		return true;
	}
	else if(it->kind() == QCToken::EndOfFile){
		return QCExpr::makeEOF(beg);
	}
	else if(it->str() == QString(".")){
		auto continueFn = [=](const QCToken *beg_, const QCToken *end_) -> ParseResult{
			if(beg_->kind() != QCToken::Type){
				fmt::print(stderr, "[ERROR] Invalid type '{}'\n", beg_->str().toString().toStdString());
				return false;
			}

			auto start = beg_;

			return parseDecl(
				QCType(
					QCType::Field,
					*QCType::fromStr(start->str())
				),
				beg,
				++beg_,
				end_
			);
		};

		++it;
		if(it == end){
			m_parseFn = continueFn;
		}
		else{
			return continueFn(it, end);
		}
	}
	else if(it->kind() == QCToken::Type){
		auto ty = *QCType::fromStr(it->str());
		return parseDecl(ty, beg, ++it, end);
	}

	return false;
}
