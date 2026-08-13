#include "scripting/OMParser.h"
#include <stdexcept>
#include <cctype>

namespace Omega {

Lexer::Lexer(const std::string& source) 
    : source(source), start(0), current(0), line(1), column(1) {}

char Lexer::advance() {
    char c = source[current++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.size()) return '\0';
    return source[current + 1];
}

bool Lexer::isAtEnd() {
    return current >= source.size();
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    advance();
    return true;
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }
    
    if (isAtEnd()) {
        tokens.push_back({TokenType::Error, "Unterminated string", line, column});
        return;
    }
    
    advance(); // closing "
    std::string value = source.substr(start + 1, current - start - 2);
    tokens.push_back({TokenType::String, value, line, column});
}

void Lexer::number() {
    while (isDigit(peek())) advance();
    
    if (peek() == '.' && isDigit(peekNext())) {
        advance(); // consume .
        while (isDigit(peek())) advance();
    }
    
    std::string lexeme = source.substr(start, current - start);
    TokenType type = (lexeme.find('.') != std::string::npos) ? TokenType::Number : TokenType::Integer;
    tokens.push_back({type, lexeme, line, column});
}

void Lexer::identifier() {
    while (isAlphaNumeric(peek())) advance();
    
    std::string text = source.substr(start, current - start);
    
    static std::unordered_map<std::string, TokenType> keywords = {
        {"var", TokenType::Var},
        {"func", TokenType::Func},
        {"if", TokenType::If},
        {"else", TokenType::Else},
        {"while", TokenType::While},
        {"for", TokenType::For},
        {"return", TokenType::Return},
        {"true", TokenType::True},
        {"false", TokenType::False},
        {"null", TokenType::Null},
        {"and", TokenType::And},
        {"or", TokenType::Or},
        {"not", TokenType::Not}
    };
    
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        tokens.push_back({it->second, text, line, column});
    } else {
        tokens.push_back({TokenType::Identifier, text, line, column});
    }
}

void Lexer::scanToken() {
    char c = advance();
    
    switch (c) {
        case '(': tokens.push_back({TokenType::LeftParen, "(", line, column}); break;
        case ')': tokens.push_back({TokenType::RightParen, ")", line, column}); break;
        case '{': tokens.push_back({TokenType::LeftBrace, "{", line, column}); break;
        case '}': tokens.push_back({TokenType::RightBrace, "}", line, column}); break;
        case '[': tokens.push_back({TokenType::LeftBracket, "[", line, column}); break;
        case ']': tokens.push_back({TokenType::RightBracket, "]", line, column}); break;
        case ',': tokens.push_back({TokenType::Comma, ",", line, column}); break;
        case '.': tokens.push_back({TokenType::Dot, ".", line, column}); break;
        case ';': tokens.push_back({TokenType::Semicolon, ";", line, column}); break;
        
        case '+': tokens.push_back({TokenType::Plus, "+", line, column}); break;
        case '-': tokens.push_back({TokenType::Minus, "-", line, column}); break;
        case '*': tokens.push_back({TokenType::Star, "*", line, column}); break;
        case '/':
            if (match('/')) {
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) {
                while (!isAtEnd() && !(peek() == '*' && peekNext() == '/')) {
                    if (peek() == '\n') line++;
                    advance();
                }
                if (!isAtEnd()) {
                    advance(); // *
                    advance(); // /
                }
            } else {
                tokens.push_back({TokenType::Slash, "/", line, column});
            }
            break;
        case '%': tokens.push_back({TokenType::Percent, "%", line, column}); break;
        
        case '=':
            tokens.push_back({match('=') ? TokenType::EqualEqual : TokenType::Equal, 
                             match('=') ? "==" : "=", line, column});
            break;
        case '!':
            tokens.push_back({match('=') ? TokenType::BangEqual : TokenType::Not,
                             match('=') ? "!=" : "!", line, column});
            break;
        case '<':
            tokens.push_back({match('=') ? TokenType::LessEqual : TokenType::Less,
                             match('=') ? "<=" : "<", line, column});
            break;
        case '>':
            tokens.push_back({match('=') ? TokenType::GreaterEqual : TokenType::Greater,
                             match('=') ? ">=" : ">", line, column});
            break;
        
        case '"': string(); break;
        
        default:
            if (isDigit(c)) {
                number();
            } else if (isAlpha(c)) {
                identifier();
            } else if (c == ' ' || c == '\r' || c == '\t') {
                // skip whitespace
            } else {
                tokens.push_back({TokenType::Error, std::string("Unexpected character: ") + c, line, column});
            }
            break;
    }
}

std::vector<Token> Lexer::tokenize() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    
    tokens.push_back({TokenType::Eof, "", line, column});
    return tokens;
}

} // namespace Omega
