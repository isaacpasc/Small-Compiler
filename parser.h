#ifndef P3CSE340_PARSER_H
#define P3CSE340_PARSER_H

#include <map>
#include <string>
#include <tuple>
#include "lexer.h"
#include "execute.h"


class parser {
public:
    InstructionNode* parse_Input();
private:
    LexicalAnalyzer lexer;
    std::map<std::string, int> locationTable;
    static void error(int id);
    Token expect(TokenType expected_type, int id);

    InstructionNode* parse_program();
    void parse_var_section();
    void parse_id_list();
    InstructionNode* parse_body();
    InstructionNode* parse_stmt_list();
    InstructionNode* parse_stmt();
    InstructionNode* parse_assign_stmt();
    void parse_expr(InstructionNode*);
    int parse_primary();
    ArithmeticOperatorType parse_op();
    InstructionNode* parse_input_stmt();
    InstructionNode* parse_output_stmt();
    InstructionNode* parse_while_stmt();
    InstructionNode* parse_if_stmt();
    std::tuple<int, ConditionalOperatorType, int> parse_condition();
    ConditionalOperatorType parse_relop();
    InstructionNode* parse_switch_stmt();
    InstructionNode* parse_for_stmt();
    InstructionNode* parse_case_list(InstructionNode*, int);
    InstructionNode* parse_case(InstructionNode*, int);
    InstructionNode* parse_default_case();
    void parse_inputs();
    void parse_num_list();
};


#endif
