#include "SequentialFile.h"

#define OPEN_METADATA meta_data.open(meta_data_path, std::ios::binary | std::ios::in | std::ios::out);
#define CLOSE_METADATA meta_data.close();

#define OPEN_MAINFILE main_file.open(main_file_path, std::ios::binary | std::ios::in | std::ios::out);
#define CLOSE_MAINFILE main_file.close();

// ====================================================================================================================
// MAIN FUNCTIONS
// ====================================================================================================================

SequentialFile::SequentialFile(const std::string& meta_data_path, const std::string& main_file_path,
                               int initial_aux_file_max_size) {

    this->meta_data_path = meta_data_path;
    this->main_file_path = main_file_path;

    OPEN_METADATA
    meta_data.seekg(0, std::ios::end);
    bool meta_data_is_empty = meta_data.tellg() == 0;
    CLOSE_METADATA

    if (meta_data_is_empty) {
        this->aux_file_max_size = initial_aux_file_max_size;
        writeInitialMetadata(initial_aux_file_max_size);
    }
    else {
        readExistingMetadata();
    }
}

void SequentialFile::insertRecord(Record record_to_insert) {
    if (aux_file_count < aux_file_max_size) {
        int prev_record_in_data_pos = binSearchPrevPos(record_to_insert, 0, data_file_count);

        if (prev_record_in_data_pos == -1) {
            if (aux_file_count == 0) {
                if (data_file_count == 0) {
                    writeRecordInAuxFile(record_to_insert);
                    aux_file_count += 1;
                    writeCurrentMetadata();

                    return;
                }
                else {
                    record_to_insert.next_record_pos = 0;
                    writeRecordInAuxFile(record_to_insert);
                    aux_file_count += 1;
                    writeCurrentMetadata();

                    return;
                }
            }

            Record prev_record_in_aux;
            int prev_record_in_aux_pos;
            bool there_is_a_direct_prev = false;

            for (int i = 0; i < aux_file_count; ++i) {
                Record i_record;
                readRecordFromAuxFile(i_record, i);

                if (i_record.bookID < record_to_insert.bookID) {
                    there_is_a_direct_prev = true;
                    prev_record_in_aux = i_record;
                    prev_record_in_aux_pos = i;
                    break;
                }
            }

            if (there_is_a_direct_prev) {
                for (int i = 0; i < aux_file_count; ++i) {
                    Record i_record;
                    readRecordFromAuxFile(i_record, i);

                    if (i_record.bookID < record_to_insert.bookID && i_record.bookID > prev_record_in_aux.bookID) {
                        prev_record_in_aux = i_record;
                        prev_record_in_aux_pos = i;
                        there_is_a_direct_prev = true;
                    }
                }
            }

            Record next_record_in_aux;
            readRecordFromAuxFile(next_record_in_aux, 0);
            int next_record_in_aux_pos = 0;

            for (int i = 0; i < aux_file_count; ++i) {
                Record i_record;
                readRecordFromAuxFile(i_record, i);

                if (i_record.bookID > record_to_insert.bookID && i_record.bookID <= next_record_in_aux.bookID) {
                    next_record_in_aux = i_record;
                    next_record_in_aux_pos = i + data_file_count;
                }
            }

            if (there_is_a_direct_prev) {
                int prev_record_in_aux_old_next_record_pos = prev_record_in_aux.next_record_pos;
                record_to_insert.next_record_pos = prev_record_in_aux_old_next_record_pos;
                prev_record_in_aux.next_record_pos = data_file_count + aux_file_count;
                writeRecordInAuxFile(record_to_insert);
                updateRecordInAuxFile(prev_record_in_aux, prev_record_in_aux_pos);
                aux_file_count += 1;
                writeCurrentMetadata();
            }
            else {
                if (data_file_count == 0) {
                    record_to_insert.next_record_pos = next_record_in_aux_pos;
                    writeRecordInAuxFile(record_to_insert);
                    aux_file_count += 1;
                    writeCurrentMetadata();
                }
                else {
                    Record next_record_in_data;
                    readRecordFromMainFile(next_record_in_data, 0);

                    if (next_record_in_aux.bookID < next_record_in_data.bookID) {
                        record_to_insert.next_record_pos = next_record_in_aux_pos;
                    }
                    else {
                        record_to_insert.next_record_pos = 0;
                    }

                    writeRecordInAuxFile(record_to_insert);
                    aux_file_count += 1;
                    writeCurrentMetadata();
                }
            }
        }
        else {
            Record prev_record_in_data;
            readRecordFromMainFile(prev_record_in_data, prev_record_in_data_pos);

            while (true) {
                Record prev_record_in_data_next;

                if (prev_record_in_data.next_record_pos == -1) {
                    break;
                }

                readRecordFromMainFile(prev_record_in_data_next, prev_record_in_data.next_record_pos);
                if (prev_record_in_data_next.bookID < record_to_insert.bookID) {
                    prev_record_in_data_pos = prev_record_in_data.next_record_pos;
                    prev_record_in_data = prev_record_in_data_next;
                }
                else {
                    break;
                }
            }

            int prev_record_in_data_old_next = prev_record_in_data.next_record_pos;
            record_to_insert.next_record_pos = prev_record_in_data_old_next;
            prev_record_in_data.next_record_pos = data_file_count + aux_file_count;

            writeRecordInMainFile(prev_record_in_data, prev_record_in_data_pos);
            writeRecordInAuxFile(record_to_insert);
            aux_file_count += 1;
            writeCurrentMetadata();
        }
    }
    else {
        reestructureFile();
        insertRecord(record_to_insert);
    }
}

