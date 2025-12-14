#pragma once
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "Token.hpp"

class Lexer{
private:
	std::string text;
	std::vector<Token>tokens;
	int nowLine,nowColumn;
	size_t nowPos;
	std::unordered_map<std::string,TokenType>keywords;
	std::unordered_map<std::string,TokenType>opts;

	char peak(size_t k=0)const;
	char get();
	bool eof()const;
	void skipSpace();
	bool endChar(size_t k=0)const;
	void skip(Token token);

	Token scanNumber();
	Token scanString();
	Token scanChar();
	Token scanIdentifier();
	Token scanOperatorOrPunct();
	Token scanKeyword();
	Token getNextToken();
public:
	Lexer();
	std::vector<Token>getTokens()const;
	void setText(std::string text);
	void doLexer();
};