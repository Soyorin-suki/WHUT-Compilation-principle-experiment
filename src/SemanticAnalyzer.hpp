#pragma once

#include "AST.hpp"
#include "SymbolTable.hpp"

#include <optional>
#include <stdexcept>
#include <string>

class SemanticError : public std::runtime_error {
public:
	explicit SemanticError(const std::string& msg) : std::runtime_error(msg) {}
};

class SemanticAnalyzer {
public:
	SemanticAnalyzer() = default;
	void analyze(const Program& program);

private:
	SymbolTable symbols;
	std::optional<Type> currentFunctionReturnType;

	void analyzeDecl(const Decl& decl);
	void analyzeVarDecl(const VarDecl& decl, bool isGlobal);
	void analyzeFunDeclSignature(const FunDecl& decl);
	void analyzeFunDeclBody(const FunDecl& decl);

	void analyzeStmt(const Stmt& stmt);
	void analyzeCompound(const CompoundStmt& stmt);
	void analyzeIf(const IfStmt& stmt);
	void analyzeWhile(const WhileStmt& stmt);
	void analyzeFor(const ForStmt& stmt);
	void analyzeReturn(const ReturnStmt& stmt);
	void analyzeExprStmt(const ExprStmt& stmt);

	Type analyzeExpr(const Expr& expr);
	Type analyzeAssignExpr(const AssignExpr& expr);
	Type analyzeBinaryExpr(const BinaryExpr& expr);
	Type analyzeUnaryExpr(const UnaryExpr& expr);
	Type analyzeCallExpr(const CallExpr& expr);
	Type analyzeValExpr(const ValExpr& expr);
	Type analyzeIntLiteral(const IntLiteral& expr);
	Type analyzeCharLiteral(const CharLiteral& expr);
	Type analyzeDoubleLiteral(const DoubleLiteral& expr);

	static bool isNumeric(Type t);
	static int numericRank(Type t);
	static Type commonNumericType(Type a, Type b);
	static bool canWiden(Type from, Type to);
	static std::string typeName(Type t);

	[[noreturn]] void errorAt(const ASTNode& node, const std::string& msg) const;
};

