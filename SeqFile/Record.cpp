#include "Record.h"

std::string Record::toString() {
    std::stringstream ss;
    ss << id << ", " << name << ", " << last_name << ", " << next_record_pos;

    return ss.str();
}

Record::Record(int id, std::string name, std::string lastname) {
    this->id = id;
    std::strncpy(this->name, name.c_str(), sizeof(name) - 1);
    std::strncpy(this->last_name, lastname.c_str(), sizeof(last_name) - 1);

    name[sizeof(name) - 1] = '\0';
    last_name[sizeof(last_name) - 1] = '\0';;
}