int SequentialFile::binSearchPrevPos(Record record, int low, int high) {
    int pos = -1;

    for (int i = 0; i < data_file_count; ++i) {
        Record i_record;
        readRecordFromMainFile(i_record, i);

        if (i_record.bookID < record.bookID) {
            pos = i;
        }
    }

    return pos;
}

void SequentialFile::reestructureFile() {
    Record first_record_in_data;
    int first_record_in_data_pos;

    if (data_file_count > 0) {
        for (int i = 0; i < data_file_count; ++i) {
            Record i_record;
            readRecordFromMainFile(i_record, i);

            if (!i_record.is_deleted) {
                first_record_in_data = i_record;
                first_record_in_data_pos = i;
            }
        }
    }

    for (int i = 0; i < data_file_count; ++i) {
        Record i_record;
        readRecordFromMainFile(i_record, i);

        if (i_record.bookID < first_record_in_data.bookID && !i_record.is_deleted) {
            first_record_in_data = i_record;
            first_record_in_data_pos = i;
        }
    }

    Record first_record_in_aux;
    int first_record_in_aux_pos = 0;

    readRecordFromAuxFile(first_record_in_aux, 0);

    for (int i = 0; i < aux_file_count; ++i) {
        Record i_record;
        readRecordFromAuxFile(i_record, i);

        if (i_record.bookID < first_record_in_aux.bookID && !i_record.is_deleted) {
            first_record_in_aux = i_record;
            first_record_in_aux_pos = i;
        }
    }

    Record real_first_record;
    int real_first_record_pos;

    if (data_file_count == 0 || first_record_in_aux.bookID < first_record_in_data.bookID) {
        real_first_record = first_record_in_aux;
        real_first_record_pos = first_record_in_aux_pos;
    }
    else {
        real_first_record = first_record_in_data;
        real_first_record_pos = first_record_in_data_pos;
    }

    std::ofstream new_main;
    new_main.open("../tmp_main.bin", std::ios::binary | std::ios::out);

    for (int i = 0; i < data_file_count + aux_file_count; ++i) {
        Record real_first_record_correct_next = real_first_record;
        if (i == data_file_count + aux_file_count - 1) {
            real_first_record_correct_next.next_record_pos = -1;
        }
        else {
            real_first_record_correct_next.next_record_pos = i + 1;
        }

        new_main.seekp(i * sizeof(Record), std::ios::beg);
        new_main.write((char*) &real_first_record_correct_next, sizeof(Record));
        readRecordFromMainFile(real_first_record, real_first_record.next_record_pos);
    }
    new_main.close();

    data_file_count = data_file_count + aux_file_count;
    aux_file_count = 0;
    writeCurrentMetadata();

    remove(main_file_path.c_str());
    rename("../tmp_main.bin", main_file_path.c_str());

}

