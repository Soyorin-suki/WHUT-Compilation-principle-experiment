
#include "SemanticAnalyzer.hpp"

#include <sstream>

static bool isRelOp(const std::string& op) {
	return op == "<" || op == ">" || op == "<=" || op == ">=";
}

static bool isEqOp(const std::string& op) {
	return op == "==" || op == "!=";
}

static bool isLogicalOp(const std::string& op) {
	return op == "&&" || op == "||";
}

static bool isArithOp(const std::string& op) {
	return op == "+" || op == "-" || op == "*" || op == "/" || op == "%";
}

void SemanticAnalyzer::analyze(const Program& program) {
	symbols.reset();
	currentFunctionReturnType.reset();

	// pass 1: declare all global function signatures (so calls before definition work)
	for (const auto& declPtr : program.decls) {
		if (!declPtr) {
			continue;
		}
		if (auto fun = dynamic_cast<const FunDecl*>(declPtr.get())) {
			analyzeFunDeclSignature(*fun);
		}
	}

	// pass 2: analyze all decls (vars + function bodies)
	for (const auto& declPtr : program.decls) {
		if (!declPtr) {
			continue;
		}
		analyzeDecl(*declPtr);
	}
}

void SemanticAnalyzer::analyzeDecl(const Decl& decl) {
	if (auto varDecl = dynamic_cast<const VarDecl*>(&decl)) {
		analyzeVarDecl(*varDecl, true);
		return;
	}
	if (auto funDecl = dynamic_cast<const FunDecl*>(&decl)) {
		// signature already declared in pass 1
		analyzeFunDeclBody(*funDecl);
		return;
	}
	// unknown decl type
}

void SemanticAnalyzer::analyzeVarDecl(const VarDecl& decl, bool /*isGlobal*/) {
	Symbol sym;
	sym.kind = SymbolKind::Var;
	sym.type = decl.type;
	sym.declLine = decl.line;
	sym.declCol = decl.col;

	if (!symbols.declare(decl.name, sym)) {
		errorAt(decl, "重复声明标识符: " + decl.name);
	}

	if (decl.init) {
		Type initType = analyzeExpr(*decl.init);
		if (!canWiden(initType, decl.type)) {
			std::ostringstream oss;
			oss << "初始化类型不兼容: 变量 '" << decl.name << "' 为 " << typeName(decl.type)
			    << ", 但初始化表达式为 " << typeName(initType);
			errorAt(decl, oss.str());
		}
	}
}

void SemanticAnalyzer::analyzeFunDeclSignature(const FunDecl& decl) {
	Symbol sym;
	sym.kind = SymbolKind::Func;
	sym.type = decl.returnType;
	sym.func.returnType = decl.returnType;
	sym.declLine = decl.line;
	sym.declCol = decl.col;
	for (const auto& p : decl.params) {
		if (!p) {
			continue;
		}
		sym.func.paramTypes.push_back(p->type);
	}

	if (!symbols.declare(decl.name, sym)) {
		errorAt(decl, "重复声明函数: " + decl.name);
	}
}

void SemanticAnalyzer::analyzeFunDeclBody(const FunDecl& decl) {
	// enter function scope
	symbols.enterScope();
	currentFunctionReturnType = decl.returnType;

	// declare params
	for (const auto& p : decl.params) {
		if (!p) {
			continue;
		}
		Symbol sym;
		sym.kind = SymbolKind::Param;
		sym.type = p->type;
		sym.declLine = p->line;
		sym.declCol = p->col;
		if (!symbols.declare(p->name, sym)) {
			errorAt(*p, "重复声明形参: " + p->name);
		}
	}

	if (decl.body) {
		analyzeCompound(*decl.body);
	}

	currentFunctionReturnType.reset();
	symbols.leaveScope();
}

