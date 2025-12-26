#pragma once

#include "AST.hpp"

#include <sstream>
#include <string>

class TACGenerator {
public:
	TACGenerator() = default;
	std::string generate(const Program& program);

private:
	int tempCounter = 0;
	int labelCounter = 0;
	std::ostringstream out;

	void reset();
	std::string newTemp();
	std::string newLabel();

	void emit(const std::string& line);
	void emitLabel(const std::string& label);

	void genDecl(const Decl& decl);
	void genVarDecl(const VarDecl& decl);
	void genFunDecl(const FunDecl& decl);

	void genStmt(const Stmt& stmt);
	void genCompound(const CompoundStmt& stmt);
	void genIf(const IfStmt& stmt);
	void genReturn(const ReturnStmt& stmt);
	void genExprStmt(const ExprStmt& stmt);

	std::string genExpr(const Expr& expr);
	std::string genAssignExpr(const AssignExpr& expr);
	std::string genBinaryExpr(const BinaryExpr& expr);
	std::string genUnaryExpr(const UnaryExpr& expr);
	std::string genCallExprValue(const CallExpr& expr);
	void genCallExprStmt(const CallExpr& expr);
	std::string genValExpr(const ValExpr& expr);
	std::string genIntLiteral(const IntLiteral& expr);
	std::string genCharLiteral(const CharLiteral& expr);
	std::string genDoubleLiteral(const DoubleLiteral& expr);
};
