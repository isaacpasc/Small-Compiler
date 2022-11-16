//
// Created by cse310 on 11/15/22.
//

#include <iostream>
#include "parser.h"

using namespace std;

void parser::error(int id) {
    cout << "ERROR AT " << id << endl;
    exit(1);
}

Token parser::expect(TokenType expected_type, int id) {
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        cout << "EXPECTED: " << expected_type << endl;
        cout << "GOT: " << t.token_type << endl;
        error(id);
    return t;
}

void parser::printAllInput() {
    Token t;
    t = lexer.GetToken();
    while (t.token_type != END_OF_FILE) {
        t.Print();
        t = lexer.GetToken();
    }
}

void parser::parse_Input() {
    parse_program();
    expect(END_OF_FILE, 1);
}

void parser::parse_program() {
    parse_var_section();
    parse_body();
    parse_inputs();
}

void parser::parse_var_section() {
    parse_id_list();
    expect(SEMICOLON, 2);
}

void parser::parse_id_list() {
    expect(ID, 3);
    Token t = lexer.peek(1);
    if (t.token_type == COMMA) {
        expect(COMMA, 4);
        parse_id_list();
    }
}

void parser::parse_body() {
    expect(LBRACE, 5);
    parse_stmt_list();
    expect(RBRACE, 6);
}

void parser::parse_stmt_list() {
    parse_stmt();
    Token t = lexer.peek(1);
    if (t.token_type != RBRACE) {
        parse_stmt_list();
    }
}

void parser::parse_stmt() {
    Token t = lexer.peek(1);
    if (t.token_type == ID) {
        parse_assign_stmt();
    } else if (t.token_type == WHILE) {
        parse_while_stmt();
    } else if (t.token_type == IF) {
        parse_if_stmt();
    } else if (t.token_type == SWITCH) {
        parse_switch_stmt();
    } else if (t.token_type == FOR) {
        parse_for_stmt();
    } else if (t.token_type == OUTPUT) {
        parse_output_stmt();
    } else if (t.token_type == INPUT) {
        parse_input_stmt();
    }
}

void parser::parse_assign_stmt() {
    expect(ID, 7);
    expect(EQUAL, 8);
    Token t = lexer.peek(2);
    if (t.token_type == PLUS || t.token_type == MINUS || t.token_type == DIV || t.token_type == MULT) {
        parse_expr();
    } else {
        parse_primary();
    }
    expect(SEMICOLON, 9);
}

void parser::parse_expr() {
    parse_primary();
    parse_op();
    parse_primary();
}

void parser::parse_primary() {
    Token t = lexer.peek(1);
    if (t.token_type == ID) {
        expect(ID, 10);
    } else {
        expect(NUM, 11);
    }
}

void parser::parse_op() {
    Token t = lexer.peek(1);
    if (t.token_type == MINUS) {
        expect(MINUS, 12);
    } else if (t.token_type == PLUS) {
        expect(PLUS, 13);
    } else if (t.token_type == DIV) {
        expect(DIV, 14);
    } else if (t.token_type == MULT) {
        expect(MULT, 15);
    }
}

void parser::parse_input_stmt() {
    expect(INPUT, 16);
    expect(ID, 17);
    expect(SEMICOLON, 18);
}

void parser::parse_output_stmt() {
    expect(OUTPUT, 19);
    expect(ID, 20);
    expect(SEMICOLON, 21);
}

void parser::parse_while_stmt() {
    expect(WHILE, 22);
    parse_condition();
    parse_body();
}

void parser::parse_if_stmt() {
    expect(IF, 23);
    parse_condition();
    parse_body();
}

void parser::parse_condition() {
    parse_primary();
    parse_relop();
    parse_primary();
}

void parser::parse_relop() {
    Token t = lexer.peek(1);
    if (t.token_type == GREATER) {
        expect(GREATER, 24);
    } else if (t.token_type == LESS) {
        expect(LESS, 25);
    } else if (t.token_type == NOTEQUAL) {
        expect(NOTEQUAL, 26);
    }
}

void parser::parse_switch_stmt() {
    expect(SWITCH, 27);
    expect(ID, 28);
    expect(LBRACE, 29);
    parse_case_list();
    Token t = lexer.peek(1);
    if (t.token_type != RBRACE) {
        parse_default_case();
    }
    expect(RBRACE, 30);
}

void parser::parse_for_stmt() {
    expect(FOR, 31);
    expect(LPAREN, 32);
    parse_assign_stmt();
    parse_condition();
    expect(SEMICOLON, 33);
    parse_assign_stmt();
    expect(RPAREN, 34);
    parse_body();
}

void parser::parse_case_list() {
    parse_case();
    Token t = lexer.peek(1);
    if (t.token_type == RBRACE || t.token_type == DEFAULT) {
        parse_case_list();
    }
}

void parser::parse_case() {
    expect(CASE, 35);
    expect(NUM, 36);
    expect(COLON, 37);
    parse_body();
}

void parser::parse_default_case() {
    expect(DEFAULT, 38);
    expect(COLON, 39);
    parse_body();
}

void parser::parse_inputs() {
    parse_num_list();
}

void parser::parse_num_list() {
    expect(NUM, 40);
    Token t = lexer.peek(1);
    if (t.token_type == NUM) {
        parse_num_list();
    }
}