void SemanticAnalyzer::analyzeStmt(const Stmt& stmt) {
	if (auto compound = dynamic_cast<const CompoundStmt*>(&stmt)) {
		analyzeCompound(*compound);
		return;
	}
	if (auto ifStmt = dynamic_cast<const IfStmt*>(&stmt)) {
		analyzeIf(*ifStmt);
		return;
	}
	if (auto whileStmt = dynamic_cast<const WhileStmt*>(&stmt)) {
		analyzeWhile(*whileStmt);
		return;
	}
	if (auto forStmt = dynamic_cast<const ForStmt*>(&stmt)) {
		analyzeFor(*forStmt);
		return;
	}
	if (auto retStmt = dynamic_cast<const ReturnStmt*>(&stmt)) {
		analyzeReturn(*retStmt);
		return;
	}
	if (auto exprStmt = dynamic_cast<const ExprStmt*>(&stmt)) {
		analyzeExprStmt(*exprStmt);
		return;
	}
	// unknown stmt type
}

void SemanticAnalyzer::analyzeCompound(const CompoundStmt& stmt) {
	symbols.enterScope();

	for (const auto& v : stmt.localVars) {
		if (!v) {
			continue;
		}
		analyzeVarDecl(*v, false);
	}
	for (const auto& s : stmt.stmts) {
		if (!s) {
			continue;
		}
		analyzeStmt(*s);
	}

	symbols.leaveScope();
}

void SemanticAnalyzer::analyzeIf(const IfStmt& stmt) {
	if (!stmt.cond) {
		errorAt(stmt, "if 条件缺失");
	}
	Type condType = analyzeExpr(*stmt.cond);
	if (condType == Type::VOID) {
		errorAt(stmt, "if 条件不能为 void");
	}
	if (stmt.thenBranch) {
		analyzeStmt(*stmt.thenBranch);
	}
	if (stmt.elseBranch) {
		analyzeStmt(*stmt.elseBranch);
	}
}

void SemanticAnalyzer::analyzeWhile(const WhileStmt& stmt) {
	if (!stmt.cond) {
		errorAt(stmt, "while 条件缺失");
	}
	Type condType = analyzeExpr(*stmt.cond);
	if (condType == Type::VOID) {
		errorAt(stmt, "while 条件不能为 void");
	}
	if (stmt.body) {
		analyzeStmt(*stmt.body);
	}
}

void SemanticAnalyzer::analyzeFor(const ForStmt& stmt) {
	// for 在你实现里 init/cond/update 都是 expr_opt，因此这里只做表达式检查
	if (stmt.init) {
		(void)analyzeExpr(*stmt.init);
	}
	if (stmt.cond) {
		Type condType = analyzeExpr(*stmt.cond);
		if (condType == Type::VOID) {
			errorAt(stmt, "for 条件不能为 void");
		}
	}
	if (stmt.update) {
		(void)analyzeExpr(*stmt.update);
	}
	if (stmt.body) {
		analyzeStmt(*stmt.body);
	}
}

void SemanticAnalyzer::analyzeReturn(const ReturnStmt& stmt) {
	if (!currentFunctionReturnType.has_value()) {
		errorAt(stmt, "return 只能出现在函数体内");
	}
	Type expected = *currentFunctionReturnType;
	if (expected == Type::VOID) {
		if (stmt.expr) {
			errorAt(stmt, "void 函数不应返回值");
		}
		return;
	}
	if (!stmt.expr) {
		errorAt(stmt, "非 void 函数必须返回值");
	}
	Type actual = analyzeExpr(*stmt.expr);
	if (!canWiden(actual, expected)) {
		std::ostringstream oss;
		oss << "return 类型不匹配: 期望 " << typeName(expected) << ", 实际 " << typeName(actual);
		errorAt(stmt, oss.str());
	}
}

void SemanticAnalyzer::analyzeExprStmt(const ExprStmt& stmt) {
	if (!stmt.expr) {
		// 你当前语法不支持空语句，这里留作健壮性
		return;
	}
	(void)analyzeExpr(*stmt.expr);
}

