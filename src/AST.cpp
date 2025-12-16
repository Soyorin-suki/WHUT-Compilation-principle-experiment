#include "AST.hpp"
#include <sstream>

std::string dumpAstToString(const Program &prog){
	std::ostringstream oss;
	prog.dump(oss,0);
	return oss.str();
}


// Program
void Program::dump(std::ostream &out, int indent) const {
	out<<std::string(indent,' ')<<"Program\n";
	for(auto&d:decls) d->dump(out, indent+2);
}

// VarDecl
void VarDecl::dump(std::ostream &out,int indent)const {
	out<<std::string(indent,' ')<<"VarDecl "<<typeToString(type)<<" "<<name<<'\n';
	out<<std::string(indent+2,' ')<<"init:\n";
	if(init)init->dump(out,indent+4);
	else out<<std::string(indent+4,' ')<<"(empty)\n";
}

// CompoundStmt
void CompoundStmt::dump(std::ostream &out,int indent)const {
	out<<std::string(indent,' ')<<"Compound\n";
	if(!localVars.empty()){
		out<<std::string(indent+2,' ')<<"locals:\n";
		for(auto &v:localVars)v->dump(out,indent+4);
	}
	if(!stmts.empty()){
		out<<std::string(indent+2,' ')<<"statements:\n";
		for(auto &s:stmts)s->dump(out,indent+4);
	}
}

// Param
void Param::dump(std::ostream &out, int indent) const {
	out << std::string(indent, ' ') << "Param " << typeToString(type) << " " << name << "\n";
}

// FunDecl
void FunDecl::dump(std::ostream &out,int indent)const {
	out<<std::string(indent,' ')<<"FunDecl "<<typeToString(returnType)<<" "<<name<<'\n';
	out<<std::string(indent+2,' ')<<"params:\n";
	if(params.empty())out<<std::string(indent+4,' ')<<"(none)\n";
	else for(auto &p:params)p->dump(out,indent+4);
	out<<std::string(indent+2,' ')<<"body:\n";
	if(body)body->dump(out,indent+4);
	else out<<std::string(indent+4,' ')<<"(empty)\n";
}

// IfStmt
void IfStmt::dump(std::ostream &out,int indent)const {
	out<<std::string(indent,' ')<<"If\n";
	out<<std::string(indent+2,' ')<<"cond:\n";
	cond->dump(out,indent+4);
	out<<std::string(indent+2,' ')<<"then:\n";
	thenBranch->dump(out,indent+4);
	out<<std::string(indent+2,' ')<<"else:\n";
	if(elseBranch)elseBranch->dump(out,indent+4);
	else out<<std::string(indent+4,' ')<<"(empty)\n";
}

// WhileStmt
void WhileStmt::dump(std::ostream &out,int indent)const {
	out<<std::string(indent,' ')<<"While\n";
	out<<std::string(indent+2,' ')<<"cond:\n";
	cond->dump(out,indent+4);
	out<<std::string(indent+2,' ')<<"body:\n";
	body->dump(out,indent+4);
}

// ForStmt
void ForStmt::dump(std::ostream &out,int indent)const {
	out<<std::string(indent,' ')<<"For\n";
	out<<std::string(indent+2,' ')<<"init:\n";
	if(init)init->dump(out,indent+4);
	else out<<std::string(indent+4,' ')<<"(empty)\n";
	out<<std::string(indent+2,' ')<<"cond:\n";
	if(cond)cond->dump(out,indent+4);
	else out<<std::string(indent+4,' ')<<"(empty)\n";
	out<<std::string(indent+2,' ')<<"update:\n";
	if(update)update->dump(out,indent+4);
	else out<<std::string(indent+4,' ')<<"(empty)\n";
	out<<std::string(indent+2,' ')<<"body:\n";
	body->dump(out,indent+4);
}

// ReturnStmt
void ReturnStmt::dump(std::ostream &out,int indent)const {
	out<<std::string(indent,' ')<<"Return\n";
	out<<std::string(indent+2,' ')<<"expr:\n";
	if(expr)expr->dump(out,indent+4);
	else out<<std::string(indent+4,' ')<<"(empty)\n";
}

// ExprStmt
void ExprStmt::dump(std::ostream &out,int indent)const {
	out<<std::string(indent,' ')<<"ExprStmt\n";
	out<<std::string(indent+2,' ')<<"expr:\n";
	if(expr)expr->dump(out,indent+4);
	else out<<std::string(indent+4,' ')<<"(empty)\n";
}

// IntLiteral
void IntLiteral::dump(std::ostream &out,int indent)const {
	out<<std::string(indent,' ')<<"Int "<<lexeme<<'\n';
}

// CharLiteral
void CharLiteral::dump(std::ostream &out,int indent)const {
	out<<std::string(indent,' ')<<"Char "<<lexeme<<'\n';
}

// ValExpr
void ValExpr::dump(std::ostream &out,int indent)const {
	out<<std::string(indent,' ')<<"Var "<<name<<'\n';
}

void DoubleLiteral::dump(std::ostream &out,int indent)const {
	out<<std::string(indent,' ')<<"Double "<<lexeme<<'\n';
}

// AssignExpr
void AssignExpr::dump(std::ostream &out,int indent)const {
	out<<std::string(indent,' ')<<"Assign\n";
	left->dump(out,indent+2);
	right->dump(out,indent+2);
}

// BinaryExpr
void BinaryExpr::dump(std::ostream &out,int indent)const {
	out<<std::string(indent,' ')<<"Binary "<<op<<'\n';
	left->dump(out,indent+2);
	right->dump(out,indent+2);
}

// UnaryExpr
void UnaryExpr::dump(std::ostream &out,int indent)const {
	out<<std::string(indent,' ')<<"Unary "<<op<<'\n';
	operand->dump(out,indent+2);
}

// CallExpr
void CallExpr::dump(std::ostream &out,int indent)const {
	out<<std::string(indent,' ')<<"Call "<<name<<'\n';
	if(args.empty())out<<std::string(indent+2,' ')<<"args: (none)\n";
	else{
		out<<std::string(indent+2,' ')<<"args:\n";
		for(auto &a:args)a->dump(out,indent+4);
	}
}
