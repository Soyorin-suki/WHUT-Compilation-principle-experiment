#include "Parser.hpp"
#include <iostream>



Parser::Parser() : pos(0) {}

void Parser::setTokens(const std::vector<Token>& toks) {
	tokens = toks;
	pos = 0;
}

// 辅助函数实现
Token& Parser::curToken() {
	if (pos >= tokens.size()) {
		throw std::out_of_range("Parser: curToken() position out of range");
	}
	return tokens[pos];
}

Token& Parser::peek(int offset) {
	if (pos + offset >= tokens.size()) {
		throw std::out_of_range("Parser: peek() position out of range :"+std::to_string(pos + offset));
	}
	return tokens[pos + offset];
}

void Parser::advance() {
	if (pos < tokens.size()) {
		pos++;
	}
}

bool Parser::check(TokenType t) {
	return curToken().type == t;
}

void Parser::expect(TokenType t) {
	if (!check(t)) {
		std::string msg = "Parser: expected token type ";
		msg += std::to_string(static_cast<int>(t));
		msg += " but got ";
		msg += std::to_string(static_cast<int>(curToken().type));
		msg += " at line " + std::to_string(curToken().line) + ", column " + std::to_string(curToken().column);
		throw std::runtime_error(msg);
	}
	advance();
}

// 构造函数
Parser::Parser(const std::vector<Token>& toks) : tokens(toks), pos(0) {}

// 解析入口
ProgramPtr Parser::parse() {
	auto prog = std::make_unique<Program>();
	prog->decls = std::vector<DeclPtr>();
	
	// program ::= decl program | EOF
	while (!check(TokenType::Eof)) {
		prog->decls.push_back(parseDecl());
	}
	expect(TokenType::Eof);
	return prog;
}

// 解析声明或定义
DeclPtr Parser::parseDecl() {
	// decl ::= val_decl | func_decl
	// 需要通过向前看判断是变量还是函数
	// 简单规则：var_decl = type IDENT (init)? ;
	//          func_decl = type IDENT ( ... ) { ... }
	
	// 先解析类型和标识符
	Type type;
	if (check(TokenType::kw_int)) {
		type = Type::INT;
		advance();
	} else if (check(TokenType::kw_char)) {
		type = Type::CHAR;
		advance();
	} else if (check(TokenType::kw_void)) {
		type = Type::VOID;
		advance();
	} else {
		throw std::runtime_error("Parser: expected type specifier at line"+std::to_string(curToken().line)+", column "+std::to_string(curToken().column));
	}

	if (!check(TokenType::Identifier)) {
		throw std::runtime_error("Parser: expected identifier at line "+std::to_string(curToken().line)+", column "+std::to_string(curToken().column));
	}
	std::string name = curToken().lexeme;
	advance();

	// 向前看判断是变量还是函数
	if (check(TokenType::Semicolon)) {
		// 变量声明（无初始化）：int x;
		auto varDecl = std::make_unique<VarDecl>();
		varDecl->type = type;
		varDecl->name = name;
		varDecl->init = nullptr;
		advance(); // 消费 ;
		return varDecl;
	} else if (check(TokenType::Assign)) {
		// 变量声明（有初始化）：int x = expr;
		auto varDecl = std::make_unique<VarDecl>();
		varDecl->type = type;
		varDecl->name = name;
		advance(); // 消费 =
		varDecl->init = parseExpr();
		expect(TokenType::Semicolon);
		return varDecl;
	} else if (check(TokenType::LParen)) {
		// 函数声明：type name ( ... ) { ... }
		return parseFunDecl(type, name);  // 传入已解析的 type 和 name
	} else {
		throw std::runtime_error("Parser: unexpected token after identifier at line "+std::to_string(curToken().line)+", column "+std::to_string(curToken().column));
	}
}

// 解析变量声明
DeclPtr Parser::parseVarDecl() {
	// val_decl ::= type_spec IDENT init_opt ";"
	// 此处假设类型和标识符已被解析
	auto varDecl = std::make_unique<VarDecl>();
	// 这会在 parseDecl 中处理
	return varDecl;
}


