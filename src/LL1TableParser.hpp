#pragma once

#include "Token.hpp"
#include "TokenType.hpp"

#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class LL1TableParser {
public:
	struct Result {
		bool success = false;
		std::string trace;
		std::string error;
	};

	LL1TableParser();

	// Runs a table-driven LL(1) parse and returns a human-readable trace.
	Result parseAndTrace(const std::vector<Token>& tokens) const;

private:
	enum class NT {
		Program,
		DeclList,
		Decl,
		TypeSpec,
		DeclAfterId,
		ParamClause,
		ParamList,
		ParamListTail,
		Param,
		ParamTypeSpec,
		CompoundStmt,
		LocalDecls,
		LocalDecl,
		LocalInitOpt,
		StmtList,
		Stmt,
		IfStmt,
		ElseOpt,
		WhileStmt,
		ForStmt,
		ExprOpt,
		ReturnStmt,
		ReturnExprOpt,
		ExprStmt,
		Expr,
		Assignment,
		AssignmentTail,
		LogicalOr,
		LogicalOrTail,
		LogicalAnd,
		LogicalAndTail,
		Equality,
		EqualityTail,
		Relational,
		RelationalTail,
		Additive,
		AdditiveTail,
		Multiplicative,
		MultiplicativeTail,
		Unary,
		UnaryOp,
		Primary,
		PrimaryAfterId,
		ArgListOpt,
		ArgListTail,
	};

	struct Sym {
		bool isTerminal = false;
		TokenType term = TokenType::Unknown;
		NT nonterm = NT::Program;
	};

	struct Production {
		NT lhs;
		std::vector<Sym> rhs; // empty => epsilon
	};

	struct FirstSet {
		std::set<TokenType> terms;
		bool hasEps = false;
	};

	std::vector<Production> prods;
	std::unordered_map<int, std::vector<int>> prodsByLhs; // NT -> [prod indices]

	std::unordered_map<int, FirstSet> first;
	std::unordered_map<int, std::set<TokenType>> follow;
	std::unordered_map<int, std::unordered_map<TokenType, int>> table; // NT -> (terminal -> prod index)

	static int nti(NT nt);

	static Sym T(TokenType t);
	static Sym N(NT nt);

	static std::string ntToString(NT nt);
	static std::string symToString(const Sym& s);

	void buildGrammar();
	void buildIndexes();
	void computeFirst();
	void computeFollow();
	void buildParseTableOrThrow();

	FirstSet firstOfSequence(const std::vector<Sym>& seq, size_t startIndex = 0) const;

	std::string productionToString(int prodIndex) const;
	static std::string tokenShort(const Token& tok);
	static std::string tokenTypeShort(TokenType t);

	static std::string stackToString(const std::vector<Sym>& st);
	static std::string inputPreview(const std::vector<Token>& tokens, size_t pos, size_t maxCount = 6);
};
