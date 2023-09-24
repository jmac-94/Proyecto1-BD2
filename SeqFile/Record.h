#ifndef SEQUENTIAL_FILE_RECORD_H
#define SEQUENTIAL_FILE_RECORD_H

#include <iostream>
#include <sstream>

class Record {
public:
    int id{};
    char name[10]{'\0'};
    char last_name[10]{'\0'};

    int next_record_pos = -1;
    bool is_deleted = false;

    std::string toString();
};

#endif