# Small-Compiler

A small C++ compiler that will read an input program and represent it in a linked list. A node of the linked list represents one instruction. An instruction node specifies: (1) the type of the instruction, (2) the operand(s) of the instruction (if any) and, for jump instructions, the next instruction to be executed (the default is to execute instruction in the list order). After the list of instructions is generated by the compiler, the compiler will execute the generated list of instructions by interpreting it. This means that the program will traverse the data structure and, at every node it visits, it will “execute” the node by changing the content of memory locations corresponding to operands and then proceeds to execute the next instruction after determining what that instruction should be. This process continues until there is no next instruction to execute.

## Grammer:

- program → var section body inputs
- var section → id list SEMICOLON
- id list → ID COMMA id list | ID
- body → LBRACE stmt list RBRACE
- stmt list → stmt stmt list | stmt
- stmt → assign stmt | while stmt | if stmt | switch stmt | for stmt
- stmt → output stmt | input stmt
- assign stmt → ID EQUAL primary SEMICOLON
- assign stmt → ID EQUAL expr SEMICOLON
- expr → primary op primary
- primary → ID | NUM
- op → PLUS | MINUS | MULT | DIV
- output stmt → output ID SEMICOLON
- input stmt → input ID SEMICOLON
- while stmt → WHILE condition body
- if stmt → IF condition body
- condition → primary relop primary
- relop → GREATER | LESS | NOTEQUAL
- switch stmt → SWITCH ID LBRACE case list RBRACE
- switch stmt → SWITCH ID LBRACE case list default case RBRACE
- for stmt → FOR LPAREN assign stmt condition SEMICOLON assign stmt RPAREN body
- case list → case case list | case
- case → CASE NUM COLON body
- default case → DEFAULT COLON body
- inputs → num list
- num list → NUM
- num list → NUM num list
