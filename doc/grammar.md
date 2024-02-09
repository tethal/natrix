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

statement: expression NEWLINE
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
    | LPAREN expression RPAREN
```
