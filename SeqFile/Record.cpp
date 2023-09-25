#include "Record.h"

std::string Record::toString() {
    std::stringstream ss;
    ss << bookID << ", " << name << ", " << last_name << ", " << next_record_pos;

    return ss.str();
}

