#pragma once

enum class TokenType{
	Unknown,		//未知，识别失败

	Identifier,		//标识符
		
	kw_void,		// void
	kw_int,			// int
	kw_char,		// char
	kw_float,		// float
	kw_double,		// double
	kw_long,		// long

	kw_if,			// if
	kw_for,			// for
	kw_while,		// while
	kw_continue,	// continue
	kw_break,		// break
	kw_return,		// return
	
	IntLiterial,	// int字面量
	DoubleLiterial,	// double字面量
	StringLiterial,	// 字符串字面量
	CharLiterial,	// char字面量

	Semicolon,		// ;
	Comma,			// ,
	LParen,			// (
	RParen,			// )
	LBrace,			// {
	RBrace,			// }
	LBracket,		// [
	RBracket,		// ]

	Plus,			// +
	Minus,			// -
	Star,			// *
	Slash,			// /
	Percent,		// %
	Assign,			// =
	Equal,			// ==
	NotEq,			// !=
	Less,			// <
	Greater,		// >
	LessEq,			// <=
	GreaterEq,		// >=
	PlusPlus,		// ++
	MinusMinus,		// --
	PlusEq,			// +=
	MinusEq,		// -=
	StarEq,			// *=
	SlashEq,		// /=
	PercentEq,		// %=

	LeftShift,		// <<
	RightShift,		// >>

	Eof				// EOF

};