std::string SequentialFile::toString() {
    std::string res;

    res += "INFO --------------------------------------------------------------------------\n";
    res += "Data file count:\t" + std::to_string(this->data_file_count) + "\n";
    res += "Aux file count:\t\t" + std::to_string(this->aux_file_count) + "\n";
    res += "Aux file max size:\t" + std::to_string(this->aux_file_max_size) + "\n";
    res += "First record position:\t" + std::to_string(this->first_record_pos) + "\n";
    res += "-------------------------------------------------------------------------------\n\n";

    res += "DATA FILE ---------------------------------------------------------------------\n";
    for (int i = 0; i < data_file_count; ++i) {
        Record record_i;
        readRecordFromMainFile(record_i, i);
        res += std::to_string(i) + ".\t" + record_i.toString() + "\n";
    }
    res += "-------------------------------------------------------------------------------\n\n";

    res += "AUX FILE ----------------------------------------------------------------------\n";
    for (int i = data_file_count; i < data_file_count + aux_file_count; ++i) {
        Record record_i;
        readRecordFromMainFile(record_i, i);
        res += std::to_string(i) + ".\t" + record_i.toString() + "\n";
    }
    res += "-------------------------------------------------------------------------------\n";

    return res;
}

std::optional<Record> SequentialFile::searchRecord(int id) {
    Record found_record;
    int possible_pos_in_data = binSearch(id);
    int possible_pos_in_aux = seqSearch(id);

    if (possible_pos_in_data == -1 && possible_pos_in_aux == -1) {
        return std::nullopt;
    }

    if (possible_pos_in_data != -1) {
        readRecordFromMainFile(found_record, possible_pos_in_data);
    }
    else if (possible_pos_in_aux != -1) {
        readRecordFromAuxFile(found_record, possible_pos_in_aux);
    }

    return found_record;
}

int SequentialFile::binSearch(int id) {
    int l = 0;
    int r = data_file_count - 1;

    while (l <= r) {
        int m = l + (r - l) / 2;

        Record mid_record;
        readRecordFromMainFile(mid_record, m);

        if (mid_record.bookID == id) {
            return m;
        }

        if (mid_record.bookID < id) {
            l = m + 1;
        }
        else {
            r = m - 1;
        }
    }

    return -1;
}

int SequentialFile::seqSearch(int id) {
    for (int i = 0; i < aux_file_count; ++i) {
        Record i_record;
        readRecordFromAuxFile(i_record, i);
        if (i_record.bookID == id) {
            return i;
        }
    }
    return -1;
}

std::vector<Record> SequentialFile::rangeSearchRecords(int id_low, int id_high) {
    int start = lowerBound(id_low);
    int end = upperBound(id_high);

    std::vector<Record> result;
    for (int i = start; i < end; i++) {
        Record i_record;
        readRecordFromMainFile(i_record, i);
        result.push_back(i_record);
    }

    for (int i = 0; i < aux_file_count; i++) {
        Record i_record;
        readRecordFromAuxFile(i_record, i);
        if (i_record.bookID >= id_low && i_record.bookID <= id_high) {
            result.push_back(i_record);
        }
    }

    return result;
}


// ====================================================================================================================
// HELPER FUNCTIONS
// ====================================================================================================================

