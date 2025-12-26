基于当前实现的 C 子集说明

- 类型：`int`、`char`、`void`、`double`。`float`/`long` 出现在词法关键字里，但语法不接受。
- 声明：顶层和块内均支持 `type IDENT` 可选 `= expr`，单个变量一行，不支持一次声明多个。
- 函数：`type IDENT ( params? ) compound`，参数类型同上；`(void)` 视为无参。形参也允许 `void`（与标准 C 不同）。
- 语句：复合语句、`if/else`、`while`、`for`、`return`、表达式语句、局部变量声明。空语句（单独 `;`）当前不支持。
- 表达式：右结合赋值；逻辑 `|| && !`；比较 `== != < > <= >=`；算术 `+ - * / %`；一元 `+ - * !`；括号；变量与函数调用；字面量 `int/char/double`。
- 词法：支持上述关键字、分隔符、运算符，字符串被识别成 Token 但语法未使用。

递归下降/优先级分层文法：

```
# ========= Program / Decl =========
program        ::= decl_list EOF
decl_list       ::= decl decl_list | ε

decl            ::= type_spec IDENT decl_tail
decl_tail       ::= ";" 
                 | "=" expr ";" 
                 | "(" param_clause compound_stmt

# 说明：把右括号并入 param_clause，便于用 1 个 lookahead 决定参数分支
param_clause    ::= ")" 
                 | "void" param_clause_after_void
                 | type_spec_nonvoid IDENT param_list_tail ")"

param_clause_after_void ::= ")"
                         | IDENT param_list_tail ")"

param_list_tail ::= "," param param_list_tail | ε
param           ::= type_spec IDENT

type_spec       ::= "int" | "char" | "void" | "double"
type_spec_nonvoid ::= "int" | "char" | "double"

# ========= Compound / Local Decl =========
compound_stmt   ::= "{" compound_items "}"
compound_items  ::= compound_item compound_items | ε
compound_item   ::= local_decl | stmt

local_decl      ::= type_spec IDENT local_init_opt ";"
local_init_opt  ::= "=" expr | ε

# ========= Stmt (dangling-else resolved) =========
stmt            ::= matched_stmt | unmatched_stmt | other_stmt

matched_stmt    ::= "if" "(" expr ")" matched_stmt "else" matched_stmt
                 | other_stmt

unmatched_stmt  ::= "if" "(" expr ")" stmt
                 | "if" "(" expr ")" matched_stmt "else" unmatched_stmt

other_stmt      ::= compound_stmt
                 | while_stmt
                 | for_stmt
                 | return_stmt
                 | expr ";"

while_stmt      ::= "while" "(" expr ")" stmt
for_stmt        ::= "for" "(" expr_opt ";" expr_opt ";" expr_opt ")" stmt
return_stmt     ::= "return" expr_opt ";"
expr_opt        ::= expr | ε

# ========= Expr (LL(1)-friendly) =========
expr            ::= assignment_expr

# 说明：这样做 LL(1) 没问题；“左边必须是左值”留到语义分析检查
assignment_expr ::= logical_or_expr assignment_tail
assignment_tail ::= "=" assignment_expr | ε

logical_or_expr ::= logical_and_expr logical_or_tail
logical_or_tail ::= "||" logical_and_expr logical_or_tail | ε

logical_and_expr ::= equality_expr logical_and_tail
logical_and_tail ::= "&&" equality_expr logical_and_tail | ε

equality_expr   ::= relational_expr equality_tail
equality_tail   ::= ("==" | "!=") relational_expr equality_tail | ε

relational_expr ::= additive_expr relational_tail
relational_tail ::= ("<" | ">" | "<=" | ">=") additive_expr relational_tail | ε

additive_expr   ::= multiplicative_expr additive_tail
additive_tail   ::= ("+" | "-") multiplicative_expr additive_tail | ε

multiplicative_expr ::= unary_expr multiplicative_tail
multiplicative_tail ::= ("*" | "/" | "%") unary_expr multiplicative_tail | ε

unary_expr      ::= unary_op unary_expr | primary_expr
unary_op        ::= "+" | "-" | "*" | "!"

primary_expr    ::= IDENT primary_suffix
                 | INT_LITERAL
                 | CHAR_LITERAL
                 | DOUBLE_LITERAL
                 | "(" expr ")"

primary_suffix  ::= "(" arg_list_opt ")" | ε
arg_list_opt    ::= expr arg_list_tail | ε
arg_list_tail   ::= "," expr arg_list_tail | ε
```

与实现的差异/注意事项：
- 本文法允许一元 `*`，实现同样接受（用作解引用/取值含义未定义，仅语法上通过）。
- 局部声明接受 `void`，与标准 C 不一致。
- 空语句 `;` 不会被解析成合法语句。
- 字符串字面量被词法器识别，但语法未使用。
