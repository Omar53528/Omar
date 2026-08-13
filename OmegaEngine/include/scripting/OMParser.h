#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace Omega {

// Token types for OmegaScript
enum class TokenType {
    // Literals
    Identifier,
    String,
    Number,
    Integer,
    
    // Keywords
    Var,
    Func,
    If,
    Else,
    While,
    For,
    Return,
    True,
    False,
    Null,
    And,
    Or,
    Not,
    
    // Operators
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Equal,
    EqualEqual,
    BangEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    
    // Delimiters
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    LeftBracket,
    RightBracket,
    Comma,
    Dot,
    Semicolon,
    
    // Special
    Eof,
    Error
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
    
private:
    void scanToken();
    char advance();
    char peek();
    char peekNext();
    bool isAtEnd();
    
    void string();
    void number();
    void identifier();
    
    bool match(char expected);
    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
    
    std::string source;
    std::vector<Token> tokens;
    size_t start;
    size_t current;
    int line;
    int column;
};

} // namespace Omega
