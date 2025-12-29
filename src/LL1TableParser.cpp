#include "LL1TableParser.hpp"

#include <iomanip>
#include <stdexcept>

int LL1TableParser::nti(NT nt) {
	return static_cast<int>(nt);
}

LL1TableParser::Sym LL1TableParser::T(TokenType t) {
	Sym s;
	s.isTerminal = true;
	s.term = t;
	return s;
}

LL1TableParser::Sym LL1TableParser::N(NT nt) {
	Sym s;
	s.isTerminal = false;
	s.nonterm = nt;
	return s;
}

LL1TableParser::LL1TableParser() {
	buildGrammar();
	buildIndexes();
	computeFirst();
	computeFollow();
	buildParseTableOrThrow();
}

void LL1TableParser::buildGrammar() {
	prods.clear();

	// Program -> DeclList
	prods.push_back({NT::Program, {N(NT::DeclList)}});

	// DeclList -> Decl DeclList | eps
	prods.push_back({NT::DeclList, {N(NT::Decl), N(NT::DeclList)}});
	prods.push_back({NT::DeclList, {}});

	// Decl -> TypeSpec Identifier DeclAfterId
	prods.push_back({NT::Decl, {N(NT::TypeSpec), T(TokenType::Identifier), N(NT::DeclAfterId)}});

	// TypeSpec -> int | char | void | double
	prods.push_back({NT::TypeSpec, {T(TokenType::kw_int)}});
	prods.push_back({NT::TypeSpec, {T(TokenType::kw_char)}});
	prods.push_back({NT::TypeSpec, {T(TokenType::kw_void)}});
	prods.push_back({NT::TypeSpec, {T(TokenType::kw_double)}});

	// DeclAfterId -> ';' | '=' Expr ';' | '(' ParamClause ')' CompoundStmt
	prods.push_back({NT::DeclAfterId, {T(TokenType::Semicolon)}});
	prods.push_back({NT::DeclAfterId, {T(TokenType::Assign), N(NT::Expr), T(TokenType::Semicolon)}});
	prods.push_back({NT::DeclAfterId,
		{T(TokenType::LParen), N(NT::ParamClause), T(TokenType::RParen), N(NT::CompoundStmt)}});

	// ParamClause -> 'void' | ParamList | eps
	prods.push_back({NT::ParamClause, {T(TokenType::kw_void)}});
	prods.push_back({NT::ParamClause, {N(NT::ParamList)}});
	prods.push_back({NT::ParamClause, {}});

	// ParamList -> Param ParamListTail
	prods.push_back({NT::ParamList, {N(NT::Param), N(NT::ParamListTail)}});

	// ParamListTail -> ',' Param ParamListTail | eps
	prods.push_back({NT::ParamListTail, {T(TokenType::Comma), N(NT::Param), N(NT::ParamListTail)}});
	prods.push_back({NT::ParamListTail, {}});

	// Param -> ParamTypeSpec Identifier
	prods.push_back({NT::Param, {N(NT::ParamTypeSpec), T(TokenType::Identifier)}});

	// ParamTypeSpec -> int | char | double
	prods.push_back({NT::ParamTypeSpec, {T(TokenType::kw_int)}});
	prods.push_back({NT::ParamTypeSpec, {T(TokenType::kw_char)}});
	prods.push_back({NT::ParamTypeSpec, {T(TokenType::kw_double)}});

	// CompoundStmt -> '{' LocalDecls StmtList '}'
	prods.push_back({NT::CompoundStmt,
		{T(TokenType::LBrace), N(NT::LocalDecls), N(NT::StmtList), T(TokenType::RBrace)}});

	// LocalDecls -> LocalDecl LocalDecls | eps
	prods.push_back({NT::LocalDecls, {N(NT::LocalDecl), N(NT::LocalDecls)}});
	prods.push_back({NT::LocalDecls, {}});

	// LocalDecl -> TypeSpec Identifier LocalInitOpt ';'
	prods.push_back({NT::LocalDecl,
		{N(NT::TypeSpec), T(TokenType::Identifier), N(NT::LocalInitOpt), T(TokenType::Semicolon)}});

	// LocalInitOpt -> '=' Expr | eps
	prods.push_back({NT::LocalInitOpt, {T(TokenType::Assign), N(NT::Expr)}});
	prods.push_back({NT::LocalInitOpt, {}});

	// StmtList -> Stmt StmtList | eps
	prods.push_back({NT::StmtList, {N(NT::Stmt), N(NT::StmtList)}});
	prods.push_back({NT::StmtList, {}});

	// Stmt -> CompoundStmt | IfStmt | WhileStmt | ForStmt | ReturnStmt | ExprStmt
	prods.push_back({NT::Stmt, {N(NT::CompoundStmt)}});
	prods.push_back({NT::Stmt, {N(NT::IfStmt)}});
	prods.push_back({NT::Stmt, {N(NT::WhileStmt)}});
	prods.push_back({NT::Stmt, {N(NT::ForStmt)}});
	prods.push_back({NT::Stmt, {N(NT::ReturnStmt)}});
	prods.push_back({NT::Stmt, {N(NT::ExprStmt)}});

	// IfStmt -> if '(' Expr ')' Stmt ElseOpt
	prods.push_back({NT::IfStmt,
		{T(TokenType::kw_if), T(TokenType::LParen), N(NT::Expr), T(TokenType::RParen), N(NT::Stmt), N(NT::ElseOpt)}});

	// ElseOpt -> else Stmt | eps
	prods.push_back({NT::ElseOpt, {T(TokenType::kw_else), N(NT::Stmt)}});
	prods.push_back({NT::ElseOpt, {}});

	// WhileStmt -> while '(' Expr ')' Stmt
	prods.push_back({NT::WhileStmt,
		{T(TokenType::kw_while), T(TokenType::LParen), N(NT::Expr), T(TokenType::RParen), N(NT::Stmt)}});

	// ForStmt -> for '(' ExprOpt ';' ExprOpt ';' ExprOpt ')' Stmt
	prods.push_back({NT::ForStmt,
		{T(TokenType::kw_for), T(TokenType::LParen), N(NT::ExprOpt), T(TokenType::Semicolon), N(NT::ExprOpt),
		 T(TokenType::Semicolon), N(NT::ExprOpt), T(TokenType::RParen), N(NT::Stmt)}});

	// ExprOpt -> Expr | eps
	prods.push_back({NT::ExprOpt, {N(NT::Expr)}});
	prods.push_back({NT::ExprOpt, {}});

	// ReturnStmt -> return ReturnExprOpt ';'
	prods.push_back({NT::ReturnStmt, {T(TokenType::kw_return), N(NT::ReturnExprOpt), T(TokenType::Semicolon)}});

	// ReturnExprOpt -> Expr | eps
	prods.push_back({NT::ReturnExprOpt, {N(NT::Expr)}});
	prods.push_back({NT::ReturnExprOpt, {}});

	// ExprStmt -> Expr ';'
	prods.push_back({NT::ExprStmt, {N(NT::Expr), T(TokenType::Semicolon)}});

	// Expr -> Assignment
	prods.push_back({NT::Expr, {N(NT::Assignment)}});

	// Assignment -> LogicalOr AssignmentTail
	prods.push_back({NT::Assignment, {N(NT::LogicalOr), N(NT::AssignmentTail)}});

	// AssignmentTail -> '=' Assignment | eps
	prods.push_back({NT::AssignmentTail, {T(TokenType::Assign), N(NT::Assignment)}});
	prods.push_back({NT::AssignmentTail, {}});

	// LogicalOr -> LogicalAnd LogicalOrTail
	prods.push_back({NT::LogicalOr, {N(NT::LogicalAnd), N(NT::LogicalOrTail)}});
	// LogicalOrTail -> '||' LogicalAnd LogicalOrTail | eps
	prods.push_back({NT::LogicalOrTail, {T(TokenType::LogicalOr), N(NT::LogicalAnd), N(NT::LogicalOrTail)}});
	prods.push_back({NT::LogicalOrTail, {}});

	// LogicalAnd -> Equality LogicalAndTail
	prods.push_back({NT::LogicalAnd, {N(NT::Equality), N(NT::LogicalAndTail)}});
	// LogicalAndTail -> '&&' Equality LogicalAndTail | eps
	prods.push_back({NT::LogicalAndTail, {T(TokenType::LogicalAnd), N(NT::Equality), N(NT::LogicalAndTail)}});
	prods.push_back({NT::LogicalAndTail, {}});

	// Equality -> Relational EqualityTail
	prods.push_back({NT::Equality, {N(NT::Relational), N(NT::EqualityTail)}});
	// EqualityTail -> '==' Relational EqualityTail | '!=' Relational EqualityTail | eps
	prods.push_back({NT::EqualityTail, {T(TokenType::Equal), N(NT::Relational), N(NT::EqualityTail)}});
	prods.push_back({NT::EqualityTail, {T(TokenType::NotEq), N(NT::Relational), N(NT::EqualityTail)}});
	prods.push_back({NT::EqualityTail, {}});

	// Relational -> Additive RelationalTail
	prods.push_back({NT::Relational, {N(NT::Additive), N(NT::RelationalTail)}});
	// RelationalTail -> < Additive ... | > ... | <= ... | >= ... | eps
	prods.push_back({NT::RelationalTail, {T(TokenType::Less), N(NT::Additive), N(NT::RelationalTail)}});
	prods.push_back({NT::RelationalTail, {T(TokenType::Greater), N(NT::Additive), N(NT::RelationalTail)}});
	prods.push_back({NT::RelationalTail, {T(TokenType::LessEq), N(NT::Additive), N(NT::RelationalTail)}});
	prods.push_back({NT::RelationalTail, {T(TokenType::GreaterEq), N(NT::Additive), N(NT::RelationalTail)}});
	prods.push_back({NT::RelationalTail, {}});

	// Additive -> Multiplicative AdditiveTail
	prods.push_back({NT::Additive, {N(NT::Multiplicative), N(NT::AdditiveTail)}});
	// AdditiveTail -> + Multiplicative ... | - ... | eps
	prods.push_back({NT::AdditiveTail, {T(TokenType::Plus), N(NT::Multiplicative), N(NT::AdditiveTail)}});
	prods.push_back({NT::AdditiveTail, {T(TokenType::Minus), N(NT::Multiplicative), N(NT::AdditiveTail)}});
	prods.push_back({NT::AdditiveTail, {}});

	// Multiplicative -> Unary MultiplicativeTail
	prods.push_back({NT::Multiplicative, {N(NT::Unary), N(NT::MultiplicativeTail)}});
	// MultiplicativeTail -> * Unary ... | / ... | % ... | eps
	prods.push_back({NT::MultiplicativeTail, {T(TokenType::Star), N(NT::Unary), N(NT::MultiplicativeTail)}});
	prods.push_back({NT::MultiplicativeTail, {T(TokenType::Slash), N(NT::Unary), N(NT::MultiplicativeTail)}});
	prods.push_back({NT::MultiplicativeTail, {T(TokenType::Percent), N(NT::Unary), N(NT::MultiplicativeTail)}});
	prods.push_back({NT::MultiplicativeTail, {}});

	// Unary -> UnaryOp Unary | Primary
	prods.push_back({NT::Unary, {N(NT::UnaryOp), N(NT::Unary)}});
	prods.push_back({NT::Unary, {N(NT::Primary)}});

	// UnaryOp -> + | - | * | !
	prods.push_back({NT::UnaryOp, {T(TokenType::Plus)}});
	prods.push_back({NT::UnaryOp, {T(TokenType::Minus)}});
	prods.push_back({NT::UnaryOp, {T(TokenType::Star)}});
	prods.push_back({NT::UnaryOp, {T(TokenType::Not)}});

	// Primary -> intlit | charlit | doublelit | ident PrimaryAfterId | '(' Expr ')'
	prods.push_back({NT::Primary, {T(TokenType::IntLiterial)}});
	prods.push_back({NT::Primary, {T(TokenType::CharLiterial)}});
	prods.push_back({NT::Primary, {T(TokenType::DoubleLiterial)}});
	prods.push_back({NT::Primary, {T(TokenType::Identifier), N(NT::PrimaryAfterId)}});
	prods.push_back({NT::Primary, {T(TokenType::LParen), N(NT::Expr), T(TokenType::RParen)}});

	// PrimaryAfterId -> '(' ArgListOpt ')' | eps
	prods.push_back({NT::PrimaryAfterId, {T(TokenType::LParen), N(NT::ArgListOpt), T(TokenType::RParen)}});
	prods.push_back({NT::PrimaryAfterId, {}});

	// ArgListOpt -> Expr ArgListTail | eps
	prods.push_back({NT::ArgListOpt, {N(NT::Expr), N(NT::ArgListTail)}});
	prods.push_back({NT::ArgListOpt, {}});

	// ArgListTail -> ',' Expr ArgListTail | eps
	prods.push_back({NT::ArgListTail, {T(TokenType::Comma), N(NT::Expr), N(NT::ArgListTail)}});
	prods.push_back({NT::ArgListTail, {}});
}

