//
// Created by cse310 on 11/15/22.
//

#ifndef P3CSE340_PARSER_H
#define P3CSE340_PARSER_H

#include "lexer.h"


class parser {
public:
    void parse_Input();
private:
    LexicalAnalyzer lexer;
    void error(int id);
    Token expect(TokenType expected_type, int id);
    void printAllInput();

    void parse_program();
    void parse_var_section();
    void parse_id_list();
    void parse_body();
    void parse_stmt_list();
    void parse_stmt();
    void parse_assign_stmt();
    void parse_expr();
    void parse_primary();
    void parse_op();
    void parse_input_stmt();
    void parse_output_stmt();
    void parse_while_stmt();
    void parse_if_stmt();
    void parse_condition();
    void parse_relop();
    void parse_switch_stmt();
    void parse_for_stmt();
    void parse_case_list();
    void parse_case();
    void parse_default_case();
    void parse_inputs();
    void parse_num_list();
};


#endif //P3CSE340_PARSER_H
