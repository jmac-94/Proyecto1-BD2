#ifndef PROYECTO1_BD2_EXTENDIBLEHASHING_H
#define PROYECTO1_BD2_EXTENDIBLEHASHING_H

#include "ExtendibleHashingBucket.h"

using namespace std;

class Record {
private:
    long key;
    long next;
    string* data;
public:

    Record(){};

    void empty_record() {
        this->key = -1;
        this->data = nullptr;
        this->next = -1;
    }

    long getKey() {return this->key;}

    void serialization(string object);

    void print() {
        cout << "KEY: " << this->key << " Data: " << *this->data << endl;
    }; // print Record Data
    void input() { // initialize from input
        cout << "Ingresar key (ID): "; cin >> this->key;

        string s;
        cout << "Data: "; cin >> s;

        this->data = new string(s);
    }
};


template<typename KeyType>
class ExtendibleHashing {
    string _indexFile;
    string _dataFile;
    string _propertyFile;
    int _globalDepth;

    long toPosition(KeyType key) {
        long k = long(key);
        return k;
    }

    long hash(long k) {
        return k & ((1LL << _globalDepth) - 1);
    }

    long searchIndex(long key) {
        cout << "Called Search index for key " << key << endl;
        long dataPos = hash(key) * sizeof(long);
        fstream indexFile(_indexFile, ios::binary | ios::in);
        if (!indexFile.is_open()) throw invalid_argument("Cannot open index file. 1");
        indexFile.seekg(dataPos);
        long bucketPos;
        indexFile.read((char *) &bucketPos, sizeof(bucketPos));
        indexFile.close();
        cout << "IndexSearch Pos: " << bucketPos << endl;
        return bucketPos;
    }

    long searchBucket(long bucketPos) {
        fstream dataFile(_dataFile, ios::binary | ios::in);
        if (!dataFile.is_open()) throw invalid_argument("Cannot open data file. 1");
        while (bucketPos != -1) {
            Bucket<Record> bucket{};
            dataFile.seekg(bucketPos);
            dataFile.read((char *) &bucket, sizeof(bucket));
            if (bucket._size < M || bucket._next == -1) break;
            bucketPos = bucket._next;
        }
        dataFile.close();
        return bucketPos;
    }

    void splitBucket(long bucket, long bits, int localDepth) {
        fstream indexFile(_indexFile, ios::binary | ios::app | ios::in);
        if (!indexFile.is_open()) throw invalid_argument("Cannot open index file. 2");
        long n = 1LL << (_globalDepth - localDepth);
        for (long i = 0; i < n; ++i) {
            long j = (bits | (i << localDepth));
            indexFile.seekg(j * sizeof(long));
            indexFile.write((char *) &bucket, sizeof(bucket));
        }
        indexFile.close();
    }

    void _insert(long key, Record record) {
        long bucketPos = searchBucket(searchIndex(key));
        std::fstream dataFile(_dataFile, std::ios::binary | ios::in | ios::out);
        if (!dataFile.is_open()) throw std::invalid_argument("Cannot open data file. 2");
        dataFile.seekg(bucketPos, ios::beg);
        Bucket<Record> bucket{};
        dataFile.read((char *) &bucket, sizeof(bucket));
        if (bucket._size < M) {
            bucket._values[bucket._size++] = std::pair<long, Record>(toPosition(key), record);
            dataFile.seekg(bucketPos, ios::beg);
            dataFile.write((char *) &bucket, sizeof(bucket));
        } else if (bucket._size == M) {
            if (bucket._localDepth == M) {
                dataFile.seekg(0, std::ios::end);
                long newBucketPos = dataFile.tellg();
                Bucket<Record> newBucket(bucket._localDepth, bucket._bits);
                dataFile.write((char *) &newBucket, sizeof(newBucket));
                bucket._next = newBucketPos;
                dataFile.seekg(bucketPos);
                dataFile.write((char *) &bucket, sizeof(bucket));
            } else {
                Bucket<Record> newBucket(bucket._localDepth + 1, bucket._bits | (1LL << bucket._localDepth));
                bucket._localDepth++;
                std::vector<std::pair<long, Record>> values = {std::pair<long, Record>(key, record)};
                for (int i = 0; i < bucket._size; ++i)
                    values.push_back(bucket._values[i]);
                bucket._size = 0;

                dataFile.seekg(bucketPos);
                dataFile.write((char *) &bucket, sizeof(bucket));
                dataFile.seekg(0, std::ios::end);
                long newBucketPos = dataFile.tellg();
                dataFile.write((char *) &newBucket, sizeof(bucket));
                splitBucket(newBucketPos, newBucket._bits, newBucket._localDepth);
                dataFile.close();
                for (const auto &e: values)
                    insert(e.first, e.second);
            }
        }
        dataFile.close();
    }

public:
    explicit ExtendibleHashing(const string &name) : _indexFile(name + "-index.bin"), _dataFile(name + "-data.bin"),
                                                       _propertyFile(name + "-property.bin") {
        fstream propertyFile(_propertyFile, ios::binary | ios::in | ios::out);
        if (!propertyFile.is_open()) throw invalid_argument("Cannot open property file.");
        propertyFile.read((char *) &_globalDepth, sizeof(int));
        propertyFile.close();
    }

