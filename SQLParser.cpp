#include "SQLParser.h"

#include <iostream>

SQLParser::SQLParser(SQLScanner* sc) : scanner(sc) {}

SQLStatement* SQLParser::parseSQLStatement() {
    current = scanner->nextToken();

    if (match(SQLToken::CREATE)) {
        return parseCreateTable();
    }
    else if (match(SQLToken::SELECT)) {
        return parseSelect();
    }
    else if (match(SQLToken::INSERT)) {
        return parseInsert();
    }
    else if (match(SQLToken::DELETE)) {
        return parseDelete();
    }
    else if (scanner->currentToken->type == SQLToken::END) {
        return new EndStatement();
    }
    else {
        std::cout << "Error: Unexpected SQL statement" << std::endl;
        return new EndStatement();
    }
}

bool SQLParser::match(SQLToken::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool SQLParser::check(SQLToken::Type ttype) {
    if (isAtEnd()) {
        return false;
    }
    return scanner->currentToken->type == ttype;
}

bool SQLParser::advance() {
    if (!isAtEnd()) {
        SQLToken* temp = scanner->currentToken;
        scanner->currentToken = scanner->nextToken();
        delete temp;
        return true;
    }
    return false;
}

bool SQLParser::isAtEnd() {
    return (scanner->currentToken->type == SQLToken::END);
}

CreateTableStatement* SQLParser::parseCreateTable() {
    bool is;
    is = match(SQLToken::TABLE);
    std::string tableName = scanner->currentToken->lexema;
    advance();

    is = match(SQLToken::FROM);
    is = match(SQLToken::FILE);

    std::string fileName = scanner->currentToken->lexema;
    is = match(SQLToken::STRING);

    is = match(SQLToken::USING);

    is = match(SQLToken::INDEX);
    std::string indexType = scanner->currentToken->lexema;
    advance();

    is = match(SQLToken::LPAREN);

    std::string indexedColumn = scanner->currentToken->lexema;
    is = match(SQLToken::STRING);

    is = match(SQLToken::RPAREN);

    is = match(SQLToken::SEMICOLON);

    return new CreateTableStatement(tableName, fileName, indexType, indexedColumn);
}

std::vector<ColumnDefinition> SQLParser::parseColumnList() {
    std::vector<ColumnDefinition> columns;
    columns.push_back(parseColumnDefinition());

    while (match(SQLToken::COMMA)) {
        columns.push_back(parseColumnDefinition());
    }

    return columns;
}

ColumnDefinition SQLParser::parseColumnDefinition() {
    std::string columnName = scanner->currentToken->lexema;
    advance();
    match(SQLToken::ID);
    std::string dataType = scanner->currentToken->lexema;
    advance();
    match(SQLToken::ID);

    return ColumnDefinition(columnName, dataType);
}

SelectStatement* SQLParser::parseSelect() {
    // std::string columnName = scanner->currentToken->lexema;
    match(SQLToken::STAR);

    match(SQLToken::FROM);

    std::string tableName = scanner->currentToken->lexema;
    advance();

    match(SQLToken::WHERE);

    Condition* condition = parseCondition();

    match(SQLToken::SEMICOLON);

    return new SelectStatement(tableName, condition);
}

Condition* SQLParser::parseCondition() {
    std::string columnName = scanner->currentToken->lexema;
    advance();

    if (match(SQLToken::EQUALS)) {
        std::string value = scanner->currentToken->lexema;
        advance();

        return new ConditionEquals(columnName, value);
    }
    else if (match(SQLToken::BETWEEN)) {
        std::string left = scanner->currentToken->lexema;
        advance();

        match(SQLToken::AND);

        std::string right = scanner->currentToken->lexema;
        advance();

        return new ConditionBetween(columnName, left, right);
    }

    return nullptr;
}

InsertStatement* SQLParser::parseInsert() {
    match(SQLToken::INTO);

    std::string tableName = scanner->currentToken->lexema;
    advance();

    match(SQLToken::VALUES);
    match(SQLToken::LPAREN);
    std::vector<std::string> values = parseValueList();
    match(SQLToken::RPAREN);

    return new InsertStatement(tableName, values);
}

std::vector<std::string> SQLParser::parseValueList() {
    std::vector<std::string> values;

    values.push_back(scanner->currentToken->lexema);
    advance();

    while (match(SQLToken::COMMA)) {
        values.push_back(scanner->currentToken->lexema);
        advance();
    }

    return values;
}

DeleteStatement* SQLParser::parseDelete() {
    match(SQLToken::FROM);

    std::string table_name = scanner->currentToken->lexema;
    advance();

    match(SQLToken::WHERE);

    Condition* condition = parseCondition();

    match(SQLToken::SEMICOLON);

    return new DeleteStatement(table_name, condition);
}