void SequentialFile::readRecordFromMainFile(Record& record, int pos) {
    OPEN_MAINFILE
    main_file.seekg(pos * (long) sizeof(Record), std::ios::beg);
    main_file.read((char*) &record, sizeof(Record));
    CLOSE_MAINFILE
}

void SequentialFile::readRecordFromAuxFile(Record& record, int pos) {
    OPEN_MAINFILE
    main_file.seekg(data_file_count * (long) sizeof(Record) + pos * (long) sizeof(Record), std::ios::beg);
    main_file.read((char*) &record, sizeof(Record));
    CLOSE_MAINFILE
}

void SequentialFile::writeRecordInMainFile(Record& record, int pos) {
    OPEN_MAINFILE
    main_file.seekp(pos * (long) sizeof(Record), std::ios::beg);
    main_file.write((char*) &record, sizeof(Record));
    CLOSE_MAINFILE
}

void SequentialFile::writeRecordInAuxFile(Record& record) {
    OPEN_MAINFILE
    main_file.seekp(data_file_count * (long) sizeof(Record) + aux_file_count * (long) sizeof(Record),
                    std::ios::beg);
    main_file.write((char*) &record, sizeof(Record));
    CLOSE_MAINFILE
}

void SequentialFile::updateRecordInAuxFile(Record& record, int pos) {
    OPEN_MAINFILE
    main_file.seekp(data_file_count * (long) sizeof(Record) + pos * (long) sizeof(Record), std::ios::beg);
    main_file.write((char*) &record, sizeof(Record));
    CLOSE_MAINFILE
}

void SequentialFile::writeInitialMetadata(int initial_aux_file_max_size) {
    OPEN_METADATA
    meta_data.seekp(0, std::ios::beg);
    meta_data.write((char*) &data_file_count, sizeof(data_file_count));
    meta_data.write((char*) &aux_file_count, sizeof(aux_file_count));
    meta_data.write((char*) &first_record_pos, sizeof(first_record_pos));
    meta_data.write((char*) &initial_aux_file_max_size, sizeof(initial_aux_file_max_size));
    CLOSE_METADATA
}

void SequentialFile::readExistingMetadata() {
    OPEN_METADATA
    meta_data.seekg(0, std::ios::beg);
    meta_data.read((char*) &data_file_count, sizeof(data_file_count));
    meta_data.read((char*) &aux_file_count, sizeof(aux_file_count));
    meta_data.read((char*) &first_record_pos, sizeof(first_record_pos));
    meta_data.read((char*) &aux_file_max_size, sizeof(aux_file_max_size));
    CLOSE_METADATA
}

void SequentialFile::writeCurrentMetadata() {
    OPEN_METADATA
    meta_data.seekp(0, std::ios::beg);
    meta_data.write((char*) &data_file_count, sizeof(data_file_count));
    meta_data.write((char*) &aux_file_count, sizeof(aux_file_count));
    meta_data.write((char*) &first_record_pos, sizeof(first_record_pos));
    meta_data.write((char*) &aux_file_max_size, sizeof(aux_file_max_size));
    CLOSE_METADATA
}

int SequentialFile::lowerBound(int x) {
    int l = 0, r = data_file_count - 1, ans = data_file_count;

    while (l <= r) {
        int m = l + (r - l) / 2;
        Record m_record;
        readRecordFromMainFile(m_record, m);

        if (m_record.bookID >= x) {
            ans = m;
            r = m - 1;
        }
        else {
            l = m + 1;
        }
    }

    return ans;
}

int SequentialFile::upperBound(int x) {
    int l = 0, r = data_file_count - 1, ans = data_file_count;

    while (l <= r) {
        int m = l + (r - l) / 2;
        Record m_record;
        readRecordFromMainFile(m_record, m);

        if (m_record.bookID <= x) {
            l = m + 1;
        }
        else {
            ans = m;
            r = m - 1;
        }
    }

    return ans;
}


















