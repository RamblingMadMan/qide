#include "QCLexer.hpp"
#include "QCType.hpp"

#include "fmt/format.h"

void QCLexer::reset(){
	m_curLoc = { 0, 0 };
	m_rem = m_src;
	m_mode = Normal;
}

QCToken QCLexer::lexNormal(QStringView::iterator it, QStringView::iterator end){
	const auto tokLoc = m_curLoc;
	const auto tokStart = it;

	QCToken::Kind tokKind = QCToken::EndOfFile;

	if(*it == u';'){
		tokKind = QCToken::Term;
		++it;
	}
	else if(*it == u'"'){
		tokKind = QCToken::String;

		do {
			++it;
			++m_curLoc.col;

			if(*it == u'\\'){
				++it;
				if(*it == u'\n'){
					++m_curLoc.line;
					m_curLoc.col = 0;
				}
			}
		} while(*it != u'"');
	}
	else if(*it == u'#'){
		tokKind = QCToken::GlobalId;

		do {
			++it;
			++m_curLoc.col;
		} while((*it == u'_') || it->isLetterOrNumber());
	}
	else if(*it == u'_' || it->isLetter()){
		tokKind = QCToken::Id;

		do {
			++it;
			++m_curLoc.col;
		} while((*it == u'_') || it->isLetterOrNumber());

		auto view = QStringView(tokStart, it);
		auto ty = QCType::fromStr(view);

		tokKind = ty ? QCToken::Type : QCToken::Id;
	}
	else if(it->isPunct() || it->isSymbol()){
		tokKind = QCToken::Op;

		do {
			++it;
			++m_curLoc.col;
		} while(it->isPunct());

		auto view = QStringView(tokStart, it);

		bool isMultiline = view.startsWith(u"/*");
		bool isComment = isMultiline || view.startsWith(u"//");

		if(isComment){
			if(isMultiline){
				while(it != end && !view.endsWith(u"*/")){
					++it;

					if(*it == u'\n'){
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
		} while(it->isLetterOrNumber());
	}
	else{
		tokKind = QCToken::Unknown;

		do {
			++it;
			++m_curLoc.col;
		} while(!it->isSpace());
	}

	m_rem = QStringView(it, end);

	return QCToken(tokKind, QStringView(tokStart, it), tokLoc);
}

QCToken QCLexer::lexMultilineComment(QStringView::iterator it, QStringView::iterator end){
	const auto tokLoc = m_curLoc;
	const auto tokStart = it++;

	auto view = QStringView(tokStart, it);

	while(it != end){
		++it;

		if(*it == u'\n'){
			++m_curLoc.line;
			m_curLoc.col = 0;
		}
		else{
			++m_curLoc.col;
		}

		view = QStringView(tokStart, it);
		if(view.endsWith(u"*/")){
			m_mode = Mode::Normal;
			break;
		}
	}

	m_rem = QStringView(it, end);

	return QCToken(QCToken::Comment, view, tokLoc);
}

QCToken QCLexer::lex(){
	auto it = std::begin(m_rem);
	const auto end = std::end(m_rem);

	if(it == end) return QCToken(QCToken::EndOfFile, QStringView(), m_curLoc);

	while(it->isSpace()){
		if(*it == u'\n'){
			++m_curLoc.line;
			m_curLoc.col = 0;
		}
		else{
			++m_curLoc.col;
		}

		++it;
	}

	if(it == end || *it == u'\0'){
		m_rem = QStringView(end, end);
		return QCToken(QCToken::EndOfFile, m_rem, m_curLoc);
	}

	if(m_mode == Mode::MultilineComment) return lexMultilineComment(it, end);
	else return lexNormal(it, end);
}
