#pragma once

#include "AST.hpp"

#include <sstream>
#include <string>
#include <vector>

struct Triple {
	std::string op;
	std::string arg1;
	std::string arg2;
};

class TripleGenerator {
public:
	TripleGenerator() = default;
	std::string generate(const Program& program);

private:
	std::vector<Triple> triples;
	int labelCounter = 0;

	void reset();
	int emitTriple(const std::string& op, const std::string& arg1 = "", const std::string& arg2 = "");
	std::string ref(int index) const;
	std::string newLabel();

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

	std::string format() const;
};
