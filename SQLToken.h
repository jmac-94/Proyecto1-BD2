#ifndef SQLTOKEN_H
#define SQLTOKEN_H

#include <string>
#include <unordered_map>

class SQLToken {
public:
    enum Type {
        CREATE,
        SELECT,
        INSERT,
        DELETE,
        INTO,
        FROM,
        WHERE,
        VALUES,
        AND,
        BETWEEN,
        TABLE,
        STAR,
        EQUALS,
        USING,
        COMMA,
        SEMICOLON,
        LPAREN,
        RPAREN,
        STRING,
        NUM,
        ID,
        FILE,
        INDEX,
        ERR,
        END
    };
    static const char* token_names[25];
    Type type;
    std::string lexema;
    SQLToken(Type);
    SQLToken(Type, const std::string& source);
};

#endif // SQLTOKEN_H