// 解析函数定义（接收已解析的返回类型和函数名）
DeclPtr Parser::parseFunDecl(Type returnType, const std::string &name) {
	// 当前 token 应该是 LParen
	expect(TokenType::LParen);
	auto funDecl = std::make_unique<FunDecl>();
	funDecl->returnType = returnType;
	funDecl->name = name;
	// 参数列表
	funDecl->params.clear();
	
	// 特殊处理：如果参数列表是 (void)，表示无参数
	if (check(TokenType::kw_void) && peek(1).type == TokenType::RParen) {
		advance(); // 跳过 void
	} else if (!check(TokenType::RParen)) {
		// 解析参数列表
		while (true) {
			Type ptype;
			if (check(TokenType::kw_int)) { ptype = Type::INT; advance(); }
			else if (check(TokenType::kw_char)) { ptype = Type::CHAR; advance(); }
			else if (check(TokenType::kw_void)) { ptype = Type::VOID; advance(); }
			else throw std::runtime_error("Parser: expected parameter type at line "+std::to_string(curToken().line)+", column "+std::to_string(curToken().column));

			if (!check(TokenType::Identifier)) throw std::runtime_error("Parser: expected parameter name at line "+std::to_string(curToken().line)+", column "+std::to_string(curToken().column));
			auto param = std::make_unique<Param>();
			param->type = ptype;
			param->name = curToken().lexeme;
			advance();
			funDecl->params.push_back(std::move(param));

			if (check(TokenType::Comma)) { advance(); continue; }
			break;
		}
	}
	expect(TokenType::RParen);
	// 函数体：复合语句
	funDecl->body = parseCompoundStmt();
	return funDecl;
}

// 解析语句
StmtPtr Parser::parseStmt() {
	// stmt ::= expr_stmt | compound_stmt | if_stmt | while_stmt | for_stmt | return_stmt | val_decl
	if (check(TokenType::LBrace)) {
		// 返回复合语句本身作为一个 Stmt
		return parseCompoundStmt();
	} else if (check(TokenType::kw_if)) {
		advance();
		expect(TokenType::LParen);
		auto ifStmt = std::make_unique<IfStmt>();
		ifStmt->cond = parseExpr();
		expect(TokenType::RParen);
		ifStmt->thenBranch = parseStmt();
		if (check(TokenType::kw_else)) { // 注意：还需在TokenType中添加kw_else
			advance();
			ifStmt->elseBranch = parseStmt();
		} else {
			ifStmt->elseBranch = nullptr;
		}
		return ifStmt;
	} else if (check(TokenType::kw_while)) {
		advance();
		expect(TokenType::LParen);
		auto whileStmt = std::make_unique<WhileStmt>();
		whileStmt->cond = parseExpr();
		expect(TokenType::RParen);
		whileStmt->body = parseStmt();
		return whileStmt;
	} else if (check(TokenType::kw_for)) {
		advance();
		expect(TokenType::LParen);
		auto forStmt = std::make_unique<ForStmt>();
		// for ( expr_opt ; expr_opt ; expr_opt ) stmt
		forStmt->init = check(TokenType::Semicolon) ? nullptr : parseExpr();
		expect(TokenType::Semicolon);
		forStmt->cond = check(TokenType::Semicolon) ? nullptr : parseExpr();
		expect(TokenType::Semicolon);
		forStmt->update = check(TokenType::RParen) ? nullptr : parseExpr();
		expect(TokenType::RParen);
		forStmt->body = parseStmt();
		return forStmt;
	} else if (check(TokenType::kw_return)) {
		advance();
		auto returnStmt = std::make_unique<ReturnStmt>();
		returnStmt->expr = check(TokenType::Semicolon) ? nullptr : parseExpr();
		expect(TokenType::Semicolon);
		return returnStmt;
	} else {
		// 表达式语句
		auto exprStmt = std::make_unique<ExprStmt>();
		exprStmt->expr = parseExpr();
		expect(TokenType::Semicolon);
		return exprStmt;
	}
}

// 解析复合语句
std::unique_ptr<CompoundStmt> Parser::parseCompoundStmt() {
	expect(TokenType::LBrace);
	auto compound = std::make_unique<CompoundStmt>();
	compound->localVars = std::vector<std::unique_ptr<VarDecl>>();
	compound->stmts = std::vector<StmtPtr>();

	while (!check(TokenType::RBrace)) {
		// 尝试解析局部变量声明或语句
		if (check(TokenType::kw_int) || check(TokenType::kw_char) || check(TokenType::kw_void)) {
			// 解析局部变量声明（只支持单个声明形式： type ident [= expr] ; ）
			Type vtype;
			if (check(TokenType::kw_int)) { vtype = Type::INT; advance(); }
			else if (check(TokenType::kw_char)) { vtype = Type::CHAR; advance(); }
			else { vtype = Type::VOID; advance(); }

			if (!check(TokenType::Identifier)) throw std::runtime_error("Parser: expected identifier in local declaration at line "+std::to_string(curToken().line)+", column "+std::to_string(curToken().column));
			std::string vname = curToken().lexeme; advance();
			auto vdecl = std::make_unique<VarDecl>();
			vdecl->type = vtype; vdecl->name = vname; vdecl->init = nullptr;
			if (check(TokenType::Assign)) {
				advance();
				vdecl->init = parseExpr();
			}
			expect(TokenType::Semicolon);
			compound->localVars.push_back(std::move(vdecl));
			continue;
		}
		compound->stmts.push_back(parseStmt());
	}
	expect(TokenType::RBrace);
	return compound;
}

