基于当前实现的 C 子集说明

- 类型：`int`、`char`、`void`、`double`。`float`/`long` 出现在词法关键字里，但语法不接受。
- 声明：顶层和块内均支持 `type IDENT` 可选 `= expr`，单个变量一行，不支持一次声明多个。
- 函数：`type IDENT ( params? ) compound`，参数类型同上；`(void)` 视为无参。
- 语句：复合语句、`if/else`、`while`、`for`、`return`、表达式语句、局部变量声明。空语句（单独 `;`）当前不支持。
- 表达式：右结合赋值；逻辑 `|| && !`；比较 `== != < > <= >=`；算术 `+ - * / %`；一元 `+ - * !`；括号；变量与函数调用；字面量 `int/char/double`。
- 词法：支持上述关键字、分隔符、运算符，字符串被识别成 Token 但语法未使用。

递归下降/优先级分层文法：

```
# ========= Program / Decl =========
program        ::= decl_list EOF
decl_list       ::= decl decl_list | ε

decl            ::= type_spec IDENT decl_tail
decl_tail       ::= ";"                             # 单独的变量声明,FIRST()={";"}
                 | "=" expr ";"                     # 变量声明并初始化,FIRST()={"="}
                 | "(" param_clause compound_stmt   # 函数声明定义,FIRST()={"("}

# 说明：把右括号并入 param_clause，便于用 1 个 lookahead 决定参数分支
param_clause    ::= ")"                             # 无参,FIRST()={")"}
                 | "void" ")"                       # void 形参, FIRST()={"void"}
                 | type_spec_nonvoid IDENT param_list_tail ")"      # 非 void 形参, FIRST()={"int","char","double"}


param_list_tail ::= "," param param_list_tail | ε       # 多个参数, FIRST()={",",")"}
param           ::= type_spec IDENT                     # 单个参数, FIRST()={"int","char","double"}

type_spec       ::= "int" | "char" | "void" | "double"
type_spec_nonvoid ::= "int" | "char" | "double"

# ========= Compound / Local Decl =========
compound_stmt   ::= "{" compound_items "}"              # 复合语句, FIRST()={"{"}
compound_items  ::= compound_item compound_items | ε    # 复合语句内项列表, FIRST()={"int","char","void","double","if","while","for","return",";","IDENT","{"},FOLLOW()={"}"}
compound_item   ::= local_decl | stmt                   # 局部声明或语句, FIRST()={"int","char","void","double","if","while","for","return",";","IDENT","{"}

local_decl      ::= type_spec IDENT local_init_opt ";"  # 局部变量声明, FIRST()={"int","char","double","void"}
local_init_opt  ::= "=" expr | ε                  # 可选初始化, FIRST()={"="} or FOLLOW()={";"}

# ========= Stmt (dangling-else resolved) =========
stmt            ::= matched_stmt | unmatched_stmt | other_stmt        # 语句, FIRST()={"if","while","for","return",";","IDENT","{"}

matched_stmt    ::= "if" "(" expr ")" matched_stmt "else" matched_stmt  # 已匹配的 if-else 语句, FIRST()={"if"}
                 | other_stmt                                 # 其他语句, FIRST()={"while","for","return",";","IDENT","{"}

unmatched_stmt  ::= "if" "(" expr ")" stmt
                 | "if" "(" expr ")" matched_stmt "else" unmatched_stmt

other_stmt      ::= compound_stmt       # 复合语句, FIRST()={"{"}
                 | while_stmt           # while语句,FIRST()={"while"}
                 | for_stmt          # for语句,FIRST()={"for"}
                 | return_stmt     # return语句,FIRST()={"return"}
                 | expr ";"          # 表达式语句,FIRST()={IDENT,INT_LITERAL,CHAR_LITERAL,DOUBLE_LITERAL,"(","+","-","*","!"}

while_stmt      ::= "while" "(" expr ")" stmt      # while语句，暂时不提供语义翻译
for_stmt        ::= "for" "(" expr_opt ";" expr_opt ";" expr_opt ")" stmt       # for语句，暂时不提供语义翻译
return_stmt     ::= "return" expr_opt ";"           # return语句,FIRST()={"return"}
expr_opt        ::= expr | ε    # 可选表达式,FIRST()={IDENT,INT_LITERAL,CHAR_LITERAL,DOUBLE_LITERAL,"(","+","-","*","!"} , FOLLOW()={";"}

# ========= Expr (LL(1)-friendly) =========
expr            ::= assignment_expr                 # 表达式入口,FIRST()={IDENT,INT_LITERAL,CHAR_LITERAL,DOUBLE_LITERAL,"(","+","-","*","!"}

# 说明：这样做 LL(1) 没问题；“左边必须是左值”留到语义分析检查
assignment_expr ::= logical_or_expr assignment_tail         # 赋值表达式,FIRST()={IDENT,INT_LITERAL,CHAR_LITERAL,DOUBLE_LITERAL,"(","+","-","*","!"}
assignment_tail ::= "=" assignment_expr | ε         # 可选赋值,FIRST()={"="} or FOLLOW()={EOF,";","}","else",")",",","||","&&","==","!=","<",">","<=",">=","+","-","*","/","%"}

logical_or_expr ::= logical_and_expr logical_or_tail        # 逻辑或表达式,FIRST()={IDENT,INT_LITERAL,CHAR_LITERAL,DOUBLE_LITERAL,"(","+","-","*","!"}
logical_or_tail ::= "||" logical_and_expr logical_or_tail | ε       # 逻辑或表达式尾部,FIRST()={"||"} or FOLLOW()={EOF,";","}","else",")",",","&&","==","!=","<",">","<=",">=","+","-","*","/","%"}

logical_and_expr ::= equality_expr logical_and_tail       # 逻辑与表达式,FIRST()={IDENT,INT_LITERAL,CHAR_LITERAL,DOUBLE_LITERAL,"(","+","-","*","!"}
logical_and_tail ::= "&&" equality_expr logical_and_tail | ε    # 逻辑与表达式尾部,FIRST()={"&&"} or FOLLOW()={EOF,";","}","else",")",",","||","==","!=","<",">","<=",">=","+","-","*","/","%"}

equality_expr   ::= relational_expr equality_tail       # 相等表达式,FIRST()={IDENT,INT_LITERAL,CHAR_LITERAL,DOUBLE_LITERAL,"(","+","-","*","!"}
equality_tail   ::= ("==" | "!=") relational_expr equality_tail | ε     # 相等表达式尾部,FIRST()={"==","!="} or FOLLOW()={EOF,";","}","else",")",",","||","&&","<",">","<=",">=","+","-","*","/","%"}

relational_expr ::= additive_expr relational_tail       # 关系表达式,FIRST()={IDENT,INT_LITERAL,CHAR_LITERAL,DOUBLE_LITERAL,"(","+","-","*","!"}
relational_tail ::= ("<" | ">" | "<=" | ">=") additive_expr relational_tail | ε     # 关系表达式尾部,FIRST()={"<",">","<=",">="} or FOLLOW()={EOF,";","}","else",")",",","||","&&","==","!=","+","-","*","/","%"}

additive_expr   ::= multiplicative_expr additive_tail       # 加法表达式,FIRST()={IDENT,INT_LITERAL,CHAR_LITERAL,DOUBLE_LITERAL,"(","+","-","*","!"}
additive_tail   ::= ("+" | "-") multiplicative_expr additive_tail | ε   # 加法表达式尾部,FIRST()={"+","-"} or FOLLOW()={EOF,";","}","else",")",",","||","&&","==","!=","<",">","<=",">=","*","/","%"}

multiplicative_expr ::= unary_expr multiplicative_tail      # 乘法表达式,FIRST()={IDENT,INT_LITERAL,CHAR_LITERAL,DOUBLE_LITERAL,"(","+","-","*","!"}
multiplicative_tail ::= ("*" | "/" | "%") unary_expr multiplicative_tail | ε        # 乘法表达式尾部,FIRST()={"*","/","%"} or FOLLOW()={EOF,";","}","else",")",",","||","&&","==","!=","<",">","<=",">=","+","-"}

unary_expr      ::= unary_op unary_expr             # 一元表达式,FIRST()={"+","-","*","!"}
                 | primary_expr                 # 基本表达式,FIRST()={IDENT,INT_LITERAL,CHAR_LITERAL,DOUBLE_LITERAL,"("}
unary_op        ::= "+" | "-" | "*" | "!"

primary_expr    ::= IDENT primary_suffix        # 函数调用或变量,FIRST()={IDENT}
                 | INT_LITERAL
                 | CHAR_LITERAL
                 | DOUBLE_LITERAL
                 | "(" expr ")"

primary_suffix  ::= "(" arg_list_opt ")" | ε    # 函数调用参数列表,FIRST()={"("} or FOLLOW()={EOF,";","}","else",")",",","||","&&","==","!=","<",">","<=",">=","+","-","*","/","%"}
arg_list_opt    ::= expr arg_list_tail | ε      # 可选参数列表,FIRST()={IDENT,INT_LITERAL,CHAR_LITERAL,DOUBLE_LITERAL,"(","+","-","*","!"} or FOLLOW()={")"}
arg_list_tail   ::= "," expr arg_list_tail | ε   # 多个参数,FIRST()={","} or FOLLOW()={")"}
```

