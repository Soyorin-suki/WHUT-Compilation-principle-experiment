基于当前实现的 C 子集说明

- 类型：`int`、`char`、`void`、`double`。`float`/`long` 出现在词法关键字里，但语法不接受。
- 声明：顶层和块内均支持 `type IDENT` 可选 `= expr`，单个变量一行，不支持一次声明多个。
- 函数：`type IDENT ( params? ) compound`，参数类型同上；`(void)` 视为无参。形参也允许 `void`（与标准 C 不同）。
- 语句：复合语句、`if/else`、`while`、`for`、`return`、表达式语句、局部变量声明。空语句（单独 `;`）当前不支持。
- 表达式：右结合赋值；逻辑 `|| && !`；比较 `== != < > <= >=`；算术 `+ - * / %`；一元 `+ - * !`；括号；变量与函数调用；字面量 `int/char/double`。
- 词法：支持上述关键字、分隔符、运算符，字符串被识别成 Token 但语法未使用。

递归下降/优先级分层文法：

```
program          ::= decl program | EOF

decl             ::= type_spec IDENT decl_tail
decl_tail        ::= ";"                // 变量声明，无初始化
                   | "=" expr ";"       // 变量声明，带初始化
                   | "(" param_list_opt ")" compound_stmt   // 函数定义

param_list_opt   ::= param_list | ε
param_list       ::= param { "," param }
param            ::= type_spec IDENT

type_spec        ::= "int" | "char" | "void" | "double"

compound_stmt    ::= "{" { local_decl | stmt } "}"
local_decl       ::= type_spec IDENT [ "=" expr ] ";"

stmt             ::= compound_stmt
                   | if_stmt
                   | while_stmt
                   | for_stmt
                   | return_stmt
                   | expr ";"            // 不允许空表达式语句

if_stmt          ::= "if" "(" expr ")" stmt [ "else" stmt ]
while_stmt       ::= "while" "(" expr ")" stmt
for_stmt         ::= "for" "(" expr_opt ";" expr_opt ";" expr_opt ")" stmt
return_stmt      ::= "return" expr_opt ";"

expr             ::= assignment_expr
assignment_expr  ::= logical_or_expr
                   | unary_expr "=" assignment_expr   // 右结合

logical_or_expr  ::= logical_and_expr { "||" logical_and_expr }
logical_and_expr ::= equality_expr { "&&" equality_expr }
equality_expr    ::= relational_expr { ("==" | "!=") relational_expr }
relational_expr  ::= additive_expr { ("<" | ">" | "<=" | ">=") additive_expr }
additive_expr    ::= multiplicative_expr { ("+" | "-") multiplicative_expr }
multiplicative_expr ::= unary_expr { ("*" | "/" | "%") unary_expr }
unary_expr       ::= ("+" | "-" | "*" | "!") unary_expr
                   | primary_expr

primary_expr     ::= IDENT
                   | IDENT "(" [ expr { "," expr } ] ")"   // 函数调用
                   | INT_LITERAL
                   | CHAR_LITERAL
                   | DOUBLE_LITERAL
                   | "(" expr ")"

expr_opt         ::= expr | ε   // 仅用于 for/return
```

与实现的差异/注意事项：
- 本文法允许一元 `*`，实现同样接受（用作解引用/取值含义未定义，仅语法上通过）。
- 局部声明接受 `void`，与标准 C 不一致。
- 空语句 `;` 不会被解析成合法语句。
- 字符串字面量被词法器识别，但语法未使用。
