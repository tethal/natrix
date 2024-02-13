# natrix grammar {#natrix_grammar}

### Top-level rules
```
file: statements EOF
```

### Statements
```
block: NEWLINE INDENT statements DEDENT

statements: 
    statement
    | statement statements

statement:
    KW_WHILE expression COLON block
    | KW_IF expression COLON block
    | KW_IF expression COLON block else_block
    | KW_IF expression COLON block elif_block
    | simple_statement NEWLINE

simple_statement: 
    KW_PRINT LPAREN expression RPAREN
    | KW_PASS
    | expression EQUALS expression
    | expression

 elif_block:
     KW_ELIF expression COLON block
     | KW_ELIF expression COLON block else_block
     | KW_ELIF expression COLON block elif_block

else_block: KW_ELSE COLON block
```

### Expressions
```
expression: relational_expr

relational_expr:
    relational_expr (EQ | NE | GT | GE | LT | LE) additive_expr
    | additive_expr

additive_expr:
    additive_expr (PLUS | MINUS) multiplicative_expr 
    | multiplicative_expr

multiplicative_expr:
    multiplicative_expr (STAR | SLASH) primary
    | primary

primary:
    INT_LITERAL
    | IDENTIFIER
    | LPAREN expression RPAREN
```
