#ifndef SQLSTATEMENT_H
#define SQLSTATEMENT_H

#include <string>
#include <vector>
#include <unordered_map>

class SQLStatement {
public:
    SQLStatement() {}
    virtual ~SQLStatement() {}

    virtual std::string getSQLStatementType() const = 0;

    virtual std::unordered_map<std::string, std::string> getAttributes() = 0;
};

class ColumnDefinition {
public:
    ColumnDefinition(const std::string& name, const std::string& dataType)
        : name(name), dataType(dataType) {}

    std::string name;
    std::string dataType;
};

class CreateTableStatement : public SQLStatement {
public:
    CreateTableStatement(const std::string& tableName, const std::string& fileName, const std::string& indexType, const std::string& indexedColumn)
        : tableName(tableName), fileName(fileName), indexType(indexType), indexedColumn(indexedColumn) {}

    std::string getSQLStatementType() const override {
        return "CREATE";
    }

    std::unordered_map<std::string, std::string> getAttributes() override {
        std::unordered_map<std::string, std::string> um;
        um["table_name"] = tableName;
        um["file_name"] = fileName;
        um["index_type"] = indexType;
        um["indexed_column"] = indexedColumn;

        return um;
    }

    std::string tableName;
    std::string fileName;
    std::string indexType;
    std::string indexedColumn;
    std::vector<ColumnDefinition> columns;
};

class Condition {
public:
    Condition() {}

    virtual std::string getConditionStr() = 0;
    virtual std::string getConditionType() const = 0;
};

class ConditionEquals : public Condition {
public:
    ConditionEquals(const std::string& columnName, const std::string& value)
        : columnName(columnName), value(value) {}

    std::string getConditionStr() override {
        return columnName + " = " + value;
    }

    std::string getConditionType() const override {
        return "equals";
    }

    std::string columnName;
    std::string value;
};

class ConditionBetween : public Condition {
public:
    ConditionBetween(const std::string& columnName, const std::string& left, const std::string& right)
        : columnName(columnName), left(left), right(right) {}

    std::string getConditionStr() override {
        return columnName + " between " + left + " and " + right;
    }

    std::string getConditionType() const override {
        return "between";
    }

    std::string columnName;
    std::string left;
    std::string right;
};

class SelectStatement : public SQLStatement {
public:
    SelectStatement(const std::string& tableName, Condition* condition)
        : tableName(tableName), condition(condition) {}

    ~SelectStatement() {
        delete condition;
    }

    std::string getSQLStatementType() const override {
        return "SELECT";
    }

    std::unordered_map<std::string, std::string> getAttributes() override {
        std::unordered_map<std::string, std::string> um;

        um["table_name"] = tableName;
        um["condition"] = condition->getConditionStr();

        return um;
    }

    std::string tableName;
    Condition* condition;
};

class InsertStatement : public SQLStatement {
public:
    InsertStatement(const std::string& tableName, const std::vector<std::string>& values)
        : tableName(tableName), values(values) {}

    std::string getSQLStatementType() const override {
        return "INSERT";
    }

    std::unordered_map<std::string, std::string> getAttributes() override {
        std::unordered_map<std::string, std::string> um;

        um["table_name"] = tableName;
        um["values"] = values[0];

        return um;
    }

    std::string tableName;
    std::vector<std::string> values;
};

class DeleteStatement : public SQLStatement {
public:
    DeleteStatement(const std::string& table_name, Condition* condition) : table_name(table_name), condition(condition) {}

    ~DeleteStatement() {
        delete condition;
    }

    std::string getSQLStatementType() const override {
        return "DELETE";
    }

    std::unordered_map<std::string, std::string> getAttributes() override {
        std::unordered_map<std::string, std::string> um;

        um["table_name"] = table_name;
        um["condition"] = condition->getConditionStr();

        return um;
    }

    std::string table_name;
    Condition* condition;
};

class EndStatement : public SQLStatement {
public:
    EndStatement() {}

    std::string getSQLStatementType() const override {
        return "END";
    }

    std::unordered_map<std::string, std::string> getAttributes() override {
        std::unordered_map<std::string, std::string> um;

        return um;
    }
};

#endif // SQLSTATEMENT_H
