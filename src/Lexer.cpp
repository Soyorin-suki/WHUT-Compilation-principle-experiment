#include "Lexer.hpp"
#include <queue>

std::vector<Token> Lexer::getTokens()const{
	return tokens;
}

Lexer::Lexer(){
	tokens.clear();
	nowLine=nowColomn=1;
	nowPos=0;
	keywords.clear();
	keywords["void"]=TokenType::kw_void;
	keywords["int"]=TokenType::kw_int;
	keywords["char"]=TokenType::kw_char;
	keywords["float"]=TokenType::kw_float;
	keywords["double"]=TokenType::kw_double;
	keywords["long"]=TokenType::kw_long;

	keywords["if"]=TokenType::kw_if;
	keywords["for"]=TokenType::kw_for;
	keywords["while"]=TokenType::kw_while;
	keywords["continue"]=TokenType::kw_continue;
	keywords["break"]=TokenType::kw_break;
	keywords["return"]=TokenType::kw_return;

	opts["++"]=TokenType::PlusPlus;
	opts["--"]=TokenType::MinusMinus;
	opts["+="]=TokenType::PlusEq;
	opts["-="]=TokenType::MinusEq;
	opts["*="]=TokenType::StarEq;
	opts["/="]=TokenType::SlashEq;
	opts["%="]=TokenType::PercentEq;
	opts["!="]=TokenType::NotEq;
	opts["<="]=TokenType::LessEq;
	opts[">="]=TokenType::GreaterEq;
	opts["=="]=TokenType::Equal;
	opts[";"]=TokenType::Semicolon;
	opts[","]=TokenType::Comma;
	opts["("]=TokenType::LParen;
	opts[")"]=TokenType::RParen;
	opts["{"]=TokenType::LBrace;
	opts["}"]=TokenType::RBrace;
	opts["["]=TokenType::LBracket;
	opts["]"]=TokenType::RBracket;
	opts["<"]=TokenType::Less;
	opts[">"]=TokenType::Greater;
	opts["+"]=TokenType::Plus;
	opts["-"]=TokenType::Minus;
	opts["*"]=TokenType::Star;
	opts["/"]=TokenType::Slash;
	opts["="]=TokenType::Assign;
	opts["%"]=TokenType::Percent;
	
}

void Lexer::setText(std::string text){
	this->text=text;
	tokens.clear();
	nowLine=nowColomn=1;
	nowPos=0;
}

char Lexer::peak(size_t k)const{
	if(text.length()<=nowPos+k)return EOF;
	return text[nowPos+k];
}


char Lexer::get(){
	char ch=peak();
	if(ch=='\n'){
		nowLine++;
		nowColomn=1;
	}else nowColomn++;
	nowPos++;
	return ch;
}


bool Lexer::eof()const{
	return nowPos>=text.length();
}


void Lexer::skipSpace(){
	while(peak()==' '||peak()=='\n')get();
}

bool Lexer::endChar(size_t k)const{
	return peak(k)==' '||peak(k)=='\n'||peak(k)==EOF;
}

void Lexer::skip(Token token){
	int n=token.lexeme.length();
	while(n--)get();
}


Token Lexer::scanNumber(){
	std::string tmpText;
	int tmpPos=0,nowSituation=0;
	while(nowSituation!=2&&nowSituation!=4){
		if(endChar(tmpPos))break;
		char nowCh=peak(tmpPos);
		if(!(isalnum(nowCh)||nowCh=='_'||nowCh=='.'))break;
		if(nowSituation==0){
			if(isdigit(nowCh)){
				tmpText+=nowCh;
				nowSituation=1;
			}else {
				nowSituation=5;
			}
		}else if(nowSituation==1){
			if(isdigit(nowCh)){
				tmpText+=nowCh;
				nowSituation=1;
			}else if(nowCh=='.'){
				tmpText+=nowCh;
				nowSituation=3;
			}else if(isalpha(nowCh)||nowCh=='_'){
				tmpText+=nowCh;
				nowSituation=5;
			}else {
				nowSituation=2;
			}
		}else if(nowSituation==3){
			if(isdigit(nowCh)){
				tmpText+=nowCh;
				nowSituation=3;
			}else if(isalpha(nowCh)||nowCh=='_'){
				tmpText+=nowCh;
				nowSituation=5;
			}else {
				nowSituation=4;
			}
		}else if(nowSituation==5){
			tmpText+=nowCh;
		}
		tmpPos++;
	}
	if(nowSituation==2||nowSituation==1)return Token(TokenType::IntLiterial,nowLine,nowColomn,tmpText);
	else if(nowSituation==4||nowSituation==3)return Token(TokenType::DoubleLiterial,nowLine,nowColomn,tmpText);
	else return Token(TokenType::Unknown,nowLine,nowColomn,tmpText);
}

