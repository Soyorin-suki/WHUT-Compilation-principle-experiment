#include "TripleGenerator.hpp"

#include <stdexcept>

void TripleGenerator::reset() {
	triples.clear();
	labelCounter = 0;
}

int TripleGenerator::emitTriple(const std::string& op, const std::string& arg1, const std::string& arg2) {
	triples.push_back(Triple{op, arg1, arg2});
	return static_cast<int>(triples.size()); // 1-based index
}

std::string TripleGenerator::ref(int index) const {
	return "(" + std::to_string(index) + ")";
}

std::string TripleGenerator::newLabel() {
	return "L" + std::to_string(++labelCounter);
}

std::string TripleGenerator::generate(const Program& program) {
	reset();
	for (const auto& d : program.decls) {
		if (!d) continue;
		genDecl(*d);
	}
	return format();
}

void TripleGenerator::genDecl(const Decl& decl) {
	if (auto v = dynamic_cast<const VarDecl*>(&decl)) {
		genVarDecl(*v);
		return;
	}
	if (auto f = dynamic_cast<const FunDecl*>(&decl)) {
		genFunDecl(*f);
		return;
	}
}

void TripleGenerator::genVarDecl(const VarDecl& decl) {
	// 仅输出带初始化的全局变量初始化
	if (!decl.init) return;
	std::string rhs = genExpr(*decl.init);
	emitTriple(":=", rhs, decl.name);
}

void TripleGenerator::genFunDecl(const FunDecl& decl) {
	emitTriple("func", decl.name, "");
	if (decl.body) {
		genCompound(*decl.body);
	}
	emitTriple("end", decl.name, "");
	emitTriple("note", "", ""); // blank line separator
}

void TripleGenerator::genStmt(const Stmt& stmt) {
	if (auto c = dynamic_cast<const CompoundStmt*>(&stmt)) {
		genCompound(*c);
		return;
	}
	if (auto i = dynamic_cast<const IfStmt*>(&stmt)) {
		genIf(*i);
		return;
	}
	if (auto r = dynamic_cast<const ReturnStmt*>(&stmt)) {
		genReturn(*r);
		return;
	}
	if (auto e = dynamic_cast<const ExprStmt*>(&stmt)) {
		genExprStmt(*e);
		return;
	}
	if (dynamic_cast<const WhileStmt*>(&stmt)) {
		emitTriple("note", "while: not implemented", "");
		return;
	}
	if (dynamic_cast<const ForStmt*>(&stmt)) {
		emitTriple("note", "for: not implemented", "");
		return;
	}
	emitTriple("note", "stmt: not implemented", "");
}

void TripleGenerator::genCompound(const CompoundStmt& stmt) {
	for (const auto& v : stmt.localVars) {
		if (!v) continue;
		if (v->init) {
			std::string rhs = genExpr(*v->init);
			emitTriple(":=", rhs, v->name);
		}
	}
	for (const auto& s : stmt.stmts) {
		if (!s) continue;
		genStmt(*s);
	}
}

void TripleGenerator::genIf(const IfStmt& stmt) {
	std::string lElseOrEnd = newLabel();
	std::string lEnd = newLabel();

	std::string cond = stmt.cond ? genExpr(*stmt.cond) : "0";
	// jz cond, L  : cond == 0 跳转
	emitTriple("jz", cond, lElseOrEnd);

	if (stmt.thenBranch) {
		genStmt(*stmt.thenBranch);
	}

	if (stmt.elseBranch) {
		emitTriple("jmp", lEnd, "");
		emitTriple("label", lElseOrEnd, "");
		genStmt(*stmt.elseBranch);
		emitTriple("label", lEnd, "");
	} else {
		emitTriple("label", lElseOrEnd, "");
	}
}

void TripleGenerator::genReturn(const ReturnStmt& stmt) {
	if (!stmt.expr) {
		emitTriple("ret", "", "");
		return;
	}
	std::string v = genExpr(*stmt.expr);
	emitTriple("ret", v, "");
}

