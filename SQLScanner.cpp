#include "SQLScanner.h"

#include <cctype>

SQLScanner::SQLScanner(const std::string& s) : input(s), first(0), current(0), currentToken(nullptr) {
    reserved["create"] = SQLToken::CREATE;
    reserved["select"] = SQLToken::SELECT;
    reserved["insert"] = SQLToken::INSERT;
    reserved["delete"] = SQLToken::DELETE;
    reserved["from"] = SQLToken::FROM;
    reserved["where"] = SQLToken::WHERE;
    reserved["values"] = SQLToken::VALUES;
    reserved["and"] = SQLToken::AND;
    reserved["between"] = SQLToken::BETWEEN;
    reserved["table"] = SQLToken::TABLE;
    reserved["*"] = SQLToken::STAR;
    reserved["using"] = SQLToken::USING;
    reserved["file"] = SQLToken::FILE;
    reserved["into"] = SQLToken::INTO;
    reserved["index"] = SQLToken::INDEX;
}

SQLScanner::~SQLScanner() {

}

SQLToken* SQLScanner::nextToken() {
    SQLToken* token;
    char c;
    c = nextChar();

    while (isspace(c)) {
        c = nextChar();
    }

    if (c == '\0') {
        return new SQLToken(SQLToken::END);
    }

    startLexema();

    if (isdigit(c)) {
        c = nextChar();
        while (isdigit(c)) c = nextChar();
        rollBack();

        // decimal numbers
        if (c == '.') {
            c = nextChar();
            c = nextChar();
            while (isdigit(c)) c = nextChar();
            rollBack();
        }

        token = new SQLToken(SQLToken::NUM, getLexema());
    }
    else if (isalpha(c)) {
        c = nextChar();
        while (isalpha(c) || isdigit(c) || c == '_' || c == '/') c = nextChar();
        rollBack();
        std::string lex = getLexema();
        SQLToken::Type ttype = checkReserved(lex);
        if (ttype != SQLToken::ERR) {
            token = new SQLToken(ttype);
        }
        else {
            token = new SQLToken(SQLToken::ID, getLexema());
        }
    }
    else if (strchr("()*=,;", c)) {
        switch (c) {
            case '(':
                token = new SQLToken(SQLToken::LPAREN);
                break;
            case ')':
                token = new SQLToken(SQLToken::RPAREN);
                break;
            case '*':
                token = new SQLToken(SQLToken::STAR);
                break;
            case '=':
                token = new SQLToken(SQLToken::EQUALS);
                break;
            case ',':
                token = new SQLToken(SQLToken::COMMA);
                break;
            case ';':
                token = new SQLToken(SQLToken::SEMICOLON);
                break;
            default:
                token = new SQLToken(SQLToken::ERR, getLexema());
        }
    }
    else if (c == '"') {
        c = nextChar();
        while (c != '"' && c != '\0') c = nextChar();
        if (c == '\0') token = new SQLToken(SQLToken::ERR, getLexema());
        else
            token = new SQLToken(SQLToken::STRING, getLexema());
    }
    else
        token = new SQLToken(SQLToken::ERR, getLexema());

    currentToken = token;

    return token;
}

char SQLScanner::nextChar() {
    char c = input[current];
    current++;
    return c;
}

void SQLScanner::rollBack() { current--; }
void SQLScanner::startLexema() { first = current - 1; }
std::string SQLScanner::getLexema() { return input.substr(first, current - first); }

SQLToken::Type SQLScanner::checkReserved(const std::string& lexema) {
    auto it = reserved.find(lexema);
    if (it == reserved.end()) return SQLToken::ERR;
    return it->second;
}
