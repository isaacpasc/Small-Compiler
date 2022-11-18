#include <iostream>
#include "parser.h"

using namespace std;

struct InstructionNode * parse_generate_intermediate_representation() {
    parser p;
    return p.parse_Input();
}

void parser::error(int id) {
    cout << "ERROR AT " << id << endl;
    exit(1);
}

Token parser::expect(TokenType expected_type, int id) {
    Token t = lexer.GetToken();
    if (t.token_type != expected_type) {
        cout << "EXPECTED: " << expected_type << endl;
        cout << "GOT: " << t.token_type << endl;
        error(id);
    }
    return t;
}

InstructionNode* parser::parse_Input() {
    InstructionNode* node = parse_program();
    expect(END_OF_FILE, 1);
    return node;
}

InstructionNode* parser::parse_program() {
    parse_var_section();
    InstructionNode* node = parse_body();
    parse_inputs();
    return node;
}

void parser::parse_var_section() {
    parse_id_list();
    expect(SEMICOLON, 2);
}

void parser::parse_id_list() {
    Token variable = expect(ID, 3);
    locationTable[variable.lexeme] = next_available;
    mem[next_available] = 0;
    next_available++;
    Token t = lexer.peek(1);
    if (t.token_type == COMMA) {
        expect(COMMA, 4);
        parse_id_list();
    }
}

InstructionNode* parser::parse_body() {
    expect(LBRACE, 5);
    InstructionNode* iList = parse_stmt_list();
    expect(RBRACE, 6);
    return iList;
}

InstructionNode* parser::parse_stmt_list() {
    InstructionNode* i1;
    InstructionNode* i2;
    i1 = parse_stmt();
    Token t = lexer.peek(1);
    if (t.token_type != RBRACE) {
        i2 = parse_stmt_list();
        InstructionNode* final = i1;
        while (final->next) {
            final = final->next;
        }
        final->next = i2;
    }
    return i1;
}

InstructionNode* parser::parse_stmt() {
    Token t = lexer.peek(1);
    if (t.token_type == ID) {
        return parse_assign_stmt();
    } else if (t.token_type == WHILE) {
        return parse_while_stmt();
    } else if (t.token_type == IF) {
        return parse_if_stmt();
    } else if (t.token_type == SWITCH) {
        return parse_switch_stmt();
    } else if (t.token_type == FOR) {
        return parse_for_stmt();
    } else if (t.token_type == OUTPUT) {
        return parse_output_stmt();
    } else if (t.token_type == INPUT) {
        return parse_input_stmt();
    }
}

InstructionNode* parser::parse_assign_stmt() {
    Token leftSide = expect(ID, 7);
    expect(EQUAL, 8);
    Token t = lexer.peek(2);
    auto* node = new InstructionNode;
    node->next = nullptr;
    node->type = ASSIGN;
    node->assign_inst.left_hand_side_index = locationTable[leftSide.lexeme];
    if (t.token_type == PLUS || t.token_type == MINUS || t.token_type == DIV || t.token_type == MULT) {
        parse_expr(node);
        expect(SEMICOLON, 9);
    } else {
        int number = parse_primary();
        expect(SEMICOLON, 9);
        node->assign_inst.op = OPERATOR_NONE;
        node->assign_inst.opernd1_index = number;
    }
    return node;
}

void parser::parse_expr(InstructionNode* node) {
    int first = parse_primary();
    ArithmeticOperatorType op = parse_op();
    int second = parse_primary();
    node->assign_inst.op = op;
    node->assign_inst.opernd1_index = first;
    node->assign_inst.opernd2_index = second;
}

int parser::parse_primary() {
    Token t = lexer.peek(1);
    if (t.token_type == ID) {
        Token variable = expect(ID, 10);
        return locationTable[variable.lexeme];
    } else {
        Token number = expect(NUM, 11);
        if (locationTable.count(number.lexeme) == 0) {
            locationTable[number.lexeme] = next_available;
            mem[next_available] = stoi(number.lexeme);
            next_available++;
        }
        return locationTable[number.lexeme];
    }
}

ArithmeticOperatorType parser::parse_op() {
    Token t = lexer.peek(1);
    if (t.token_type == MINUS) {
        expect(MINUS, 12);
        return OPERATOR_MINUS;
    } else if (t.token_type == PLUS) {
        expect(PLUS, 13);
        return OPERATOR_PLUS;
    } else if (t.token_type == DIV) {
        expect(DIV, 14);
        return OPERATOR_DIV;
    } else if (t.token_type == MULT) {
        expect(MULT, 15);
        return OPERATOR_MULT;
    }
}

