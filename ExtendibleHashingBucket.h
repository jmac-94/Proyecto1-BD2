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

constexpr size_t M = 4;

template<typename Record>
struct Bucket {
    int _localDepth;
    long _bits;
    int _size;
    pair<long, Record> _values[M];
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
        vector<pair<long, Record>> values;
        for (const auto &e: _values) values.push_back(e);
        for (auto it = values.begin(); it < values.end(); ++it) {
            if (it->first == key) {
                values.erase(it);
                break;
            }
        }
        _size = values.size();
        for (long i = 0; i < _size; ++i) _values[i] = values[i];
    }
};