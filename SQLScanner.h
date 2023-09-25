#ifndef SQLSCANNER_H
#define SQLSCANNER_H

#include <string>
#include <cstring>
#include <unordered_map>
#include "SQLToken.h"

class SQLScanner {
public:
    SQLScanner(const std::string& in_s);
    SQLToken* nextToken();
    ~SQLScanner();

    SQLToken* currentToken;

private:
    std::string input;
    int first, current;
    std::unordered_map<std::string, SQLToken::Type> reserved;
    char nextChar();
    void rollBack();
    void startLexema();
    std::string getLexema();
    SQLToken::Type checkReserved(const std::string& lexema);
};
#endif // SQLSCANNER_H