InstructionNode* parser::parse_input_stmt() {
    expect(INPUT, 16);
    Token t = expect(ID, 17);
    auto* node = new InstructionNode;
    node->next = nullptr;
    node->type = IN;
    node->input_inst.var_index = locationTable[t.lexeme];
    expect(SEMICOLON, 18);
    return node;
}

InstructionNode* parser::parse_output_stmt() {
    expect(OUTPUT, 19);
    Token variable = expect(ID, 20);
    auto* node = new InstructionNode;
    node->next = nullptr;
    node->type = OUT;
    node->output_inst.var_index = locationTable[variable.lexeme];
    expect(SEMICOLON, 21);
    return node;
}

InstructionNode* parser::parse_while_stmt() {
    auto* node = new InstructionNode;
    node->type = CJMP;
    expect(WHILE, 22);
    auto conditional = parse_condition();
    node->cjmp_inst.condition_op = get<1>(conditional);
    node->cjmp_inst.opernd1_index = get<0>(conditional);
    node->cjmp_inst.opernd2_index = get<2>(conditional);
    InstructionNode* next = parse_body();
    node->next = next;
    auto* jump = new InstructionNode;
    jump->type = JMP;
    jump->jmp_inst.target = node;
    auto* noop = new InstructionNode;
    noop->type = NOOP;
    noop->next = nullptr;
    node->cjmp_inst.target = noop;
    jump->next = noop;
    InstructionNode* final = next;
    while (final->next) {
        final = final->next;
    }
    final->next = jump;
    return node;
}

InstructionNode* parser::parse_if_stmt() {
    expect(IF, 23);
    auto* node = new InstructionNode;
    node->type = CJMP;
    auto conditional = parse_condition();
    node->cjmp_inst.condition_op = get<1>(conditional);
    node->cjmp_inst.opernd1_index = get<0>(conditional);
    node->cjmp_inst.opernd2_index = get<2>(conditional);
    InstructionNode* next = parse_body();
    node->next = next;
    auto* noop = new InstructionNode;
    noop->type = NOOP;
    noop->next = nullptr;
    node->cjmp_inst.target = noop;
    InstructionNode* final = next;
    while (final->next) {
        final = final->next;
    }
    final->next = noop;
    return node;
}

tuple<int, ConditionalOperatorType, int> parser::parse_condition() {
    int first = parse_primary();
    ConditionalOperatorType op = parse_relop();
    int second = parse_primary();
    return make_tuple(first, op, second);
}

ConditionalOperatorType parser::parse_relop() {
    Token t = lexer.peek(1);
    if (t.token_type == GREATER) {
        expect(GREATER, 24);
        return CONDITION_GREATER;
    } else if (t.token_type == LESS) {
        expect(LESS, 25);
        return CONDITION_LESS;
    } else if (t.token_type == NOTEQUAL) {
        expect(NOTEQUAL, 26);
        return CONDITION_NOTEQUAL;
    }
}

InstructionNode* parser::parse_switch_stmt() {
    auto* noop = new InstructionNode;
    noop->type = NOOP;
    noop->next = nullptr;
    expect(SWITCH, 27);
    Token switchVariable = expect(ID, 28);
    expect(LBRACE, 29);
    auto* allCases = parse_case_list(noop, locationTable[switchVariable.lexeme]);
    Token t = lexer.peek(1);
    if (t.token_type == DEFAULT) {
        auto* defaultBody = parse_default_case();
        InstructionNode* final = allCases;
        if (final->type == CJMP && final->cjmp_inst.target == nullptr) {
            final->cjmp_inst.target = defaultBody;
            final->next->cjmp_inst.target = defaultBody;
        }
        while (final->next) {
            if (final->type == CJMP && final->cjmp_inst.target == nullptr) {
                final->cjmp_inst.target = defaultBody;
                final->next->cjmp_inst.target = defaultBody;
            }
            final = final->next;
        }
        final->next = defaultBody;
        defaultBody->next = noop;
    } else {
        InstructionNode* final = allCases;
        if (final->type == CJMP && final->cjmp_inst.target == nullptr) {
            final->cjmp_inst.target = noop;
            final->next->cjmp_inst.target = noop;
        }
        while (final->next) {
            if (final->type == CJMP && final->cjmp_inst.target == nullptr) {
                final->cjmp_inst.target = noop;
                final->next->cjmp_inst.target = noop;
            }
            final = final->next;
        }
        final->next = noop;
    }
    expect(RBRACE, 30);
    return allCases;
}