// 表达式解析
ExprPtr Parser::parseExpr() {
	return parseAssignmentExpr();
}

ExprPtr Parser::parseAssignmentExpr() {
	auto left = parseLogicalOrExpr();
	if (check(TokenType::Assign)) {
		advance();
		auto right = parseAssignmentExpr();
		auto assignExpr = std::make_unique<AssignExpr>();
		assignExpr->left = std::move(left);
		assignExpr->right = std::move(right);
		return assignExpr;
	}
	return left;
}

ExprPtr Parser::parseLogicalOrExpr() {
	// 目前 TokenType 中未定义 ||/&&，所以直接降级到 logical and
	return parseLogicalAndExpr();
}

ExprPtr Parser::parseLogicalAndExpr() {
	// 目前 TokenType 中未定义 &&，直接降级到 equality
	return parseEqualityExpr();
}

ExprPtr Parser::parseEqualityExpr() {
	auto left = parseRelationalExpr();
	while (check(TokenType::Equal) || check(TokenType::NotEq)) {
		auto binExpr = std::make_unique<BinaryExpr>();
		binExpr->op = curToken().lexeme;
		advance();
		binExpr->left = std::move(left);
		binExpr->right = parseRelationalExpr();
		left = std::move(binExpr);
	}
	return left;
}

ExprPtr Parser::parseRelationalExpr() {
	auto left = parseAdditiveExpr();
	while (check(TokenType::Less) || check(TokenType::Greater) || check(TokenType::LessEq) || check(TokenType::GreaterEq)) {
		auto binExpr = std::make_unique<BinaryExpr>();
		binExpr->op = curToken().lexeme;
		advance();
		binExpr->left = std::move(left);
		binExpr->right = parseAdditiveExpr();
		left = std::move(binExpr);
	}
	return left;
}

ExprPtr Parser::parseAdditiveExpr() {
	auto left = parseMultiplicativeExpr();
	while (check(TokenType::Plus) || check(TokenType::Minus)) {
		auto binExpr = std::make_unique<BinaryExpr>();
		binExpr->op = curToken().lexeme;
		advance();
		binExpr->left = std::move(left);
		binExpr->right = parseMultiplicativeExpr();
		left = std::move(binExpr);
	}
	return left;
}

ExprPtr Parser::parseMultiplicativeExpr() {
	auto left = parseUnaryExpr();
	while (check(TokenType::Star) || check(TokenType::Slash) || check(TokenType::Percent)) {
		auto binExpr = std::make_unique<BinaryExpr>();
		binExpr->op = curToken().lexeme;
		advance();
		binExpr->left = std::move(left);
		binExpr->right = parseUnaryExpr();
		left = std::move(binExpr);
	}
	return left;
}

ExprPtr Parser::parseUnaryExpr() {
	if (check(TokenType::Plus) || check(TokenType::Minus) || check(TokenType::Star)) {
		auto unaryExpr = std::make_unique<UnaryExpr>();
		unaryExpr->op = curToken().lexeme;
		advance();
		unaryExpr->operand = parseUnaryExpr();
		return unaryExpr;
	}
	return parsePrimaryExpr();
}

ExprPtr Parser::parsePrimaryExpr() {
	if (check(TokenType::IntLiterial)) {
		auto intLit = std::make_unique<IntLiteral>();
		intLit->lexeme = curToken().lexeme;
		advance();
		return intLit;
	} else if (check(TokenType::CharLiterial)) {
		auto charLit = std::make_unique<CharLiteral>();
		charLit->lexeme = curToken().lexeme;
		advance();
		return charLit;
	} else if (check(TokenType::Identifier)) {
		std::string name = curToken().lexeme;
		advance();
		if (check(TokenType::LParen)) {
			// 函数调用
			advance();
			auto callExpr = std::make_unique<CallExpr>();
			callExpr->name = name;
			callExpr->args = std::vector<ExprPtr>();
			while (!check(TokenType::RParen)) {
				callExpr->args.push_back(parseExpr());
				if (!check(TokenType::RParen)) {
					expect(TokenType::Comma);
				}
			}
			expect(TokenType::RParen);
			return callExpr;
		} else {
			// 变量
			auto valExpr = std::make_unique<ValExpr>();
			valExpr->name = name;
			return valExpr;
		}
	} else if (check(TokenType::LParen)) {
		advance();
		auto expr = parseExpr();
		expect(TokenType::RParen);
		return expr;
	} else {
		throw std::runtime_error("Parser: unexpected token in primary expression at line "+std::to_string(curToken().line)+", column "+std::to_string(curToken().column));
	}
}