Type SemanticAnalyzer::analyzeExpr(const Expr& expr) {
	if (auto a = dynamic_cast<const AssignExpr*>(&expr)) return analyzeAssignExpr(*a);
	if (auto b = dynamic_cast<const BinaryExpr*>(&expr)) return analyzeBinaryExpr(*b);
	if (auto u = dynamic_cast<const UnaryExpr*>(&expr)) return analyzeUnaryExpr(*u);
	if (auto c = dynamic_cast<const CallExpr*>(&expr)) return analyzeCallExpr(*c);
	if (auto v = dynamic_cast<const ValExpr*>(&expr)) return analyzeValExpr(*v);
	if (auto i = dynamic_cast<const IntLiteral*>(&expr)) return analyzeIntLiteral(*i);
	if (auto ch = dynamic_cast<const CharLiteral*>(&expr)) return analyzeCharLiteral(*ch);
	if (auto d = dynamic_cast<const DoubleLiteral*>(&expr)) return analyzeDoubleLiteral(*d);
	// unknown expr
	return Type::VOID;
}

Type SemanticAnalyzer::analyzeAssignExpr(const AssignExpr& expr) {
	if (!expr.left || !expr.right) {
		errorAt(expr, "赋值表达式不完整");
	}
	// 左值：先按最小要求，只允许变量名作为左值（后续可扩展 *p 等）
	auto lhsVar = dynamic_cast<const ValExpr*>(expr.left.get());
	if (!lhsVar) {
		errorAt(expr, "赋值左侧必须是变量标识符（当前不支持复杂左值）");
	}
	Type lhsType = analyzeValExpr(*lhsVar);
	Type rhsType = analyzeExpr(*expr.right);
	if (!canWiden(rhsType, lhsType)) {
		std::ostringstream oss;
		oss << "赋值类型不兼容: 左侧 " << typeName(lhsType) << ", 右侧 " << typeName(rhsType);
		errorAt(expr, oss.str());
	}
	return lhsType;
}

Type SemanticAnalyzer::analyzeBinaryExpr(const BinaryExpr& expr) {
	if (!expr.left || !expr.right) {
		errorAt(expr, "二元表达式不完整");
	}
	Type lt = analyzeExpr(*expr.left);
	Type rt = analyzeExpr(*expr.right);
	const std::string& op = expr.op;

	if (isLogicalOp(op)) {
		if (lt == Type::VOID || rt == Type::VOID) {
			errorAt(expr, "逻辑运算两侧不能为 void");
		}
		return Type::INT;
	}
	if (isRelOp(op) || isEqOp(op)) {
		if (!isNumeric(lt) || !isNumeric(rt)) {
			errorAt(expr, "比较运算要求数值类型");
		}
		return Type::INT;
	}
	if (isArithOp(op)) {
		if (!isNumeric(lt) || !isNumeric(rt)) {
			errorAt(expr, "算术运算要求数值类型");
		}
		if (op == "%") {
			// 取模只允许整型
			Type ct = commonNumericType(lt, rt);
			if (ct == Type::DOUBLE) {
				errorAt(expr, "取模运算不支持 double");
			}
			return Type::INT;
		}
		return commonNumericType(lt, rt);
	}

	// unknown operator
	return Type::VOID;
}

Type SemanticAnalyzer::analyzeUnaryExpr(const UnaryExpr& expr) {
	if (!expr.operand) {
		errorAt(expr, "一元表达式缺少操作数");
	}
	Type ot = analyzeExpr(*expr.operand);
	const std::string& op = expr.op;
	if (op == "!") {
		if (ot == Type::VOID) {
			errorAt(expr, "! 运算不支持 void");
		}
		return Type::INT;
	}
	if (op == "+" || op == "-") {
		if (!isNumeric(ot)) {
			errorAt(expr, "一元 +/- 要求数值类型");
		}
		// char 提升为 int
		if (ot == Type::CHAR) return Type::INT;
		return ot;
	}
	if (op == "*") {
		// 你的 C 子集说明里该含义未定义，这里不做类型推导，直接透传
		return ot;
	}
	return Type::VOID;
}

