#ifndef SQLPARSER_H
#define SQLPARSER_H

#include "SQLScanner.h"
#include "SQLStatement.h"

class SQLParser {
public:
    SQLParser(SQLScanner* sc);
    SQLStatement* parseSQLStatement();

private:
    SQLScanner* scanner;
    SQLToken* current;

    bool match(SQLToken::Type ttype);
    bool check(SQLToken::Type ttype);
    bool advance();
    bool isAtEnd();

    CreateTableStatement* parseCreateTable();
    std::vector<ColumnDefinition> parseColumnList();
    ColumnDefinition parseColumnDefinition();
    SelectStatement* parseSelect();
    Condition* parseCondition();
    InsertStatement* parseInsert();
    std::vector<std::string> parseValueList();
    DeleteStatement* parseDelete();
};

#endif // SQLPARSER_H