    explicit ExtendibleHashing(const string &name, int globalDepth) : _globalDepth(globalDepth),
                                                                          _indexFile(name + "-index.bin"),
                                                                          _dataFile(name + "-data.bin"),
                                                                          _propertyFile(name + "-property.bin") {
        std::ofstream indexFile(_indexFile, std::ios::binary | std::ios::trunc | ios::in | ios::out );
        if (!indexFile.is_open()) throw std::invalid_argument("Cannot open index file.");
        int n = (1 << globalDepth);
        for (int i = 0; i < n; ++i) {
            long content = i % 2 * sizeof(Bucket<Record>);
            indexFile.write((char *) &content, sizeof(long));
        }
        indexFile.close();

        std::ofstream dataFile(_dataFile, std::ios::binary | std::ios::trunc | ios::out | ios::in);
        if (!dataFile.is_open()) throw std::invalid_argument("Cannot open data file.");
        Bucket<Record> bucket0(1, 0), bucket1(1, 1);
        dataFile.write((char *) &bucket0, sizeof(bucket0));
        dataFile.write((char *) &bucket1, sizeof(bucket0));
        dataFile.close();

        std::ofstream propertyFile(_propertyFile, std::ios::binary | std::ios::trunc | ios::out | ios::in);
        if (!propertyFile.is_open()) throw std::invalid_argument("Cannot open property file.");
        propertyFile.write((char *) &_globalDepth, sizeof(globalDepth));
        propertyFile.close();
    }


    pair<bool, Record> search(KeyType key) {
        cout << "Called search for key: " << key << endl;
        long bucketPos = searchIndex(key);
        cout << bucketPos << endl;
        while (bucketPos != -1) {
            Bucket<Record> bucket{};
            std::fstream dataFile(this->_dataFile, std::ios::binary | ios::in | ios::app);
            if (!dataFile.is_open()) throw std::invalid_argument("Cannot open data file.");
            
            dataFile.seekg(bucketPos, ios::beg);
            dataFile.read((char *) &bucket, sizeof(bucket));
            dataFile.close();
            auto result = bucket.search(toPosition(key));

            if (result.first == true) return result;
            bucketPos = bucket._next;
            cout << "Bucket Position: " << bucketPos << endl;;
        }

        Record not_found;

        not_found.empty_record();

        return std::pair<bool, Record>(false, not_found);
    }

    vector<Record> rangeSearch(KeyType keyA, KeyType keyB) {
        if (keyA > keyB) throw std::invalid_argument("Second key must be greater or equal to the first one.");
        std::vector<Record> result{};
        for (KeyType k = keyA; k <= keyB; ++k) {
            auto e = search(k);
            if (e.first == true) result.push_back(e.second);
        }
        return result;
    }

    void insert(KeyType key, Record record) {
        _insert(toPosition(key), record);
    }

    void remove(KeyType key) {
        long bucketPos = searchIndex(key);
        while (bucketPos != -1) {
            Bucket<Record> bucket{};
            std::fstream dataFile(_dataFile, std::ios::binary);
            dataFile.seekg(bucketPos);
            dataFile.read((char *) &bucket, sizeof(bucket));
            dataFile.close();
            auto result = bucket.search(toPosition(key));
            if (result.first == true) {
                bucket.remove(toPosition(key));
                break;
            }
            bucketPos = bucket._next;
        }
    }
};

#endif//PROYECTO1_BD2_EXTENDIBLEHASHING_H