Type SemanticAnalyzer::analyzeCallExpr(const CallExpr& expr) {
	const Symbol* sym = symbols.lookup(expr.name);
	if (!sym || sym->kind != SymbolKind::Func) {
		errorAt(expr, "未声明的函数: " + expr.name);
	}
	const auto& paramTypes = sym->func.paramTypes;
	if (expr.args.size() != paramTypes.size()) {
		std::ostringstream oss;
		oss << "函数 '" << expr.name << "' 参数个数不匹配: 期望 " << paramTypes.size() << ", 实际 "
		    << expr.args.size();
		errorAt(expr, oss.str());
	}
	for (size_t i = 0; i < expr.args.size(); ++i) {
		if (!expr.args[i]) {
			errorAt(expr, "函数参数为空");
		}
		Type actual = analyzeExpr(*expr.args[i]);
		Type expected = paramTypes[i];
		if (!canWiden(actual, expected)) {
			std::ostringstream oss;
			oss << "函数 '" << expr.name << "' 第 " << (i + 1)
			    << " 个参数类型不匹配: 期望 " << typeName(expected) << ", 实际 " << typeName(actual);
			errorAt(expr, oss.str());
		}
	}
	return sym->func.returnType;
}

Type SemanticAnalyzer::analyzeValExpr(const ValExpr& expr) {
	const Symbol* sym = symbols.lookup(expr.name);
	if (!sym) {
		errorAt(expr, "未声明的标识符: " + expr.name);
	}
	if (sym->kind == SymbolKind::Func) {
		errorAt(expr, "函数名不能当作变量使用: " + expr.name);
	}
	return sym->type;
}

Type SemanticAnalyzer::analyzeIntLiteral(const IntLiteral& /*expr*/) {
	return Type::INT;
}

Type SemanticAnalyzer::analyzeCharLiteral(const CharLiteral& /*expr*/) {
	return Type::CHAR;
}

Type SemanticAnalyzer::analyzeDoubleLiteral(const DoubleLiteral& /*expr*/) {
	return Type::DOUBLE;
}

bool SemanticAnalyzer::isNumeric(Type t) {
	return t == Type::CHAR || t == Type::INT || t == Type::DOUBLE;
}

int SemanticAnalyzer::numericRank(Type t) {
	switch (t) {
		case Type::CHAR:
			return 0;
		case Type::INT:
			return 1;
		case Type::DOUBLE:
			return 2;
		default:
			return -1;
	}
}

Type SemanticAnalyzer::commonNumericType(Type a, Type b) {
	int ra = numericRank(a);
	int rb = numericRank(b);
	int r = (ra > rb) ? ra : rb;
	if (r <= 0) return Type::INT; // char 与 char 运算也提升为 int
	if (r == 1) return Type::INT;
	return Type::DOUBLE;
}

bool SemanticAnalyzer::canWiden(Type from, Type to) {
	if (from == to) return true;
	if (from == Type::VOID || to == Type::VOID) return false;
	// numeric promotion: char -> int -> double
	if (from == Type::CHAR && (to == Type::INT || to == Type::DOUBLE)) return true;
	if (from == Type::INT && to == Type::DOUBLE) return true;
	return false;
}

std::string SemanticAnalyzer::typeName(Type t) {
	return typeToString(t);
}

[[noreturn]] void SemanticAnalyzer::errorAt(const ASTNode& node, const std::string& msg) const {
	std::ostringstream oss;
	if (node.line > 0) {
		oss << "语义错误(行 " << node.line << ", 列 " << node.col << "): ";
	} else {
		oss << "语义错误: ";
	}
	oss << msg;
	throw SemanticError(oss.str());
}
