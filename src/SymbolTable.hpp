#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "AST.hpp"

enum class SymbolKind { Var, Func, Param };

struct FuncSig {
	Type returnType = Type::VOID;
	std::vector<Type> paramTypes;
};

struct Symbol {
	SymbolKind kind = SymbolKind::Var;
	Type type = Type::VOID;
	FuncSig func;
	int declLine = 0;
	int declCol = 0;
};

struct Scope {
	std::unordered_map<std::string, Symbol> table;
};

class SymbolTable {
private:
	std::vector<Scope> scopes;

public:
	SymbolTable();
	void reset();

	void enterScope();
	void leaveScope();

	bool declare(const std::string& name, const Symbol& sym);
	const Symbol* lookup(const std::string& name) const;
	const Symbol* lookupCurrent(const std::string& name) const;
};