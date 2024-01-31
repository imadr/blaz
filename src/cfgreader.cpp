#include "CfgReader.h"

#include "filesystem.h"
#include "logger.h"

namespace blaz {

enum class TokenType {
    TOKEN_EOF,
    COMMA,
    COLON,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    SLASH,
    BACKSLASH,
    STR_LITERAL,
    FLOAT_LITERAL,
    BOOL_LITERAL,
    IMPORT
};

struct Token {
    TokenType type = TokenType::TOKEN_EOF;
    u32 line = 0;
    u32 column = 0;
    str str_value;
    f32 float_value = 0;
    bool bool_value = false;
};

struct SingleToken {
    char c;
    TokenType type;
};

const SingleToken single_tokens[] = {
    {',', TokenType::COMMA},
    {':', TokenType::COLON},
    {'[', TokenType::LEFT_BRACKET},
    {']', TokenType::RIGHT_BRACKET},
    {'{', TokenType::LEFT_BRACE},
    {'}', TokenType::RIGHT_BRACE},
    {'/', TokenType::SLASH},
    {'\\', TokenType::BACKSLASH},
    {'(', TokenType::LEFT_PARENTHESIS},
    {')', TokenType::RIGHT_PARENTHESIS},
};

struct TokenizerState {
    str path;
    str text;
    u32 text_length;
    u32 current_index;
    vec<Token> tokens;
    u32 column;
    u32 line;
    u32 tokens_count;
    u32 tokens_capacity;
    u32 in_comment;
    Error error;
};

bool is_alphabet(char c) {
    return (c >= 65 && c <= 90) || (c >= 97 && c <= 122);
}

bool is_digit(char c) {
    return (c >= 48 && c <= 57);
}

bool is_whitespace_or_newline(char c) {
    return (c == 9) || (c == 10) || (c == 13) || (c == 32);
}

bool inside_str_literal(char c) {
    return c != '"';
}

char current_char(TokenizerState* state) {
    return state->text[state->current_index];
}

bool end_of_file(TokenizerState* state) {
    return state->current_index == state->text_length - 1;
}

void advance(TokenizerState* state, u32 steps) {
    for (u32 i = 0; i < steps; i++) {
        if (end_of_file(state)) return;
        state->current_index++;
        state->column++;
        if (current_char(state) == '\n') {
            state->line++;
            state->column = 0;
        }
    }
}

char peek_char(TokenizerState* state, u32 steps) {
    steps = std::min(steps, state->text_length - state->current_index - 1);
    return state->text[state->current_index + steps];
}

bool consume_char(TokenizerState* state, char c) {
    if (peek_char(state, 0) == c) {
        advance(state, 1);
        return true;
    } else {
        return false;
    }
}

void add_token(TokenizerState* state, Token token) {
    state->tokens.push_back(token);
}

void check_comment(TokenizerState* state) {
    if (peek_char(state, 0) == '/' && peek_char(state, 1) == '*') {
        advance(state, 2);
        state->in_comment++;
    } else if (state->in_comment > 0 && peek_char(state, 0) == '*' && peek_char(state, 1) == '/') {
        advance(state, 2);
        state->in_comment--;
    }
}

bool check_single_token(TokenizerState* state) {
    for (u32 i = 0; i < sizeof(single_tokens) / sizeof(SingleToken); i++) {
        if (end_of_file(state)) break;
        if (current_char(state) == single_tokens[i].c) {
            Token token = {.type = single_tokens[i].type,
                           .line = state->line,
                           .column = state->column,
                           .str_value = std::string(1, single_tokens[i].c)};
            add_token(state, token);
            advance(state, 1);
            return true;
        }
    }
    return false;
}

str fill(TokenizerState* state, bool (*condition)(char)) {
    u32 count = 0;
    str buffer = "";
    while (condition(peek_char(state, 1)) && !end_of_file(state)) {
        buffer += str(1, current_char(state));
        advance(state, 1);
    }
    buffer += str(1, current_char(state));
    advance(state, 1);
    return buffer;
}

TokenizerState tokenize(str text, str path) {
    TokenizerState state = {.path = path,
                            .text = text,
                            .text_length = (u32)text.length(),
                            .current_index = 0,
                            .tokens = vec<Token>(),
                            .column = 1,
                            .line = 1,
                            .tokens_count = 0,
                            .in_comment = 0,
                            .error = Error()};

    while (!end_of_file(&state)) {
        check_comment(&state);
        if (state.in_comment > 0) {
            advance(&state, 1);
            continue;
        }

        if (check_single_token(&state)) continue;

        if (is_alphabet(current_char(&state))) {
            str s = fill(&state, [](char c) { return is_alphabet(c); });
            if (s == "true") {
                add_token(&state, {.type = TokenType::BOOL_LITERAL,
                                   .line = state.line,
                                   .column = state.column,
                                   .bool_value = true});
            } else if (s == "false") {
                add_token(&state, {.type = TokenType::BOOL_LITERAL,
                                   .line = state.line,
                                   .column = state.column,
                                   .bool_value = false});
            } else if (s == "import") {
                add_token(&state,
                          {.type = TokenType::IMPORT, .line = state.line, .column = state.column});
            }
        } else if (consume_char(&state, '"')) {
            str s = fill(&state, [](char c) { return inside_str_literal(c); });
            if (consume_char(&state, '"')) {
                add_token(&state, {.type = TokenType::STR_LITERAL,
                                   .line = state.line,
                                   .column = state.column,
                                   .str_value = s});
            }
        } else if (is_digit(current_char(&state)) || current_char(&state) == '-') {
            bool negative = false;
            if (consume_char(&state, '-')) {
                negative = true;
            }
            str integer_value = fill(&state, [](char c) { return is_digit(c); });
            str decimal_value = "";
            if (consume_char(&state, '.')) {
                decimal_value = fill(&state, [](char c) { return is_digit(c); });
            }
            str float_value = integer_value + "." + decimal_value;
            add_token(&state, {.type = TokenType::FLOAT_LITERAL,
                               .line = state.line,
                               .column = state.column,
                               .float_value = std::stof(float_value) * (negative ? -1 : 1)});
        } else if (!is_whitespace_or_newline(current_char(&state)) && !end_of_file(&state)) {
            str err = "Unknown token " + str(1, current_char(&state)) + " at line " +
                      std::to_string(state.line) + " column " + std::to_string(state.column) +
                      " in file '" + state.path + "'";
            state.error = Error(err);
            break;
        } else {
            advance(&state, 1);
        }
    }

    if (state.in_comment > 0) {
        state.error = Error("Comment was never closed %d\n");
    }

    add_token(&state, {.type = TokenType::TOKEN_EOF,
                       .line = state.line,
                       .column = state.column,
                       .str_value = ""});
    return state;
}

struct ParserState {
    u32 current_token;
    vec<Token> tokens;
    CfgNode root_node;
    str path;
};

void advance(ParserState* state, u32 steps) {
    steps = std::min(steps, (u32)state->tokens.size() - state->current_token - 1);
    state->current_token += steps;
}

Token current_token(ParserState* state) {
    return state->tokens[state->current_token];
}

bool end_of_tokens(ParserState* state) {
    return state->current_token == state->tokens.size() - 1;
}

Token peek_token(ParserState* state, u32 steps) {
    steps = std::min(steps, (u32)state->tokens.size() - state->current_token - 1);
    return state->tokens[state->current_token + steps];
}

bool consume_token(ParserState* state, TokenType t, Token* token) {
    if (current_token(state).type == t) {
        if (token != NULL) *token = current_token(state);
        advance(state, 1);
        return true;
    }
    return false;
}

str error_at(ParserState* state) {
    return "at line " + std::to_string(state->tokens[state->current_token].line) + " column " +
           std::to_string(state->tokens[state->current_token].column) + " in file '" + state->path +
           "'";
}

CfgNode generic_node(ParserState* state);

CfgNode float_node(ParserState* state) {
    Token token;
    if (consume_token(state, TokenType::FLOAT_LITERAL, &token)) {
        CfgNode node;
        node.type = CfgNodeType::FLOAT;
        node.float_value = token.float_value;
        return node;
    } else {
        return CfgNode();  // @error
    }
}

CfgNode bool_node(ParserState* state) {
    Token token;
    if (consume_token(state, TokenType::BOOL_LITERAL, &token)) {
        CfgNode node;
        node.type = CfgNodeType::BOOL;
        node.bool_value = token.bool_value;
        return node;
    } else {
        return CfgNode();  // @error
    }
}

CfgNode str_node(ParserState* state) {
    Token token;
    if (consume_token(state, TokenType::STR_LITERAL, &token)) {
        CfgNode node;
        node.type = CfgNodeType::STR;
        node.str_value = token.str_value;
        return node;
    } else {
        return CfgNode();  // @error
    }
}

CfgNode map_node(ParserState* state) {
    CfgNode node;
    node.type = CfgNodeType::MAP;
    if (consume_token(state, TokenType::LEFT_BRACE, NULL)) {
        bool no_comma = false;
        while (!consume_token(state, TokenType::RIGHT_BRACE, NULL) &&
               !consume_token(state, TokenType::TOKEN_EOF, NULL)) {
            if (no_comma) {
                logger.error("Expected comma " + error_at(state));
                return CfgNode();  // @error
            }
            Token key;
            if (consume_token(state, TokenType::STR_LITERAL, &key)) {
                if (consume_token(state, TokenType::COLON, NULL)) {
                    CfgNode value = generic_node(state);
                    node.map_value[key.str_value] = value;
                    no_comma = !consume_token(state, TokenType::COMMA, NULL);
                }
            }
        }
        return node;
    } else {
        return CfgNode();  // @error
    }
}

CfgNode array_node(ParserState* state) {
    CfgNode node;
    node.type = CfgNodeType::ARRAY;
    if (consume_token(state, TokenType::LEFT_BRACKET, NULL)) {
        bool no_comma = false;
        while (!consume_token(state, TokenType::RIGHT_BRACKET, NULL) &&
               !consume_token(state, TokenType::TOKEN_EOF, NULL)) {
            if (no_comma) {
                logger.error("Expected comma " + error_at(state));
                return CfgNode();  // @error
            }

            CfgNode value = generic_node(state);
            node.array_value.push_back(value);
            no_comma = !consume_token(state, TokenType::COMMA, NULL);
        }
        return node;
    } else {
        return CfgNode();  // @error
    }
}

CfgNode import_node(ParserState* state) {
    CfgNode node;

    if (consume_token(state, TokenType::IMPORT, NULL)) {
        if (consume_token(state, TokenType::LEFT_PARENTHESIS, NULL)) {
            Token import_path;
            if (consume_token(state, TokenType::STR_LITERAL, &import_path)) {
                pair<Error, CfgNode> import = read_cfg_file(import_path.str_value);
                if (import.first) {
                    logger.error(import.first);
                    return CfgNode();
                }
                node.type = CfgNodeType::MAP;
                node.map_value = import.second.map_value;
                if (!consume_token(state, TokenType::RIGHT_PARENTHESIS, NULL)) {
                    logger.error("Expected closing parenthesis " + error_at(state));
                    return CfgNode();
                }
                return node;
            }
        }
    }
    return CfgNode();  // @error
}

CfgNode generic_node(ParserState* state) {
    Token peeked_token = peek_token(state, 0);

    switch (peeked_token.type) {
        case TokenType::LEFT_BRACE:
            return map_node(state);
            break;
        case TokenType::LEFT_BRACKET:
            return array_node(state);
            break;
        case TokenType::FLOAT_LITERAL:
            return float_node(state);
            break;
        case TokenType::STR_LITERAL:
            return str_node(state);
            break;
        case TokenType::BOOL_LITERAL:
            return bool_node(state);
            break;
        case TokenType::IMPORT:
            return import_node(state);
            break;
        default:
            return CfgNode();  // @error
            break;
    }
}

ParserState parse(vec<Token> tokens, str path) {
    ParserState state = {.tokens = tokens, .path = path};

    Token peeked_token = peek_token(&state, 0);
    switch (peeked_token.type) {
        case TokenType::LEFT_BRACE:
            state.root_node = map_node(&state);
            break;
        case TokenType::LEFT_BRACKET:
            state.root_node = array_node(&state);
            break;
        default:
            state.root_node = CfgNode();  // @error
            break;
    }

    return state;
}

pair<Error, CfgNode> read_cfg_file(str path) {
    pair<Error, str> file_content = read_whole_file(path);
    if (file_content.first) {
        return std::make_pair(file_content.first, CfgNode());
    }

    TokenizerState tokenizer_state = tokenize(file_content.second, path);
    if (tokenizer_state.error) {
        return std::make_pair(Error(tokenizer_state.error), CfgNode());
    }

    ParserState parser_state = parse(tokenizer_state.tokens, path);

    return std::make_pair(Error(), parser_state.root_node);
}

}  // namespace blaz