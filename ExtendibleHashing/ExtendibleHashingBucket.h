#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <regex>
#include <vector>
#include <cstring>
#include <chrono>
#include <random>

using namespace std;

constexpr size_t FB = 4;

template<typename Record>
struct Bucket {
    int _localDepth;
    long _bits;
    int _size;
    pair<long, Record> _values[FB];
    long _next;
public:
    Bucket() = default;

    explicit Bucket(int localDepth, long bits) : _localDepth(localDepth), _bits(bits), _size(0), _next(-1) {}

    pair<bool, Record> search(long key) {
        for (long i = 0; i < _size; ++i)
            if (_values[i].first == key) return pair<bool, Record>(true, _values[i].second);
        return pair<bool, Record>(false, Record{});
    }

    void remove(long key) {
        bool found = false;
        for (int i = 0; i < this->_size; i++) {
            if (this->_values[i].first == key) {
                found = true;
            }

            if (found && i != this->_size - 1) {
                this->_values[i] = this->_values[i+1];
            }
        }
        this->_values[this->_size - 1] = pair<long, Record>{};
        this->_size--;
    }
};