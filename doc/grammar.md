# natrix grammar {#natrix_grammar}

### Top-level rules
```
file: statements EOF
```

### Statements
```
block: NEWLINE INDENT statements DEDENT

statements: statement+

statement:
    KW_WHILE expression COLON block
    | KW_IF expression COLON block (elif_block | else_block)?
    | simple_statement NEWLINE

simple_statement: 
    KW_PRINT LPAREN expression RPAREN
    | KW_PASS
    | expression EQUALS expression
    | expression

elif_block: KW_ELIF expression COLON block (elif_block | else_block)?

else_block: KW_ELSE COLON block
```

### Expressions
```
expression_list: expression (COMMA expression)* COMMA?

expression: relational_expr

relational_expr:
    relational_expr (EQ | NE | GT | GE | LT | LE) additive_expr
    | additive_expr

additive_expr:
    additive_expr (PLUS | MINUS) multiplicative_expr 
    | multiplicative_expr

multiplicative_expr:
    multiplicative_expr (STAR | SLASH) postfix_expr
    | postfix_expr

postfix_expr: primary (LBRACKET expression RBRACKET)*

primary:
    INT_LITERAL
    | IDENTIFIER
    | LPAREN expression RPAREN
    | LBRACKET expression_list? RBRACKET
```
