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

		case TokenType::Eof:
			os<<"EndOfFile";break;
	}
	os<<", "<<token.lexeme<<">";
	return os;
}

std::string Token::to_string()const{
	std::string s;
	s+="<";
	switch(this->type){
		case TokenType::Unknown:
			s+="Unknown";break;
		case TokenType::Identifier:
			s+="Identifier";break;

		case TokenType::kw_void:
			s+="kw_void";break;
		case TokenType::kw_int:
			s+="kw_int";break;
		case TokenType::kw_char:
			s+="kw_char";break;
		case TokenType::kw_float:
			s+="kw_float";break;
		case TokenType::kw_double:
			s+="kw_double";break;
		case TokenType::kw_long:
			s+="kw_long";break;

		case TokenType::kw_if:
			s+="kw_if";break;
		case TokenType::kw_for:
			s+="kw_for";break;
		case TokenType::kw_while:
			s+="kw_while";break;
		case TokenType::kw_continue:
			s+="kw_continue";break;
		case TokenType::kw_break:
			s+="kw_break";break;
		case TokenType::kw_return:
			s+="kw_return";break;

		case TokenType::IntLiterial:
			s+="IntLiterial";break;
		case TokenType::DoubleLiterial:
			s+="DoubleLiterial";break;
		case TokenType::StringLiterial:
			s+="StringLiterial";break;
		case TokenType::CharLiterial:
			s+="CharLiterial";break;

		case TokenType::Semicolon:
			s+="Semicolon";break;
		case TokenType::Comma:
			s+="Comma";break;
		case TokenType::LParen:
			s+="LParen";break;
		case TokenType::RParen:
			s+="RParen";break;
		case TokenType::LBrace:
			s+="LBrace";break;
		case TokenType::RBrace:
			s+="RBrace";break;
		case TokenType::LBracket:
			s+="LBracket";break;
		case TokenType::RBracket:
			s+="RBracket";break;

		case TokenType::Plus:
			s+="Plus";break;
		case TokenType::Minus:
			s+="Minus";break;
		case TokenType::Star:
			s+="Star";break;
		case TokenType::Slash:
			s+="Slash";break;
		case TokenType::Percent:
			s+="Percent";break;
		case TokenType::Assign:
			s+="Assign";break;
		case TokenType::Equal:
			s+="Equal";break;
		case TokenType::NotEq:
			s+="NotEq";break;
		case TokenType::Less:
			s+="Less";break;
		case TokenType::Greater:
			s+="Greater";break;
		case TokenType::LessEq:
			s+="LessEq";break;
		case TokenType::GreaterEq:
			s+="GreaterEq";break;
		case TokenType::PlusPlus:
			s+="PlusPlus";break;
		case TokenType::MinusMinus:
			s+="MinusMinus";break;
		case TokenType::PlusEq:
			s+="PlusEq";break;
		case TokenType::MinusEq:
			s+="MinusEq";break;
		case TokenType::StarEq:
			s+="StarEq";break;
		case TokenType::SlashEq:
			s+="SlashEq";break;
		case TokenType::PercentEq:
			s+="PercentEq";break;
		case TokenType::LeftShift:
			s+="LeftShift";break;
		case TokenType::RightShift:
			s+="RightShift";break;

		case TokenType::Eof:
			s+="EndOfFile";break;
	}
	s+=", ";
	s+=this->lexeme;
	s+=">";
	return s;
}

