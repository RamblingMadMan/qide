#include "QCLexer.hpp"
#include "QCType.hpp"

#include "fmt/format.h"

void QCLexer::reset(){
	m_curLoc = { 0, 0 };
	m_mode = Normal;

	if(!m_tokens.empty()){
		m_tokens.clear();
		emit tokensChanged();
	}
}

template<typename It>
inline It skipSpaces(It it, It end, bool skipNewlines = false){
	while(it != end){
		if(!it->isSpace()) break;

		if(!skipNewlines && *it == u'\n'){
			return it;
		}

		++it;
	}

	return it;
}

QCToken QCLexer::lexNormal(StrIter it, StrIter end){
	while(it != end && it->isSpace()){
		if(*it == u'\n'){
			auto nlLoc = m_curLoc;

			++m_curLoc.line;
			m_curLoc.col = 0;

			if(!m_skipNl){
				return QCToken(QCToken::NewLine, QStringView(it++, 1), nlLoc);
			}

			++it;
		}
		else if(!m_skipWs){
			auto spaceStart = it;
			auto spaceLoc = m_curLoc;

			do{
				++m_curLoc.col;
				++it;
			} while(it != end && it->isSpace() && (*it != u'\n'));

			auto spaceEnd = it;

			return QCToken(QCToken::Space, QStringView(spaceStart, spaceEnd), spaceLoc);
		}
		else{
			++it;
		}
	}

	const auto tokLoc = m_curLoc;
	const auto tokStart = it;

	QCToken::Kind tokKind = QCToken::EndOfFile;

	if(*it == u';'){
		tokKind = QCToken::Term;
		++it;
		++m_curLoc.col;
	}
	else if(*it == u'"'){
		tokKind = QCToken::String;

		do {
			++it;
			++m_curLoc.col;

			if(it == end){
				break;
			}
			else if(*it == u'"'){
				++it;
				break;
			}
			else if(*it == u'\\'){
				++it;
				if(it == end){
					break;
				}
				else if(*it == u'\n'){
					++m_curLoc.line;
					m_curLoc.col = 0;
				}
				else{
					++m_curLoc.col;
				}
			}
		} while(it != end);
	}
	else if(*it == u'#'){
		tokKind = QCToken::GlobalId;

		do {
			++it;
			++m_curLoc.col;
		} while(it != end && ((*it == u'_') || it->isLetterOrNumber()));
	}
	else if(*it == u'_' || it->isLetter()){
		do {
			++it;
			++m_curLoc.col;
		} while(it != end && ((*it == u'_') || it->isLetterOrNumber()));

		auto view = QStringView(tokStart, it);
		auto ty = QCType::fromStr(view);

		tokKind = ty ? QCToken::Type : QCToken::Id;
	}
	else if(it->isPunct() || it->isSymbol()){
		if(*it == u'/'){
			++it;
			++m_curLoc.col;

			if(*it == u'*'){
				m_mode = Mode::MultilineComment;

				++it;
				++m_curLoc.col;

				auto comment = lexMultilineComment(it, end);
				it += comment.str().length();

				return QCToken(QCToken::Comment, QStringView(tokStart, it), tokLoc);
			}
			else if(*it == u'/'){
				do{
					++it;
					++m_curLoc.col;
				} while(it != end && *it != u'\n');

				return QCToken(QCToken::Comment, QStringView(tokStart, it), tokLoc);
			}
			else{
				--it;
				--m_curLoc.col;
			}
		}

		tokKind = QCToken::Op;

		do {
			++it;
			++m_curLoc.col;
		} while(it != end && (it->isPunct() || it->isSymbol()));
	}
	else if(it->isDigit()){
		tokKind = QCToken::Number;

		do {
			++it;
			++m_curLoc.col;
		} while(it != end && it->isLetterOrNumber());
	}
	else if(it != end){
		tokKind = QCToken::Unknown;

		do {
			++it;
			++m_curLoc.col;
		} while(it != end && !it->isSpace());
	}

	return QCToken(tokKind, QStringView(tokStart, it), tokLoc);
}

QCToken QCLexer::lexMultilineComment(StrIter it, StrIter end){
	const auto tokLoc = m_curLoc;
	const auto tokStart = it;

	while(it != end){
		if(*it == u'\n'){
			++it;
			++m_curLoc.line;
			m_curLoc.col = 0;
		}
		else if(*it == u'*'){
			++it;
			++m_curLoc.col;
			if(it != end && *it == u'/'){
				m_mode = Mode::Normal;
				++it;
				++m_curLoc.col;
				break;
			}
		}
		else{
			++it;
			++m_curLoc.col;
		}
	}

	return QCToken(QCToken::Comment, QStringView(tokStart, it), tokLoc);
}

int QCLexer::lex(StrIter beg, StrIter end){
	auto it = beg;

	if(it == end) return 0;

	const int origLen = m_tokens.size();

	do {
		m_tokens.push_back(m_mode == Mode::MultilineComment ? lexMultilineComment(it, end) : lexNormal(it, end));
		it += m_tokens.back().str().length();
	} while(it != end);

	int numTokens = m_tokens.size() - origLen;

	if(numTokens > 0){
		emit tokensChanged();
	}

	if(it == end){
		return numTokens;
	}
	else{
		return -numTokens;
	}
}