void LL1TableParser::buildIndexes() {
	prodsByLhs.clear();
	for (int i = 0; i < static_cast<int>(prods.size()); i++) {
		prodsByLhs[nti(prods[i].lhs)].push_back(i);
	}
}

LL1TableParser::FirstSet LL1TableParser::firstOfSequence(const std::vector<Sym>& seq, size_t startIndex) const {
	FirstSet out;
	out.hasEps = true;
	for (size_t i = startIndex; i < seq.size(); i++) {
		const auto& s = seq[i];
		FirstSet fs;
		if (s.isTerminal) {
			fs.terms.insert(s.term);
			fs.hasEps = false;
		} else {
			auto it = first.find(nti(s.nonterm));
			if (it != first.end()) {
				fs = it->second;
			}
		}

		for (auto t : fs.terms) {
			out.terms.insert(t);
		}
		if (!fs.hasEps) {
			out.hasEps = false;
			return out;
		}
	}
	// all epsilon
	out.hasEps = true;
	return out;
}

void LL1TableParser::computeFirst() {
	first.clear();
	// init nonterminals
	for (const auto& p : prods) {
		first.emplace(nti(p.lhs), FirstSet{});
	}

	bool changed = true;
	while (changed) {
		changed = false;
		for (const auto& p : prods) {
			auto& fa = first[nti(p.lhs)];
			if (p.rhs.empty()) {
				if (!fa.hasEps) {
					fa.hasEps = true;
					changed = true;
				}
				continue;
			}

			bool allEps = true;
			for (const auto& s : p.rhs) {
				if (s.isTerminal) {
					if (fa.terms.insert(s.term).second) changed = true;
					allEps = false;
					break;
				}

				auto fs = first[nti(s.nonterm)];
				for (auto t : fs.terms) {
					if (fa.terms.insert(t).second) changed = true;
				}
				if (!fs.hasEps) {
					allEps = false;
					break;
				}
			}
			if (allEps && !fa.hasEps) {
				fa.hasEps = true;
				changed = true;
			}
		}
	}
}