## 属性文法 / 语法制导定义（SDD）说明

本项目的“语义分析 + 中间代码生成”可以用属性文法（更准确说是**语法制导定义 SDD**）来解释：在 CFG 的产生式上附加**属性**与**语义规则/语义动作**。

### 1) 主要属性（建议在报告里这样写）

为贴合当前实现（先构建 AST，再遍历做语义分析与三地址码生成），可以采用以下抽象属性来描述：

- 表达式 `E` 的综合属性（Synthesized）：
    - `E.type`：表达式类型（`int/char/double/void`）
    - `E.place`：表达式值所在位置（变量名或临时变量 `t1`、`t2`…）
    - `E.code`：计算该表达式的三地址码序列

- 语句 `S` 的综合属性：
    - `S.code`：执行该语句的三地址码序列

（可选）控制流相关属性（如果以后扩展短路或 while/for 会更有用）：
- `S.next`：语句结束后的出口标签
- `E.true / E.false`：条件表达式为真/假时跳转到的标签

### 2) 与当前实现对应的语义规则（核心覆盖 if/else、assign、call、return）

下面用“产生式 + 语义规则”的形式描述（伪代码）。其中：
- `newTemp()` 生成新临时变量名 `t1,t2,...`
- `newLabel()` 生成新标签 `L1,L2,...`
- `emit(x)` 向 `code` 追加一条三地址码
- `concat(A,B,...)` 表示代码序列拼接

