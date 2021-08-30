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

QCToken QCLexer::lexNormal(StrIter it, StrIter end){
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
		} while(it != end && *it != u'"');
	}
	else if(*it == u'#'){
		tokKind = QCToken::GlobalId;

		do {
			++it;
			++m_curLoc.col;
		} while(it != end && ((*it == u'_') || it->isLetterOrNumber()));
	}
	else if(*it == u'_' || it->isLetter()){
		tokKind = QCToken::Id;

		do {
			++it;
			++m_curLoc.col;
		} while(it != end && ((*it == u'_') || it->isLetterOrNumber()));

		auto view = QStringView(tokStart, it);
		auto ty = QCType::fromStr(view);

		tokKind = ty ? QCToken::Type : QCToken::Id;
	}
	else if(it->isPunct() || it->isSymbol()){
		tokKind = QCToken::Op;

		do {
			++it;
			++m_curLoc.col;
		} while(it != end && it->isPunct());

		auto view = QStringView(tokStart, it);

		bool isMultiline = view.startsWith(u"/*");
		bool isComment = isMultiline || view.startsWith(u"//");

		if(isComment){
			if(isMultiline){
				while(it != end && !view.endsWith(u"*/")){
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

					view = QStringView(tokStart, it);
				}

				if(!view.endsWith(u"*/")){
					m_mode = Mode::MultilineComment;
				}
			}
			else{
				while(it != end && *it != u'\n'){
					++it;
				}

				++m_curLoc.line;
				m_curLoc.col = 0;
			}

			tokKind = QCToken::Comment;
		}
	}
	else if(it->isDigit()){
		tokKind = QCToken::Number;

		do {
			++it;
			++m_curLoc.col;
		} while(it != end && it->isLetterOrNumber());
	}
	else{
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
	const auto tokStart = it++;

	auto view = QStringView(tokStart, it);

	while(it != end){
		++it;

		view = QStringView(tokStart, it);

		if(view.endsWith(u"*/")){
			m_mode = Mode::Normal;
			break;
		}
		else if(it != end && *it == u'\n'){
			++m_curLoc.line;
			m_curLoc.col = 0;
		}
		else{
			++m_curLoc.col;
		}
	}

	return QCToken(QCToken::Comment, view, tokLoc);
}

int QCLexer::lex(StrIter beg, StrIter end){
	auto it = beg;

	if(it == end) return 0;

	const int origLen = m_tokens.size();

	do {
		while(it != end && it->isSpace()){
			if(*it == u'\n'){
				++m_curLoc.line;
				m_curLoc.col = 0;
			}
			else{
				++m_curLoc.col;
			}

			++it;
		}

		if(it == end) break;

		m_tokens.append(m_mode == Mode::MultilineComment ? lexMultilineComment(it, end) : lexNormal(it, end));
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