void LL1TableParser::computeFollow() {
	follow.clear();
	for (const auto& p : prods) {
		follow.emplace(nti(p.lhs), std::set<TokenType>{});
	}
	// start symbol
	follow[nti(NT::Program)].insert(TokenType::Eof);

	bool changed = true;
	while (changed) {
		changed = false;
		for (const auto& p : prods) {
			for (size_t i = 0; i < p.rhs.size(); i++) {
				const auto& symB = p.rhs[i];
				if (symB.isTerminal) continue;

				// beta is rhs[i+1..]
				FirstSet fb = firstOfSequence(p.rhs, i + 1);
				auto& followB = follow[nti(symB.nonterm)];

				for (auto t : fb.terms) {
					if (followB.insert(t).second) changed = true;
				}

				if (fb.hasEps) {
					for (auto t : follow[nti(p.lhs)]) {
						if (followB.insert(t).second) changed = true;
					}
				}
			}
		}
	}
}

void LL1TableParser::buildParseTableOrThrow() {
	table.clear();
	std::ostringstream conflicts;
	bool hasHardConflict = false;

	auto isEpsilonProd = [&](int prodIndex) -> bool {
		return prodIndex >= 0 && prodIndex < static_cast<int>(prods.size()) && prods[prodIndex].rhs.empty();
	};

	for (int pi = 0; pi < static_cast<int>(prods.size()); pi++) {
		const auto& p = prods[pi];
		FirstSet f = firstOfSequence(p.rhs, 0);

		for (auto a : f.terms) {
			auto& row = table[nti(p.lhs)];
			auto it = row.find(a);
			if (it == row.end()) {
				row[a] = pi;
				continue;
			}
			if (it->second == pi) continue;

			// Resolve common dangling-else style conflicts by preferring non-epsilon productions.
			const bool oldEps = isEpsilonProd(it->second);
			const bool newEps = isEpsilonProd(pi);
			if (oldEps != newEps) {
				row[a] = newEps ? it->second : pi;
				continue;
			}

			hasHardConflict = true;
			conflicts << "Conflict M[" << ntToString(p.lhs) << "," << tokenTypeShort(a) << "] between "
					  << productionToString(it->second) << " and " << productionToString(pi) << "\n";
		}

		if (f.hasEps) {
			for (auto b : follow[nti(p.lhs)]) {
				auto& row = table[nti(p.lhs)];
				auto it = row.find(b);
				if (it == row.end()) {
					row[b] = pi;
					continue;
				}
				if (it->second == pi) continue;

				const bool oldEps = isEpsilonProd(it->second);
				const bool newEps = isEpsilonProd(pi);
				if (oldEps != newEps) {
					row[b] = newEps ? it->second : pi;
					continue;
				}

				hasHardConflict = true;
				conflicts << "Conflict M[" << ntToString(p.lhs) << "," << tokenTypeShort(b) << "] between "
							  << productionToString(it->second) << " and " << productionToString(pi) << "\n";
			}
		}
	}

	if (hasHardConflict) {
		throw std::runtime_error("LL1TableParser: grammar has unresolved conflicts\n" + conflicts.str());
	}
}

