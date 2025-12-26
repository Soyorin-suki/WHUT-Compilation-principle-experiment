#include "TACGenerator.hpp"

#include <stdexcept>

void TACGenerator::reset() {
	tempCounter = 0;
	labelCounter = 0;
	out.str(std::string());
	out.clear();
}

std::string TACGenerator::newTemp() {
	return "t" + std::to_string(++tempCounter);
}

std::string TACGenerator::newLabel() {
	return "L" + std::to_string(++labelCounter);
}

void TACGenerator::emit(const std::string& line) {
	out << line << "\n";
}

void TACGenerator::emitLabel(const std::string& label) {
	out << label << ":\n";
}

std::string TACGenerator::generate(const Program& program) {
	reset();
	for (const auto& d : program.decls) {
		if (!d) continue;
		genDecl(*d);
	}
	return out.str();
}

void TACGenerator::genDecl(const Decl& decl) {
	if (auto v = dynamic_cast<const VarDecl*>(&decl)) {
		genVarDecl(*v);
		return;
	}
	if (auto f = dynamic_cast<const FunDecl*>(&decl)) {
		genFunDecl(*f);
		return;
	}
}

void TACGenerator::genVarDecl(const VarDecl& decl) {
	// 只输出带初始化的全局变量初始化
	if (!decl.init) return;
	std::string rhs = genExpr(*decl.init);
	emit(decl.name + " = " + rhs);
}

void TACGenerator::genFunDecl(const FunDecl& decl) {
	emit("func " + decl.name);
	if (decl.body) {
		genCompound(*decl.body);
	}
	emit("end func " + decl.name);
	emit(std::string());
}

void TACGenerator::genStmt(const Stmt& stmt) {
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
		emit("# while: not implemented");
		return;
	}
	if (dynamic_cast<const ForStmt*>(&stmt)) {
		emit("# for: not implemented");
		return;
	}

	emit("# stmt: not implemented");
}

void TACGenerator::genCompound(const CompoundStmt& stmt) {
	// locals
	for (const auto& v : stmt.localVars) {
		if (!v) continue;
		if (v->init) {
			std::string rhs = genExpr(*v->init);
			emit(v->name + " = " + rhs);
		}
	}
	// statements
	for (const auto& s : stmt.stmts) {
		if (!s) continue;
		genStmt(*s);
	}
}

void TACGenerator::genIf(const IfStmt& stmt) {
	std::string lThen = newLabel();
	std::string lElse = newLabel();
	std::string lEnd = newLabel();

	std::string cond = stmt.cond ? genExpr(*stmt.cond) : "0";
	emit("if " + cond + " != 0 goto " + lThen);
	if (stmt.elseBranch) {
		emit("goto " + lElse);
	} else {
		emit("goto " + lEnd);
	}

	emitLabel(lThen);
	if (stmt.thenBranch) {
		genStmt(*stmt.thenBranch);
	}
	if (stmt.elseBranch) {
		emit("goto " + lEnd);
		emitLabel(lElse);
		genStmt(*stmt.elseBranch);
	}
	
	emitLabel(lEnd);
}

void TACGenerator::genReturn(const ReturnStmt& stmt) {
	if (!stmt.expr) {
		emit("return");
		return;
	}
	std::string v = genExpr(*stmt.expr);
	emit("return " + v);
}

void TACGenerator::genExprStmt(const ExprStmt& stmt) {
	if (!stmt.expr) return;
	if (auto call = dynamic_cast<const CallExpr*>(stmt.expr.get())) {
		genCallExprStmt(*call);
		return;
	}
	(void)genExpr(*stmt.expr);
}

std::string TACGenerator::genExpr(const Expr& expr) {
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

std::string TACGenerator::genAssignExpr(const AssignExpr& expr) {
	auto lhsVar = dynamic_cast<const ValExpr*>(expr.left.get());
	if (!lhsVar) {
		// 语义分析阶段已经限制，这里再做兜底
		throw std::runtime_error("TACGen: assignment lhs must be identifier");
	}
	std::string rhs = expr.right ? genExpr(*expr.right) : "0";
	emit(lhsVar->name + " = " + rhs);
	return lhsVar->name;
}

std::string TACGenerator::genBinaryExpr(const BinaryExpr& expr) {
	std::string a = expr.left ? genExpr(*expr.left) : "0";
	std::string b = expr.right ? genExpr(*expr.right) : "0";
	std::string t = newTemp();
	emit(t + " = " + a + " " + expr.op + " " + b);
	return t;
}

std::string TACGenerator::genUnaryExpr(const UnaryExpr& expr) {
	std::string a = expr.operand ? genExpr(*expr.operand) : "0";
	std::string t = newTemp();
	emit(t + " = " + expr.op + " " + a);
	return t;
}

void TACGenerator::genCallExprStmt(const CallExpr& expr) {
	for (const auto& arg : expr.args) {
		if (!arg) continue;
		std::string v = genExpr(*arg);
		emit("param " + v);
	}
	emit("call " + expr.name + ", " + std::to_string(expr.args.size()));
}

std::string TACGenerator::genCallExprValue(const CallExpr& expr) {
	for (const auto& arg : expr.args) {
		if (!arg) continue;
		std::string v = genExpr(*arg);
		emit("param " + v);
	}
	std::string t = newTemp();
	emit(t + " = call " + expr.name + ", " + std::to_string(expr.args.size()));
	return t;
}

std::string TACGenerator::genValExpr(const ValExpr& expr) {
	return expr.name;
}

std::string TACGenerator::genIntLiteral(const IntLiteral& expr) {
	return expr.lexeme;
}

std::string TACGenerator::genCharLiteral(const CharLiteral& expr) {
	return expr.lexeme;
}

std::string TACGenerator::genDoubleLiteral(const DoubleLiteral& expr) {
	return expr.lexeme;
}
