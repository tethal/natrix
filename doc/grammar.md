# natrix grammar {#natrix_grammar}

### Top-level rules
```
file: statements EOF
```

### Statements
```
statements: 
    statement
    | statement statements

statement: simple_statement NEWLINE
simple_statement: 
    expression EQUALS expression
    | expression
```

### Expressions
```
expression: additive_expr

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
