#include "SQLToken.h"

const char* SQLToken::token_names[25] = {
        "CREATE", "SELECT", "INSERT", "DELETE", "INTO", "FROM", "WHERE", "VALUES", "AND", "BETWEEN", "TABLE", "STAR", "EQUALS", "USING", "COMMA", "SEMICOLON", "LPAREN", "RPAREN", "STRING", "NUM", "ID", "FILE", "INDEX", "ERR", "END"};

SQLToken::SQLToken(Type type) : type(type) { lexema = ""; }
SQLToken::SQLToken(Type type, const std::string& source) : type(type), lexema(source) {}
