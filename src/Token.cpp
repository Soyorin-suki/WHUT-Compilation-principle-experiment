#include "Token.hpp"


Token::Token(TokenType tokenType,int line,int column,std::string lexeme):
			type(tokenType),
			line(line),
			column(column),
			lexeme(lexeme){}


std::ostream& operator<<(std::ostream& os,const Token& token){
	os<<"<";
	switch(token.type){
		case TokenType::Unknown:
			os<<"Unknown";break;
		case TokenType::Identifier:
			os<<"Identifier";break;

		case TokenType::kw_void:
			os<<"kw_void";break;
		case TokenType::kw_int:
			os<<"kw_int";break;
		case TokenType::kw_char:
			os<<"kw_char";break;
		case TokenType::kw_float:
			os<<"kw_float";break;
		case TokenType::kw_double:
			os<<"kw_double";break;
		case TokenType::kw_long:
			os<<"kw_long";break;

		case TokenType::kw_if:
			os<<"kw_if";break;
		case TokenType::kw_for:
			os<<"kw_for";break;
		case TokenType::kw_while:
			os<<"kw_while";break;
		case TokenType::kw_continue:
			os<<"kw_continue";break;
		case TokenType::kw_break:
			os<<"kw_break";break;
		case TokenType::kw_return:
			os<<"kw_return";break;

		case TokenType::IntLiterial:
			os<<"IntLiterial";break;
		case TokenType::DoubleLiterial:
			os<<"DoubleLiterial";break;
		case TokenType::StringLiterial:
			os<<"StringLiterial";break;
		case TokenType::CharLiterial:
			os<<"CharLiterial";break;

		case TokenType::Semicolon:
			os<<"Semicolon";break;
		case TokenType::Comma:
			os<<"Comma";break;
		case TokenType::LParen:
			os<<"LParen";break;
		case TokenType::RParen:
			os<<"RParen";break;
		case TokenType::LBrace:
			os<<"LBrace";break;
		case TokenType::RBrace:
			os<<"RBrace";break;
		case TokenType::LBracket:
			os<<"LBracket";break;
		case TokenType::RBracket:
			os<<"RBracket";break;

		case TokenType::Plus:
			os<<"Plus";break;
		case TokenType::Minus:
			os<<"Minus";break;
		case TokenType::Star:
			os<<"Star";break;
		case TokenType::Slash:
			os<<"Slash";break;
		case TokenType::Percent:
			os<<"Percent";break;
		case TokenType::Assign:
			os<<"Assign";break;
		case TokenType::Equal:
			os<<"Equal";break;
		case TokenType::NotEq:
			os<<"NotEq";break;
		case TokenType::Less:
			os<<"Less";break;
		case TokenType::Greater:
			os<<"Greater";break;
		case TokenType::LessEq:
			os<<"LessEq";break;
		case TokenType::GreaterEq:
			os<<"GreaterEq";break;
		case TokenType::PlusPlus:
			os<<"PlusPlus";break;
		case TokenType::MinusMinus:
			os<<"MinusMinus";break;
		case TokenType::PlusEq:
			os<<"PlusEq";break;
		case TokenType::MinusEq:
			os<<"MinusEq";break;
		case TokenType::StarEq:
			os<<"StarEq";break;
		case TokenType::SlashEq:
			os<<"SlashEq";break;
		case TokenType::PercentEq:
			os<<"PercentEq";break;
		case TokenType::LeftShift:
			os<<"LeftShift";break;
		case TokenType::RightShift:
			os<<"RightShift";break;
	}
	os<<", "<<token.lexeme<<">";
	return os;
}