#pragma once
#include "Lexer.hpp"
#include "Token.hpp"
#include "AST.hpp"
#include <vector>
#include <stdexcept>

class Parser{
	private:
	std::vector<Token>tokens;
	size_t pos=0;

	// 当前Token
	Token& curToken();
	// 查看相对位置offset的Token
	Token& peek(int offset=1);
	// 步进
	void advance();
	// 检查当前Token的TokenType是否为t
	bool check(TokenType t);
	// 消费当前Token
	void expect(TokenType t);

	// 递归下降解析函数
	// 处理程序
	// useless now
	ProgramPtr parseProgram();
	// 处理声明
	DeclPtr parseDecl();
	// 处理变量声明
	DeclPtr parseVarDecl();
	// 处理函数声明
	DeclPtr parseFunDecl(Type returnType, const std::string &name);
	// 处理语句
	StmtPtr parseStmt();
	// 处理复合体
	std::unique_ptr<CompoundStmt> parseCompoundStmt();
	// 处理表达式
	ExprPtr parseExpr();
	// 处理赋值表达式
	ExprPtr parseAssignmentExpr();
	// 处理逻辑or表达式
	ExprPtr parseLogicalOrExpr();
	// 处理逻辑and表达式
	ExprPtr parseLogicalAndExpr();
	// 处理等于表达式
	ExprPtr parseEqualityExpr();
	// 处理二元表达式
	ExprPtr parseRelationalExpr();
	// 处理加法表达式
	ExprPtr parseAdditiveExpr();
	// 处理乘法表达式
	ExprPtr parseMultiplicativeExpr();
	// 处理一元表达式
	ExprPtr parseUnaryExpr();
	// 处理主表达式
	ExprPtr parsePrimaryExpr();

	public:
	Parser(const std::vector<Token>& tokens);
	Parser();
	void setTokens(const std::vector<Token>& tokens);
	ProgramPtr parse();

};