LL1TableParser::Result LL1TableParser::parseAndTrace(const std::vector<Token>& tokens) const {
	Result r;
	std::ostringstream out;

	if (tokens.empty()) {
		r.success = false;
		r.error = "LL1TableParser: empty token stream";
		return r;
	}

	size_t pos = 0;
	std::vector<Sym> st;
	st.push_back(T(TokenType::Eof));
	st.push_back(N(NT::Program));

	out << "Step\tStack\tInput\tAction\n";
	out << "----\t-----\t-----\t------\n";

	for (int step = 1; step <= 200000; step++) {
		if (st.empty()) break;

		TokenType lookahead = tokens[pos].type;
		const Sym X = st.back();

		out << step << "\t" << stackToString(st) << "\t" << inputPreview(tokens, pos) << "\t";

		if (X.isTerminal) {
			if (X.term == lookahead) {
				out << "match " << tokenTypeShort(lookahead) << "\n";
				st.pop_back();
				if (lookahead != TokenType::Eof) pos++;
				else {
					// accept when stack ends after matching Eof
					if (st.empty()) {
						r.success = true;
						r.trace = out.str();
						return r;
					}
					pos++;
				}
				continue;
			}

			std::ostringstream err;
			err << "LL1TableParser: expected " << tokenTypeShort(X.term)
				<< " but got " << tokenShort(tokens[pos])
				<< " at line " << tokens[pos].line << ", column " << tokens[pos].column;
			out << "ERROR\n";
			r.success = false;
			r.trace = out.str();
			r.error = err.str();
			return r;
		}

		// nonterminal
		auto rowIt = table.find(nti(X.nonterm));
		int prodIndex = -1;
		if (rowIt != table.end()) {
			auto cellIt = rowIt->second.find(lookahead);
			if (cellIt != rowIt->second.end()) {
				prodIndex = cellIt->second;
			}
		}

		if (prodIndex < 0) {
			std::ostringstream err;
			err << "LL1TableParser: no rule for " << ntToString(X.nonterm)
				<< " on lookahead " << tokenShort(tokens[pos])
				<< " at line " << tokens[pos].line << ", column " << tokens[pos].column;

			// show expected terminals (row keys)
			if (rowIt != table.end()) {
				err << " (expected one of: ";
				bool firstOne = true;
				for (const auto& kv : rowIt->second) {
					if (!firstOne) err << ", ";
					firstOne = false;
					err << tokenTypeShort(kv.first);
				}
				err << ")";
			}

			out << "ERROR\n";
			r.success = false;
			r.trace = out.str();
			r.error = err.str();
			return r;
		}

		out << productionToString(prodIndex) << "\n";

		// apply production
		st.pop_back();
		const auto& rhs = prods[prodIndex].rhs;
		for (auto it = rhs.rbegin(); it != rhs.rend(); ++it) {
			st.push_back(*it);
		}
	}

	r.success = false;
	r.trace = out.str();
	r.error = "LL1TableParser: exceeded step limit";
	return r;
}