Token Lexer::scanString(){
	std::string tmpText;
	int tmpPos=0,nowSituation=0;
	while(nowSituation!=3&&nowSituation!=4){
		if(eof()||peak(tmpPos)=='\n'){
			nowSituation=4;
			break;
		}
		char nowCh=peak(tmpPos);
		if(nowSituation==0){
			if(nowCh=='\"'){
				tmpText+=nowCh;
				nowSituation=1;
			}else return Token(TokenType::Unknown,nowLine,nowColomn,tmpText);
		}
		else if(nowSituation==1){
			if(nowCh=='\"'){
				tmpText+=nowCh;
				nowSituation=3;
			}else if(nowCh=='\\'){
				tmpText+=nowCh;
				nowSituation=2;
			}else {
				tmpText+=nowCh;
				nowSituation=1;
			}
		}else if(nowSituation==2){
			tmpText+=nowCh;
			nowSituation=1;
		}
		tmpPos++;
	}
	if(nowSituation==3){
		return Token(TokenType::StringLiterial,nowLine,nowColomn,tmpText);
	}else{
		return Token(TokenType::Unknown,nowLine,nowColomn,tmpText);
	}
}

Token Lexer::scanChar(){
	std::string tmpText;
	int tmpPos=0,nowSituation=0;
	while(nowSituation!=4){
		if(endChar(tmpPos))break;
		char nowCh=peak(tmpPos);
		if(nowSituation==0){
			if(nowCh=='\''){
				tmpText+=nowCh;
				nowSituation=1;
			}else break;
		}else if(nowSituation==1){
			if(nowCh=='\\'){
				tmpText+=nowCh;
				nowSituation=2;
			}else {
				tmpText+=nowCh;
				nowSituation=3;
			}
		}else if(nowSituation==2){
			tmpText+=nowCh;
			nowSituation=3;
		}else if(nowSituation==3){
			tmpText+=nowCh;
			if(nowCh=='\''){
				nowSituation=4;
			}else break;
		}
		tmpPos++;
	}
	if(nowSituation==4){
		return Token(TokenType::CharLiterial,nowLine,nowColomn,tmpText);
	}else return Token(TokenType::Unknown,nowLine,nowColomn,tmpText);
}

Token Lexer::scanIdentifier(){
	std::string tmpText;
	int tmpPos=0,nowSituasion=0;
	while(nowSituasion!=2){
		if(endChar(tmpPos))break;
		char nowChar=peak(tmpPos);
		if(nowSituasion==0){
			if(isalpha(nowChar)||nowChar=='_'){
				tmpText+=nowChar;
				nowSituasion=1;
			}else break;
		}else if(nowSituasion==1){
			if(isalnum(nowChar)||nowChar=='_'){
				tmpText+=nowChar;
				nowSituasion=1;
			}else break;
		}
		tmpPos++;
	}
	if(nowSituasion==1){
		if(keywords.count(tmpText))return Token(keywords[tmpText],nowLine,nowColomn,tmpText);
		else return Token(TokenType::Identifier,nowLine,nowColomn,tmpText);
	}else {
		return Token(TokenType::Unknown,nowLine,nowColomn,tmpText);
	}
}

Token Lexer::scanOperatorOrPunct(){
	std::string tmpText;
	int tmpPos=0;
	if(!endChar(0))tmpText+=peak(0);
	if(!endChar(1))tmpText+=peak(1);
	if(tmpText.length()==2&&opts.count(tmpText)){
		return Token(opts[tmpText],nowLine,nowColomn,tmpText);
	}
	if(tmpText.length()==2)tmpText.pop_back();
	if(tmpText.length()==1&&opts.count(tmpText)){
		return Token(opts[tmpText],nowLine,nowColomn,tmpText);
	}else return Token(TokenType::Unknown,nowLine,nowColomn,tmpText);
}

Token Lexer::scanKeyword(){
	std::string tmpText;
	for(int i=0;i<8;++i){
		tmpText+=peak(i);
		if(endChar(i+1))break;
		if(keywords.count(tmpText)){
			return Token(keywords[tmpText],nowLine,nowColomn,tmpText);
		}
	}
	if(keywords.count(tmpText)){
		return Token(keywords[tmpText],nowLine,nowColomn,tmpText);
	}
	return Token(TokenType::Unknown,nowLine,nowColomn);
}

Token Lexer::getNextToken(){
	Token token;
	// token=scanKeyword();
	// if(token.type!=TokenType::Unknown){
	// 	skip(token);
	// 	return token;
	// }
	char nowCh=peak();
	if(isalpha(nowCh)||nowCh=='_')token=scanIdentifier();
	else if(isdigit(nowCh))token=scanNumber();
	else if(nowCh=='\'')token=scanChar();
	else if(nowCh=='"')token=scanString();
	else token=scanOperatorOrPunct();
	skip(token);
	return token;
}


void Lexer::doLexer(){
	Token token;
	skipSpace();
	while(!eof()){
		token=getNextToken();
		tokens.push_back(token);
		skipSpace();
	}
}