采用的 C 语言子集概述：

- 支持基本类型：`int`、`char`、`void`（用于函数返回类型）、`double`
- 支持变量声明（可初始化）、函数定义、`if`/`else`、`while`、`for`、`return`。
- 支持算术（`+ - * / %`）和逻辑运算（`&& || !`），以及比较运算符。

下面使用 BNF/递归形式给出文法：

```
program ::= decl program | EOF

decl ::= val_decl | func_decl

val_decl ::= type_spec IDENT init_opt ";"
init_opt ::= "=" expr | ε

func_decl ::= type_spec IDENT "(" param_list_opt ")" compound_stmt
param_list_opt ::= param_list | ε
param_list ::= param | param "," param_list
param ::= type_spec IDENT

type_spec ::= "int" | "char" | "void"

compound_stmt ::= "{" stmt_list "}"
stmt_list ::= stmt stmt_list | ε

stmt ::= expr_stmt
	| compound_stmt
	| if_stmt
	| while_stmt
	| for_stmt
	| return_stmt
	| val_decl

expr_stmt ::= expr_opt ";"
expr_opt ::= expr | ε

if_stmt ::= "if" "(" expr ")" stmt else_opt
else_opt ::= "else" stmt | ε

while_stmt ::= "while" "(" expr ")" stmt

for_stmt ::= "for" "(" expr_opt ";" expr_opt ";" expr_opt ")" stmt

return_stmt ::= "return" expr_opt ";"

/* 表达式（带运算优先级） */
expr ::= assignment_expression
assignment_expression ::= logical_or_expression
			   | unary_expression "=" assignment_expression

logical_or_expression ::= logical_and_expression { "||" logical_and_expression }
logical_and_expression ::= equality_expression { "&&" equality_expression }
equality_expression ::= relational_expression { ("==" | "!=") relational_expression }
relational_expression ::= additive_expression { ("<" | ">" | "<=" | ">=") additive_expression }
additive_expression ::= multiplicative_expression { ("+" | "-") multiplicative_expression }
multiplicative_expression ::= unary_expression { ("*" | "/" | "%") unary_expression }
unary_expression ::= ("+" | "-" | "!") unary_expression | primary_expression
primary_expression ::= IDENT | INT_LITERAL | CHAR_LITERAL | "(" expr ")"

/* 终结符说明（由词法器产生） */
IDENT       /* 标识符 */
INT_LITERAL /* 十进制整数字面量 */
CHAR_LITERAL/* 字符字面量 */
DOUBLE_LITERAL /* 双精度浮点数字面量 */
EOF         /* 输入结束 */
```

下文对文法中的每个主要非终结符作说明：

- `program`：整个翻译单元的起点；由若干 `decl`（声明或定义）组成，最后以 `EOF` 结束。使用 `program ::= decl program | EOF` 表示任意个零或多个 `decl` 后跟 `EOF`。
- `decl`：顶层声明，可能是变量声明（`val_decl`）或函数定义（`func_decl`）。
- `val_decl`：变量声明，格式为 `type IDENT`，可选地带上初始化器（`=` 表达式），以分号结束。例如 `int x;` 或 `int x = 3;`。
- `init_opt`：可选初始化部分，允许为空（`ε`）。
- `func_decl`：函数定义，包括返回类型、函数名、参数列表（可选）及函数体（复合语句）。
- `param_list_opt` / `param_list` / `param`：函数参数相关，允许无参或多个参数，每个参数为 `type IDENT`。
- `type_spec`：类型说明符，当前子集只支持 `int`、`char`、`void`。
- `compound_stmt`：复合语句（块），以 `{}` 包围，内部可包含声明和语句序列。
- `stmt_list`：语句序列或空。
- `stmt`：语句，可以是表达式语句、复合语句、分支、循环、返回或局部变量声明等。
- `expr_stmt`：以分号结束的表达式语句；允许为空（即仅分号，表示空语句）。
- `if_stmt` / `else_opt`：条件分支；`else` 分支是可选的。
- `while_stmt`：`while` 循环。
- `for_stmt`：C 风格 `for` 循环，三个子表达式都可省略（用作空表达式）。
- `return_stmt`：返回语句，返回值表达式可选（`void` 情形或空返回）。
- `expr` 及其子产生式：表达式层次，按运算符优先级从赋值到逻辑到比较再到加减乘除，最后是主表达式。
- `primary_expression`：基本项，包含标识符、字面量或圆括号内的表达式。

词法器（`Lexer`）需生成与上文终结符对应的 token（如 `IDENT`、`INT_LITERAL`、关键字 `int`/`if`/`return` 等、运算符和分隔符）。确保 `TokenType.hpp` 中的 token 名称与文法匹配。


-- 结束 --