std::string LL1TableParser::ntToString(NT nt) {
	switch (nt) {
		case NT::Program: return "Program";
		case NT::DeclList: return "DeclList";
		case NT::Decl: return "Decl";
		case NT::TypeSpec: return "TypeSpec";
		case NT::DeclAfterId: return "DeclAfterId";
		case NT::ParamClause: return "ParamClause";
		case NT::ParamList: return "ParamList";
		case NT::ParamListTail: return "ParamListTail";
		case NT::Param: return "Param";
		case NT::ParamTypeSpec: return "ParamTypeSpec";
		case NT::CompoundStmt: return "CompoundStmt";
		case NT::LocalDecls: return "LocalDecls";
		case NT::LocalDecl: return "LocalDecl";
		case NT::LocalInitOpt: return "LocalInitOpt";
		case NT::StmtList: return "StmtList";
		case NT::Stmt: return "Stmt";
		case NT::IfStmt: return "IfStmt";
		case NT::ElseOpt: return "ElseOpt";
		case NT::WhileStmt: return "WhileStmt";
		case NT::ForStmt: return "ForStmt";
		case NT::ExprOpt: return "ExprOpt";
		case NT::ReturnStmt: return "ReturnStmt";
		case NT::ReturnExprOpt: return "ReturnExprOpt";
		case NT::ExprStmt: return "ExprStmt";
		case NT::Expr: return "Expr";
		case NT::Assignment: return "Assignment";
		case NT::AssignmentTail: return "AssignmentTail";
		case NT::LogicalOr: return "LogicalOr";
		case NT::LogicalOrTail: return "LogicalOrTail";
		case NT::LogicalAnd: return "LogicalAnd";
		case NT::LogicalAndTail: return "LogicalAndTail";
		case NT::Equality: return "Equality";
		case NT::EqualityTail: return "EqualityTail";
		case NT::Relational: return "Relational";
		case NT::RelationalTail: return "RelationalTail";
		case NT::Additive: return "Additive";
		case NT::AdditiveTail: return "AdditiveTail";
		case NT::Multiplicative: return "Multiplicative";
		case NT::MultiplicativeTail: return "MultiplicativeTail";
		case NT::Unary: return "Unary";
		case NT::UnaryOp: return "UnaryOp";
		case NT::Primary: return "Primary";
		case NT::PrimaryAfterId: return "PrimaryAfterId";
		case NT::ArgListOpt: return "ArgListOpt";
		case NT::ArgListTail: return "ArgListTail";
		default: return "<NT?>";
	}
}