void TripleGenerator::genExprStmt(const ExprStmt& stmt) {
	if (!stmt.expr) return;
	if (auto call = dynamic_cast<const CallExpr*>(stmt.expr.get())) {
		genCallExprStmt(*call);
		return;
	}
	(void)genExpr(*stmt.expr);
}

std::string TripleGenerator::genExpr(const Expr& expr) {
	if (auto a = dynamic_cast<const AssignExpr*>(&expr)) return genAssignExpr(*a);
	if (auto b = dynamic_cast<const BinaryExpr*>(&expr)) return genBinaryExpr(*b);
	if (auto u = dynamic_cast<const UnaryExpr*>(&expr)) return genUnaryExpr(*u);
	if (auto c = dynamic_cast<const CallExpr*>(&expr)) return genCallExprValue(*c);
	if (auto v = dynamic_cast<const ValExpr*>(&expr)) return genValExpr(*v);
	if (auto i = dynamic_cast<const IntLiteral*>(&expr)) return genIntLiteral(*i);
	if (auto ch = dynamic_cast<const CharLiteral*>(&expr)) return genCharLiteral(*ch);
	if (auto d = dynamic_cast<const DoubleLiteral*>(&expr)) return genDoubleLiteral(*d);
	return "0";
}

std::string TripleGenerator::genAssignExpr(const AssignExpr& expr) {
	auto lhsVar = dynamic_cast<const ValExpr*>(expr.left.get());
	if (!lhsVar) {
		throw std::runtime_error("TripleGen: assignment lhs must be identifier");
	}
	std::string rhs = expr.right ? genExpr(*expr.right) : "0";
	emitTriple(":=", rhs, lhsVar->name);
	return lhsVar->name;
}

std::string TripleGenerator::genBinaryExpr(const BinaryExpr& expr) {
	std::string a = expr.left ? genExpr(*expr.left) : "0";
	std::string b = expr.right ? genExpr(*expr.right) : "0";
	int idx = emitTriple(expr.op, a, b);
	return ref(idx);
}

std::string TripleGenerator::genUnaryExpr(const UnaryExpr& expr) {
	std::string a = expr.operand ? genExpr(*expr.operand) : "0";
	int idx = emitTriple(expr.op, a, "");
	return ref(idx);
}

void TripleGenerator::genCallExprStmt(const CallExpr& expr) {
	for (const auto& arg : expr.args) {
		if (!arg) continue;
		std::string v = genExpr(*arg);
		emitTriple("param", v, "");
	}
	emitTriple("call", expr.name, std::to_string(expr.args.size()));
}

std::string TripleGenerator::genCallExprValue(const CallExpr& expr) {
	for (const auto& arg : expr.args) {
		if (!arg) continue;
		std::string v = genExpr(*arg);
		emitTriple("param", v, "");
	}
	int idx = emitTriple("call", expr.name, std::to_string(expr.args.size()));
	return ref(idx);
}

std::string TripleGenerator::genValExpr(const ValExpr& expr) {
	return expr.name;
}

std::string TripleGenerator::genIntLiteral(const IntLiteral& expr) {
	return expr.lexeme;
}

std::string TripleGenerator::genCharLiteral(const CharLiteral& expr) {
	return expr.lexeme;
}

std::string TripleGenerator::genDoubleLiteral(const DoubleLiteral& expr) {
	return expr.lexeme;
}

std::string TripleGenerator::format() const {
	std::ostringstream oss;
	for (size_t i = 0; i < triples.size(); ++i) {
		const auto& t = triples[i];
		// note with empty args used as blank line
		if (t.op == "note" && t.arg1.empty() && t.arg2.empty()) {
			oss << "\n";
			continue;
		}
		oss << (i + 1) << ": (" << t.op << ", " << t.arg1 << ", " << t.arg2 << ")\n";
	}
	return oss.str();
}
