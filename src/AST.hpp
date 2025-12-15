#pragma once
#include <memory>
#include <ostream>
#include "Token.hpp"
#include "paster.hpp"
#include <vector>

// 类型枚举
enum class Type { INT, CHAR, VOID };

inline std::string typeToString(Type t) {
    switch(t) {
        case Type::INT: return "int";
        case Type::CHAR: return "char";
        case Type::VOID: return "void";
    }
    return "unknown";
}


// AST节点基类
struct ASTNode{
	int line=0,col=0;
	virtual ~ASTNode()=default;
	virtual void dump(std::ostream &out, int indent=0) const = 0;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

// 表达式
struct Expr : ASTNode{};
using ExprPtr = std::unique_ptr<Expr>;

// 语句
struct Stmt : ASTNode{};
using StmtPtr = std::unique_ptr<Stmt>;

// 声明
struct Decl : ASTNode{};
using DeclPtr = std::unique_ptr<Decl>;

// 程序
struct Program : ASTNode{
	std::vector<DeclPtr>decls;
	void dump(std::ostream &out, int indent) const override;
};
using ProgramPtr = std::unique_ptr<Program>;

// 变量声明
struct VarDecl : Decl{
	Type type;
	std::string name;
	ExprPtr init;	// nullable
	void dump(std::ostream &out,int indent)const override;
};

// 复合结构
struct CompoundStmt : Stmt{
	std::vector<std::unique_ptr<VarDecl>>localVars;
	std::vector<StmtPtr>stmts;
	void dump(std::ostream &out,int indent)const override;
};

// 函数参数
struct Param : ASTNode{
	Type type;
	std::string name;
	void dump(std::ostream &out, int indent) const override;
};


// 函数声明
struct FunDecl : Decl{
	Type returnType;
	std::string name;
	std::vector<std::unique_ptr<Param>>params;
	std::unique_ptr<CompoundStmt>body;
	void dump(std::ostream &out,int indent)const override;
};

// if语句
struct IfStmt : Stmt{
	ExprPtr cond;					// 条件表达式
	StmtPtr thenBranch,elseBranch;	// then结构,else结构
	void dump(std::ostream &out,int indent)const override;
};

// while语句
struct WhileStmt : Stmt{
	ExprPtr cond;		// 条件表达式
	StmtPtr body;		// 循环体
	void dump(std::ostream &out,int indent)const override;
};

// for语句
struct ForStmt : Stmt{
	ExprPtr init,cond,update;	// 都nullable
	StmtPtr body;				// 循环体
	void dump(std::ostream &out,int indent)const override;
};



// return语句
struct ReturnStmt : Stmt{
	ExprPtr expr;		// nullable
	void dump(std::ostream &out,int indent)const override;
};

// 表达式语句
struct ExprStmt : Stmt{
	ExprPtr expr;		// nullable
	void dump(std::ostream &out,int indent)const override;
};

// int字面量
struct IntLiteral : Expr{
	int value;
	void dump(std::ostream &out,int indent)const override;
};

// char字面量
struct CharLiteral : Expr{
	char value;
	void dump(std::ostream &out,int indent)const override;
};

// 变量
struct ValExpr : Expr{
	std::string name;
	void dump(std::ostream &out,int indent)const override;
};

// 赋值语句
struct AssignExpr : Expr{
	ExprPtr left,right;
	void dump(std::ostream &out,int indent)const override;
};

// 二元表达式
struct BinaryExpr : Expr{
	std::string op;
	ExprPtr left,right;
	void dump(std::ostream &out,int indent)const override;
};

// 一元表达式
struct UnaryExpr : Expr{
	std::string op;
	ExprPtr operand;
	void dump(std::ostream &out,int indent)const override;
};

// 调用表达式
struct CallExpr : Expr{
	std::string name;
	std::vector<ExprPtr>args;
	void dump(std::ostream &out,int indent)const override;
};