std::string LL1TableParser::symToString(const Sym& s) {
	return s.isTerminal ? tokenTypeShort(s.term) : ntToString(s.nonterm);
}

std::string LL1TableParser::productionToString(int prodIndex) const {
	const auto& p = prods[prodIndex];
	std::ostringstream ss;
	ss << ntToString(p.lhs) << " -> ";
	if (p.rhs.empty()) {
		ss << "¦Å";
		return ss.str();
	}
	for (size_t i = 0; i < p.rhs.size(); i++) {
		if (i) ss << " ";
		ss << symToString(p.rhs[i]);
	}
	return ss.str();
}

std::string LL1TableParser::tokenTypeShort(TokenType t) {
	return tokenTypeToString(t);
}

std::string LL1TableParser::tokenShort(const Token& tok) {
	std::ostringstream ss;
	ss << tokenTypeToString(tok.type);
	if (!tok.lexeme.empty()) {
		ss << "(" << tok.lexeme << ")";
	}
	return ss.str();
}

std::string LL1TableParser::stackToString(const std::vector<Sym>& st) {
	std::ostringstream ss;
	ss << "[";
	// print top->bottom for readability
	for (size_t i = 0; i < st.size(); i++) {
		const auto& s = st[st.size() - 1 - i];
		if (i) ss << " ";
		ss << symToString(s);
		if (i >= 12 && st.size() > 16) {
			ss << " ...";
			break;
		}
	}
	ss << "]";
	return ss.str();
}

std::string LL1TableParser::inputPreview(const std::vector<Token>& tokens, size_t pos, size_t maxCount) {
	std::ostringstream ss;
	ss << "[";
	for (size_t i = 0; i < maxCount && pos + i < tokens.size(); i++) {
		if (i) ss << " ";
		ss << tokenShort(tokens[pos + i]);
	}
	if (pos + maxCount < tokens.size()) ss << " ...";
	ss << "]";
	return ss.str();
}
