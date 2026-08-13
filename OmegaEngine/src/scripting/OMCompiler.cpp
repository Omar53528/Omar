#include "scripting/OMCompiler.h"
#include <stdexcept>

namespace Omega {

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

bool Parser::isAtEnd() {
    return peek().type == TokenType::Eof;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::peek() {
    return tokens[current];
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

std::shared_ptr<ProgramNode> Parser::parse() {
    auto program = std::make_shared<ProgramNode>();
    program->line = 1;
    program->column = 1;
    
    while (!isAtEnd()) {
        auto decl = declaration();
        if (decl) {
            program->statements.push_back(decl);
        }
    }
    
    return program;
}

std::shared_ptr<ASTNode> Parser::declaration() {
    try {
        if (match(TokenType::Func)) return function("function");
        if (match(TokenType::Var)) return varDeclaration();
        return statement();
    } catch (...) {
        synchronize();
        return nullptr;
    }
}

std::shared_ptr<ASTNode> Parser::function(const std::string& kind) {
    auto func = std::make_shared<FunctionDeclNode>();
    func->line = previous().line;
    func->column = previous().column;
    
    if (!check(TokenType::Identifier)) {
        throw std::runtime_error("Expect " + kind + " name");
    }
    
    Token nameToken = advance();
    func->name = nameToken.lexeme;
    
    if (!match(TokenType::LeftParen)) {
        throw std::runtime_error("Expect '(' after " + kind + " name");
    }
    
    if (!check(TokenType::RightParen)) {
        do {
            if (func->params.size() >= 255) {
                throw std::runtime_error("Cannot have more than 255 parameters");
            }
            
            if (!check(TokenType::Identifier)) {
                throw std::runtime_error("Expect parameter name");
            }
            
            Token param = advance();
            func->params.push_back(param.lexeme);
        } while (match(TokenType::Comma));
    }
    
    if (!match(TokenType::RightParen)) {
        throw std::runtime_error("Expect ')' after parameters");
    }
    
    if (!match(TokenType::LeftBrace)) {
        throw std::runtime_error("Expect '{' before " + kind + " body");
    }
    
    // Parse body as a block
    auto block = std::make_shared<BlockNode>();
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        auto stmt = declaration();
        if (stmt) {
            block->statements.push_back(stmt);
        }
    }
    
    if (!match(TokenType::RightBrace)) {
        throw std::runtime_error("Expect '}' after " + kind + " body");
    }
    
    func->body = block;
    return func;
}

std::shared_ptr<ASTNode> Parser::varDeclaration() {
    auto var = std::make_shared<VariableDeclNode>();
    var->line = previous().line;
    var->column = previous().column;
    
    if (!check(TokenType::Identifier)) {
        throw std::runtime_error("Expect variable name");
    }
    
    Token nameToken = advance();
    var->name = nameToken.lexeme;
    
    if (match(TokenType::Equal)) {
        var->initializer = expression();
    }
    
    if (!match(TokenType::Semicolon)) {
        throw std::runtime_error("Expect ';' after variable declaration");
    }
    
    return var;
}

std::shared_ptr<ASTNode> Parser::statement() {
    if (match(TokenType::If)) {
        auto ifStmt = std::make_shared<IfStatementNode>();
        ifStmt->line = previous().line;
        ifStmt->column = previous().column;
        
        if (!match(TokenType::LeftParen)) {
            throw std::runtime_error("Expect '(' after 'if'");
        }
        
        ifStmt->condition = expression();
        
        if (!match(TokenType::RightParen)) {
            throw std::runtime_error("Expect ')' after if condition");
        }
        
        ifStmt->thenBranch = statement();
        
        if (match(TokenType::Else)) {
            ifStmt->elseBranch = statement();
        }
        
        return ifStmt;
    }
    
    if (match(TokenType::While)) {
        auto whileStmt = std::make_shared<WhileStatementNode>();
        whileStmt->line = previous().line;
        whileStmt->column = previous().column;
        
        if (!match(TokenType::LeftParen)) {
            throw std::runtime_error("Expect '(' after 'while'");
        }
        
        whileStmt->condition = expression();
        
        if (!match(TokenType::RightParen)) {
            throw std::runtime_error("Expect ')' after while condition");
        }
        
        whileStmt->body = statement();
        return whileStmt;
    }
    
    if (match(TokenType::For)) {
        auto forStmt = std::make_shared<ForStatementNode>();
        forStmt->line = previous().line;
        forStmt->column = previous().column;
        
        if (!match(TokenType::LeftParen)) {
            throw std::runtime_error("Expect '(' after 'for'");
        }
        
        if (match(TokenType::Semicolon)) {
            forStmt->initializer = nullptr;
        } else if (match(TokenType::Var)) {
            forStmt->initializer = varDeclaration();
        } else {
            forStmt->initializer = expression();
            if (!match(TokenType::Semicolon)) {
                throw std::runtime_error("Expect ';' after for initializer");
            }
        }
        
        if (!check(TokenType::Semicolon)) {
            forStmt->condition = expression();
        }
        
        if (!match(TokenType::Semicolon)) {
            throw std::runtime_error("Expect ';' after for condition");
        }
        
        if (!check(TokenType::RightParen)) {
            forStmt->increment = expression();
        }
        
        if (!match(TokenType::RightParen)) {
            throw std::runtime_error("Expect ')' after for clauses");
        }
        
        forStmt->body = statement();
        return forStmt;
    }
    
    if (match(TokenType::Return)) {
        auto returnStmt = std::make_shared<ReturnStatementNode>();
        returnStmt->line = previous().line;
        returnStmt->column = previous().column;
        
        if (!check(TokenType::Semicolon)) {
            returnStmt->value = expression();
        }
        
        if (!match(TokenType::Semicolon)) {
            throw std::runtime_error("Expect ';' after return value");
        }
        
        return returnStmt;
    }
    
    if (match(TokenType::LeftBrace)) {
        auto block = std::make_shared<BlockNode>();
        while (!check(TokenType::RightBrace) && !isAtEnd()) {
            auto decl = declaration();
            if (decl) {
                block->statements.push_back(decl);
            }
        }
        
        if (!match(TokenType::RightBrace)) {
            throw std::runtime_error("Expect '}' after block");
        }
        
        return block;
    }
    
    // Expression statement
    auto exprStmt = std::make_shared<ExpressionStatementNode>();
    exprStmt->expression = expression();
    
    if (!match(TokenType::Semicolon)) {
        throw std::runtime_error("Expect ';' after expression");
    }
    
    return exprStmt;
}

std::shared_ptr<ASTNode> Parser::expression() {
    return equality();
}

std::shared_ptr<ASTNode> Parser::equality() {
    auto left = comparison();
    
    while (match(TokenType::BangEqual) || match(TokenType::EqualEqual)) {
        Token op = previous();
        auto right = comparison();
        
        auto binary = std::make_shared<BinaryOpNode>();
        binary->line = op.line;
        binary->column = op.column;
        binary->op = op.type;
        binary->left = left;
        binary->right = right;
        
        left = binary;
    }
    
    return left;
}

std::shared_ptr<ASTNode> Parser::comparison() {
    auto left = term();
    
    while (match(TokenType::Less) || match(TokenType::LessEqual) ||
           match(TokenType::Greater) || match(TokenType::GreaterEqual)) {
        Token op = previous();
        auto right = term();
        
        auto binary = std::make_shared<BinaryOpNode>();
        binary->line = op.line;
        binary->column = op.column;
        binary->op = op.type;
        binary->left = left;
        binary->right = right;
        
        left = binary;
    }
    
    return left;
}

std::shared_ptr<ASTNode> Parser::term() {
    auto left = factor();
    
    while (match(TokenType::Plus) || match(TokenType::Minus)) {
        Token op = previous();
        auto right = factor();
        
        auto binary = std::make_shared<BinaryOpNode>();
        binary->line = op.line;
        binary->column = op.column;
        binary->op = op.type;
        binary->left = left;
        binary->right = right;
        
        left = binary;
    }
    
    return left;
}

std::shared_ptr<ASTNode> Parser::factor() {
    auto left = unary();
    
    while (match(TokenType::Star) || match(TokenType::Slash)) {
        Token op = previous();
        auto right = unary();
        
        auto binary = std::make_shared<BinaryOpNode>();
        binary->line = op.line;
        binary->column = op.column;
        binary->op = op.type;
        binary->left = left;
        binary->right = right;
        
        left = binary;
    }
    
    return left;
}

std::shared_ptr<ASTNode> Parser::unary() {
    if (match(TokenType::Not) || match(TokenType::Minus)) {
        Token op = previous();
        auto operand = unary();
        
        auto unaryNode = std::make_shared<UnaryOpNode>();
        unaryNode->line = op.line;
        unaryNode->column = op.column;
        unaryNode->op = op.type;
        unaryNode->operand = operand;
        
        return unaryNode;
    }
    
    return call();
}

std::shared_ptr<ASTNode> Parser::call() {
    auto expr = primary();
    
    while (true) {
        if (match(TokenType::LeftParen)) {
            auto callNode = std::make_shared<CallNode>();
            callNode->callee = expr;
            
            if (!check(TokenType::RightParen)) {
                do {
                    callNode->arguments.push_back(expression());
                } while (match(TokenType::Comma));
            }
            
            if (!match(TokenType::RightParen)) {
                throw std::runtime_error("Expect ')' after arguments");
            }
            
            expr = callNode;
        } else {
            break;
        }
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::primary() {
    if (match(TokenType::True)) {
        auto literal = std::make_shared<LiteralNode>();
        literal->value = true;
        return literal;
    }
    
    if (match(TokenType::False)) {
        auto literal = std::make_shared<LiteralNode>();
        literal->value = false;
        return literal;
    }
    
    if (match(TokenType::Null)) {
        auto literal = std::make_shared<LiteralNode>();
        literal->value = nullptr;
        return literal;
    }
    
    if (match(TokenType::Number) || match(TokenType::Integer)) {
        auto literal = std::make_shared<LiteralNode>();
        literal->value = std::stod(previous().lexeme);
        return literal;
    }
    
    if (match(TokenType::String)) {
        auto literal = std::make_shared<LiteralNode>();
        literal->value = previous().lexeme;
        return literal;
    }
    
    if (match(TokenType::Identifier)) {
        auto ident = std::make_shared<IdentifierNode>();
        ident->name = previous().lexeme;
        return ident;
    }
    
    if (match(TokenType::LeftParen)) {
        auto expr = expression();
        if (!match(TokenType::RightParen)) {
            throw std::runtime_error("Expect ')' after expression");
        }
        return expr;
    }
    
    throw std::runtime_error("Expect expression");
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::Semicolon) return;
        
        switch (peek().type) {
            case TokenType::Func:
            case TokenType::Var:
            case TokenType::If:
            case TokenType::While:
            case TokenType::For:
            case TokenType::Return:
                return;
            default:
                break;
        }
        
        advance();
    }
}

} // namespace Omega
