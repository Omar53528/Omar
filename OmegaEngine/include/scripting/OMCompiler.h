#pragma once

#include "scripting/OMParser.h"
#include <vector>
#include <memory>
#include <any>
#include <functional>

namespace Omega {

// AST Node types
enum class NodeType {
    Program,
    FunctionDecl,
    VariableDecl,
    Assignment,
    BinaryOp,
    UnaryOp,
    Call,
    Literal,
    Identifier,
    Block,
    IfStatement,
    WhileStatement,
    ForStatement,
    ReturnStatement,
    ExpressionStatement
};

struct ASTNode {
    NodeType type;
    int line;
    int column;
    virtual ~ASTNode() = default;
};

struct LiteralNode : ASTNode {
    std::any value;
    LiteralNode() : type(NodeType::Literal) {}
};

struct IdentifierNode : ASTNode {
    std::string name;
    IdentifierNode() : type(NodeType::Identifier) {}
};

struct BinaryOpNode : ASTNode {
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;
    TokenType op;
    BinaryOpNode() : type(NodeType::BinaryOp) {}
};

struct UnaryOpNode : ASTNode {
    std::shared_ptr<ASTNode> operand;
    TokenType op;
    UnaryOpNode() : type(NodeType::UnaryOp) {}
};

struct CallNode : ASTNode {
    std::shared_ptr<ASTNode> callee;
    std::vector<std::shared_ptr<ASTNode>> arguments;
    CallNode() : type(NodeType::Call) {}
};

struct VariableDeclNode : ASTNode {
    std::string name;
    std::shared_ptr<ASTNode> initializer;
    VariableDeclNode() : type(NodeType::VariableDecl) {}
};

struct AssignmentNode : ASTNode {
    std::string name;
    std::shared_ptr<ASTNode> value;
    AssignmentNode() : type(NodeType::Assignment) {}
};

struct FunctionDeclNode : ASTNode {
    std::string name;
    std::vector<std::string> params;
    std::shared_ptr<ASTNode> body;
    FunctionDeclNode() : type(NodeType::FunctionDecl) {}
};

struct BlockNode : ASTNode {
    std::vector<std::shared_ptr<ASTNode>> statements;
    BlockNode() : type(NodeType::Block) {}
};

struct IfStatementNode : ASTNode {
    std::shared_ptr<ASTNode> condition;
    std::shared_ptr<ASTNode> thenBranch;
    std::shared_ptr<ASTNode> elseBranch;
    IfStatementNode() : type(NodeType::IfStatement) {}
};

struct WhileStatementNode : ASTNode {
    std::shared_ptr<ASTNode> condition;
    std::shared_ptr<ASTNode> body;
    WhileStatementNode() : type(NodeType::WhileStatement) {}
};

struct ForStatementNode : ASTNode {
    std::shared_ptr<ASTNode> initializer;
    std::shared_ptr<ASTNode> condition;
    std::shared_ptr<ASTNode> increment;
    std::shared_ptr<ASTNode> body;
    ForStatementNode() : type(NodeType::ForStatement) {}
};

struct ReturnStatementNode : ASTNode {
    std::shared_ptr<ASTNode> value;
    ReturnStatementNode() : type(NodeType::ReturnStatement) {}
};

struct ExpressionStatementNode : ASTNode {
    std::shared_ptr<ASTNode> expression;
    ExpressionStatementNode() : type(NodeType::ExpressionStatement) {}
};

struct ProgramNode : ASTNode {
    std::vector<std::shared_ptr<ASTNode>> statements;
    ProgramNode() : type(NodeType::Program) {}
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::shared_ptr<ProgramNode> parse();
    
private:
    std::shared_ptr<ASTNode> declaration();
    std::shared_ptr<ASTNode> function(const std::string& kind);
    std::shared_ptr<ASTNode> varDeclaration();
    std::shared_ptr<ASTNode> statement();
    std::shared_ptr<ASTNode> expression();
    std::shared_ptr<ASTNode> equality();
    std::shared_ptr<ASTNode> comparison();
    std::shared_ptr<ASTNode> term();
    std::shared_ptr<ASTNode> factor();
    std::shared_ptr<ASTNode> unary();
    std::shared_ptr<ASTNode> call();
    std::shared_ptr<ASTNode> primary();
    
    void synchronize();
    bool match(TokenType type);
    bool check(TokenType type);
    Token advance();
    Token previous();
    Token peek();
    bool isAtEnd();
    
    std::vector<Token> tokens;
    size_t current;
};

} // namespace Omega
