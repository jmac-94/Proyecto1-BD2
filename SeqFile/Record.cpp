#include "Record.h"

std::string Record::toString() {
    std::stringstream ss;
    ss << id << ", " << name << ", " << last_name << ", " << next_record_pos;

    return ss.str();
}

Record::Record(int id, std::string name, std::string lastname) {
    this->id = id;
    strcpy_s(this->name, "TEST");
    strcpy_s(this->last_name, "TEST");
}
