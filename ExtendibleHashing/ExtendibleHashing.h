#ifndef PROYECTO1_BD2_EXTENDIBLEHASHING_H
#define PROYECTO1_BD2_EXTENDIBLEHASHING_H

#include "ExtendibleHashingBucket.h"
#include "Record.h"

using namespace std;

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

    long searchIndex(long key) {                            // O(1)
        long dataPos = hash(key) * sizeof(long);
        fstream indexFile(_indexFile, ios::binary | ios::in);
        if (!indexFile.is_open()) throw invalid_argument("Cannot open index file");
        indexFile.seekg(dataPos);
        long bucketPos;
        indexFile.read((char *) &bucketPos, sizeof(bucketPos));
        indexFile.close();
        return bucketPos;
    }

    long searchBucket(long bucketPos) {                     // O(log globalDepth())
        fstream dataFile(_dataFile, ios::binary | ios::in);
        if (!dataFile.is_open()) throw invalid_argument("Cannot open data file");
        while (bucketPos != -1) {
            Bucket<Record> bucket{};
            dataFile.seekg(bucketPos, ios::beg);
            dataFile.read((char *) &bucket, sizeof(bucket));
            if (bucket._size < FB || bucket._next == -1) break;
            bucketPos = bucket._next;
        }
        dataFile.close();
        return bucketPos;
    }

    void splitBucket(long bucket, long bits, int localDepth) {
        fstream indexFile(_indexFile, ios::binary | ios::app | ios::in);
        if (!indexFile.is_open()) throw invalid_argument("Cannot open index file");
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
        fstream dataFile(_dataFile, ios::binary | ios::in | ios::out);
        if (!dataFile.is_open()) throw invalid_argument("Cannot open data file");
        dataFile.seekg(bucketPos, ios::beg);
        Bucket<Record> bucket{};
        dataFile.read((char* ) &bucket, sizeof(bucket));
        if (bucket._size < FB) {
            bucket._values[bucket._size++] = pair<long, Record>(toPosition(key), record);
            dataFile.seekg(bucketPos, ios::beg);
            dataFile.write((char *) &bucket, sizeof(bucket));
        } else if (bucket._size == FB) {
            if (bucket._localDepth == FB) {
                dataFile.seekg(0, ios::end);
                long newBucketPos = dataFile.tellg();
                Bucket<Record> newBucket(bucket._localDepth, bucket._bits);
                dataFile.write((char *) &newBucket, sizeof(newBucket));
                bucket._next = newBucketPos;
                dataFile.seekg(bucketPos, ios::beg);
                dataFile.write((char *) &bucket, sizeof(bucket));
            } else {
                Bucket<Record> newBucket(bucket._localDepth + 1, bucket._bits | (1LL << bucket._localDepth));
                bucket._localDepth++;
                vector<pair<long, Record>> values = {pair<long, Record>(key, record)};
                for (int i = 0; i < bucket._size; ++i)
                    values.push_back(bucket._values[i]);
                bucket._size = 0;

                dataFile.seekg(bucketPos, ios::beg);
                dataFile.write((char *) &bucket, sizeof(bucket));
                dataFile.seekg(0, ios::end);
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
    explicit ExtendibleHashing(const string &name) : _indexFile(name + "-HashIndex.bin"), _dataFile(name + "-HashData.bin"),
                                                       _propertyFile(name + "-Depth.bin") {
        fstream propertyFile(_propertyFile, ios::binary | ios::in | ios::out);
        if (!propertyFile.is_open()) throw invalid_argument("Cannot open property file.");
        propertyFile.read((char *) &_globalDepth, sizeof(int));
        propertyFile.close();
    }

    explicit ExtendibleHashing(const string &name, int globalDepth) : _globalDepth(globalDepth),
                                                                          _indexFile(name + "-HashIndex.bin"),
                                                                          _dataFile(name + "-HashData.bin"),
                                                                          _propertyFile(name + "-Depth.bin") {
        ofstream indexFile(_indexFile, ios::binary | ios::trunc | ios::out );
        if (!indexFile.is_open()) throw invalid_argument("Cannot open index file.");

        int n = (1 << globalDepth);
        for (int i = 0; i < n; ++i) {
            long content = i % 2 * sizeof(Bucket<Record>);
            indexFile.write((char *) &content, sizeof(long));
        }
        indexFile.close();

        ofstream dataFile(_dataFile, ios::binary | ios::trunc | ios::out);
        if (!dataFile.is_open()) throw invalid_argument("Cannot open data file.");
        Bucket<Record> bucket0(1, 0), bucket1(1, 1);

        dataFile.write((char *) &bucket0, sizeof(bucket0));
        dataFile.write((char *) &bucket1, sizeof(bucket0));
        dataFile.close();

        ofstream propertyFile(_propertyFile, ios::binary | ios::trunc | ios::out);
        if (!propertyFile.is_open()) throw invalid_argument("Cannot open property file.");
        propertyFile.write((char *) &_globalDepth, sizeof(globalDepth));
        propertyFile.close();
    }


    pair<bool, Record> search(KeyType key) {
        long bucketPos = searchIndex(key);

        fstream dataFile(this->_dataFile, ios::binary | ios::in);
        if (!dataFile.is_open()) throw invalid_argument("Cannot open data file.");
        
        Bucket<Record> bucket;

        while (bucketPos != -1) {
            dataFile.seekg(bucketPos, ios::beg);
            dataFile.read((char *) &bucket, sizeof(bucket));
            dataFile.close();
            auto result = bucket.search(toPosition(key));

            if (result.first == true) return result;
            bucketPos = bucket._next;
        }

        Record not_found;

        not_found.empty_record();

        return pair<bool, Record>(false, not_found);
    }

    vector<Record> rangeSearch(KeyType keyA, KeyType keyB) {
        if (keyA > keyB) throw invalid_argument("Second key must be greater or equal to the first one.");
        vector<Record> result{};
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
        
        Bucket<Record> bucket;
        fstream dataFile(_dataFile, ios::binary | ios::in | ios::out);
        if (!dataFile.is_open()) throw invalid_argument("Cannot open data file Remove.");

        bool found = false;

        while (bucketPos != -1) {
            dataFile.seekg(bucketPos, ios::beg);
            dataFile.read((char *) &bucket, sizeof(bucket));
            auto result = bucket.search(toPosition(key));
            if (result.first == true) {
                bucket.remove(toPosition(key));
                found = true;
                break;
            }
            bucketPos = bucket._next;
        }

        if (found) {
            dataFile.seekg(bucketPos, ios::beg);
            dataFile.write((char* ) &bucket, sizeof(bucket));
            dataFile.close();
        } else {
            cout << "Value of key " << key << " not found. Couldn't remove." << endl;
        }
    }
};


#endif//PROYECTO1_BD2_EXTENDIBLEHASHING_H