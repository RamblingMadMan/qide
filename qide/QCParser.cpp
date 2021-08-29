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

QCParser::QCParser(): QObject(){}

void QCParser::reset(){
	m_results.clear();
	m_parseFn = std::bind_front(&QCParser::parseToplevel, this);
}

bool QCParser::parse(const QCToken *beg, const QCToken *const end){
	if(!beg || !end) return true;

	auto res = m_parseFn(beg, end);

	return std::visit(
		overload(
			[](bool res){ return res; },
			[this](const QCExpr &expr){
				m_results.push_back(expr);
				return true;
			}
		),
		res
	);
}

std::variant<bool, QCExpr> QCParser::parseDef(QCType ty, const QCToken *exprStart, const QCToken *beg, const QCToken *end){
	while(beg != end && (beg->kind() == QCToken::Comment)){
		++beg;
	}

	if(beg == end){
		m_parseFn = std::bind_front(&QCParser::parseDef, this, ty, exprStart);
		return true;
	}

	auto it = beg;

	if(it->kind() != QCToken::Id){
		auto meta = QMetaEnum::fromType<QCToken::Kind>();
		fmt::print(
			stderr, "[ERROR] expected Id but got {} '{}'\n",
			meta.valueToKey(it->kind()), it->str().toString().toStdString()
		);
		return false;
	}
	else if(it->kind() == QCToken::Term){
		++it;

	}

	return false;
}

std::variant<bool, QCExpr> QCParser::parseToplevel(const QCToken *beg, const QCToken *end){
	while(beg != end && (beg->kind() == QCToken::Term || beg->kind() == QCToken::Comment)){
		++beg;
	}

	auto it = beg;

	if(it == end){
		return true;
	}
	else if(it->kind() == QCToken::EndOfFile){
		return QCExpr::makeEOF(beg);
	}
	else if(it->str() == u"."){
		auto continueFn = [=, this](const QCToken *beg_, const QCToken *end_) -> ParseResult{
			if(beg_->kind() != QCToken::Type){
				fmt::print(stderr, "[ERROR] Invalid type '{}'\n", beg_->str().toString().toStdString());
				return false;
			}

			return parseDef(
				QCType(
					QCType::Field,
					*QCType::fromStr(beg_->str())
				),
				beg,
				beg_ + 1,
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
		return parseDef(*QCType::fromStr(it->str()), it, it + 1, end);
	}

	return false;
}
