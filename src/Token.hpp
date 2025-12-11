#pragma once
#include "TokenType.hpp"
#include <string>
#include <iostream>

struct Token{
	TokenType type;
	std::string lexeme;
	int line,colomn;
	friend std::ostream&operator<<(std::ostream& os,const Token& token);
	Token(TokenType tokenType=TokenType::Unknown,int line=0,int colomn=0,std::string lexeme="");
};