#### (a) 赋值（assign）

产生式（抽象）：

```
E -> ID "=" E1
```

语义规则：

```
E.type  = typeOf(ID)
E.place = ID.lexeme
E.code  = concat(E1.code, emit(ID.lexeme " = " E1.place))
```

类型规则（语义分析阶段）：
- `E1.type` 必须可提升到 `typeOf(ID)`（允许 `char -> int -> double`）

#### (b) 二元运算（用于比较/算术/逻辑）

产生式（抽象）：

```
E -> E1 op E2
```

语义规则：

```
E.place = newTemp()
E.type  = resultType(E1.type, op, E2.type)
E.code  = concat(E1.code,
                                 E2.code,
                                 emit(E.place " = " E1.place " " op " " E2.place))
```

说明：你的实现里比较运算（如 `<`）会生成 `t = a < b`，再由 `if t != 0 goto ...` 使用。

#### (c) 函数调用（call）

产生式（抽象）：

```
E -> ID "(" args ")"
```

语义规则（作为“有返回值的表达式”）：

```
E.place = newTemp()
E.type  = returnType(ID)
E.code  = concat(args.code,
                                 emit(E.place " = call " ID.lexeme ", " args.count))
```

语义规则（作为语句 `call f(...);`）：

```
S.code = concat(args.code, emit("call " ID.lexeme ", " args.count))
```

其中 `args.code` 典型为：对每个实参 `Ai` 先求值再 `emit("param " + Ai.place)`。

#### (d) return

产生式（抽象）：

```
S -> "return" E ";"
S -> "return" ";"
```

语义规则：

```
S.code = concat(E.code, emit("return " + E.place))
S.code = emit("return")
```

类型规则（语义分析阶段）：
- 若函数返回类型为 `void`，只能 `return;`
- 若函数返回类型非 `void`，必须 `return E;` 且 `E.type` 可提升到返回类型

#### (e) if / if-else

产生式（抽象）：

```
S -> if (E) S1
S -> if (E) S1 else S2
```

语义规则（与当前三地址码生成方式一致：先算出 E.place，再用 jmp/label）：

```
S -> if (E) S1
L_then = newLabel(); L_end = newLabel();
S.code = concat(E.code,
                                emit("if " + E.place + " != 0 goto " + L_then),
                                emit("goto " + L_end),
                                emit(L_then + ":"),
                                S1.code,
                                emit(L_end + ":"))

S -> if (E) S1 else S2
L_then = newLabel(); L_else = newLabel(); L_end = newLabel();
S.code = concat(E.code,
                                emit("if " + E.place + " != 0 goto " + L_then),
                                emit("goto " + L_else),
                                emit(L_then + ":"),
                                S1.code,
                                emit("goto " + L_end),
                                emit(L_else + ":"),
                                S2.code,
                                emit(L_end + ":"))
```

说明：这里的条件判定采用“非 0 为真”，与你当前生成 `t = (db < x)` 再 `if t != 0 goto ...` 的风格一致。

### 3) 与实现方式的关系

- 经典属性文法通常“边分析边执行语义动作”。
- 你当前实现是“先构 AST，再做语义分析/代码生成遍历”，等价于把语义动作从解析阶段推迟到遍历阶段；在课程报告里仍然可以用 SDD 来描述其正确性与生成规则。