InstructionNode* parser::parse_for_stmt() {
    auto* node = new InstructionNode;
    node->type = CJMP;
    expect(FOR, 31);
    expect(LPAREN, 32);
    InstructionNode* assn1 = parse_assign_stmt();
    assn1->next = node;
    auto conditional = parse_condition();
    node->cjmp_inst.condition_op = get<1>(conditional);
    node->cjmp_inst.opernd1_index = get<0>(conditional);
    node->cjmp_inst.opernd2_index = get<2>(conditional);
    expect(SEMICOLON, 33);
    InstructionNode* assn2 = parse_assign_stmt();
    expect(RPAREN, 34);
    InstructionNode* next = parse_body();
    node->next = next;
    auto* jump = new InstructionNode;
    jump->type = JMP;
    jump->jmp_inst.target = node;
    auto* noop = new InstructionNode;
    noop->type = NOOP;
    noop->next = nullptr;
    node->cjmp_inst.target = noop;
    jump->next = noop;
    InstructionNode* final = next;
    while (final->next) {
        final = final->next;
    }
    final->next = assn2;
    assn2->next = jump;
    return assn1;
}

InstructionNode* parser::parse_case_list(InstructionNode* noop, int switchLocation) {
    auto* caseNode1 = parse_case(noop, switchLocation);
    Token t = lexer.peek(1);
    if (t.token_type != RBRACE && t.token_type != DEFAULT) {
        auto* caseNode2 = parse_case_list(noop, switchLocation);
        InstructionNode* final = caseNode1;
        while (final->next) {
            final = final->next;
        }
        caseNode1->cjmp_inst.target = caseNode2;
        caseNode1->next->cjmp_inst.target = caseNode2;
        final->next = caseNode2;
    }
    return caseNode1;
}

InstructionNode* parser::parse_case(InstructionNode* noop, int switchLocation) {
    expect(CASE, 35);
    Token number = expect(NUM, 36);
    expect(COLON, 37);

    // get greater and smaller constant to compare in cjmp
    int theSmallerNumber = stoi(number.lexeme) - 1;
    int theBiggerNumber = stoi(number.lexeme) + 1;
    if (locationTable.count(to_string(theSmallerNumber)) == 0) {
        locationTable[to_string(theSmallerNumber)] = next_available;
        mem[next_available] = theSmallerNumber;
        next_available++;
    }
    if (locationTable.count(to_string(theBiggerNumber)) == 0) {
        locationTable[to_string(theBiggerNumber)] = next_available;
        mem[next_available] = theBiggerNumber;
        next_available++;
    }

    // create jmp to send to label after success match
    auto* jmp = new InstructionNode;
    jmp->type = JMP;
    jmp->jmp_inst.target = noop;
    jmp->next = nullptr;

    // create outer if cjmp based on greater value
    auto* greaterCJmp = new InstructionNode;
    greaterCJmp->type = CJMP;
    greaterCJmp->cjmp_inst.condition_op = CONDITION_LESS;
    greaterCJmp->cjmp_inst.opernd1_index = switchLocation;
    greaterCJmp->cjmp_inst.opernd2_index = locationTable[to_string(theBiggerNumber)];
    greaterCJmp->cjmp_inst.target = nullptr;

    // create inner if cjmp based on lesser value
    auto* lesserCJmp = new InstructionNode;
    greaterCJmp->next = lesserCJmp;
    lesserCJmp->type = CJMP;
    lesserCJmp->cjmp_inst.condition_op = CONDITION_GREATER;
    lesserCJmp->cjmp_inst.opernd1_index = switchLocation;
    lesserCJmp->cjmp_inst.opernd2_index = locationTable[to_string(theSmallerNumber)];
    lesserCJmp->cjmp_inst.target = nullptr;

    // set up body of case
    auto* body = parse_body();
    lesserCJmp->next = body;
    InstructionNode* final = body;
    while (final->next) {
        final = final->next;
    }
    final->next = jmp;
    return greaterCJmp;
}

InstructionNode* parser::parse_default_case() {
    expect(DEFAULT, 38);
    expect(COLON, 39);
    return parse_body();
}

void parser::parse_inputs() {
    parse_num_list();
}

void parser::parse_num_list() {
    Token number = expect(NUM, 40);
    inputs.push_back(stoi(number.lexeme));
    Token t = lexer.peek(1);
    if (t.token_type == NUM) {
        parse_num_list();
    }
}
