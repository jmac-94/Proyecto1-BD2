#ifndef EXTENDIBLEHASHING_H
#define EXTENDIBLEHASHING_H

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
        return k & ((1LL << _globalDepth) - 1);             // k % (2 ^ _globalDepth)
    }

    long getInitialBucketPosition(long key) {                            // O(1)
        long dataPos = hash(key) * sizeof(KeyType);
        fstream indexFile(_indexFile, ios::binary | ios::in);
        if (!indexFile.is_open()) throw invalid_argument("Cannot open index file");
        indexFile.seekg(dataPos, ios::beg);
        long bucketPos;
        indexFile.read((char *) &bucketPos, sizeof(bucketPos));
        indexFile.close();
        return bucketPos;
    }

    long getLastBucket(long bucketPos) {                     // O(log globalDepth())
        fstream dataFile(_dataFile, ios::binary | ios::in);
        if (!dataFile.is_open()) throw invalid_argument("Cannot open data file");

        Bucket<Record> bucket;
        while (bucketPos != -1) {
            dataFile.seekg(bucketPos, ios::beg);
            dataFile.read((char *) &bucket, sizeof(bucket));
            if (bucket._next == -1) break;
            bucketPos = bucket._next;
        }

        dataFile.close();
        return bucketPos;
    }

    void splitBucket(long bucket, long bits, int localDepth) {
        fstream indexFile(_indexFile, ios::binary | ios::in | ios::out);
        if (!indexFile.is_open()) throw invalid_argument("Cannot open index file");
        long n = 1LL << this->_globalDepth;
        for (long i = 0; i < n; ++i) {
            if ((i & ((1LL << localDepth) - 1)) == bits) {
                indexFile.seekg(i * sizeof(long), ios::beg);
                indexFile.write((char *) &bucket, sizeof(long));
            }
        }
        indexFile.close();
    }

    void _insert(long key, Record record) {

        // Obtengo el codigo hash de la llave y bajo hasta al ultimo bucket enlazado.
        long bucketPos = getLastBucket(getInitialBucketPosition(key));

        // Abrimos el datafile
        fstream dataFile(_dataFile, ios::binary | ios::in | ios::out);
        if (!dataFile.is_open()) throw invalid_argument("Cannot open data file");

        // Cargo el bucket de dicha posicion
        dataFile.seekg(bucketPos, ios::beg);
        Bucket<Record> bucket;
        dataFile.read((char* ) &bucket, sizeof(bucket));

        // Si el bucket aun tiene espacio, se inserta en dicho bucket y se escribe el en datafile
        if (bucket._size < FB) {
            bucket._values[bucket._size] = pair<long, Record>(key, record);
            bucket._size++;
            dataFile.seekg(bucketPos, ios::beg);
            dataFile.write((char *) &bucket, sizeof(bucket));
        } else if (bucket._size == FB) {    // En caso de que este lleno
            if (bucket._localDepth == this->_globalDepth) { // Si dicho bucket ya alcanzo la profundidad global se encadena un nuevo bucket al final del archivo.

                dataFile.seekg(0, ios::end);

                // Calculo la posicion del nuevo bucket a insertar, me muevo hacia ahi, inserto y escribo
                long newBucketPos = dataFile.tellg();
                Bucket<Record> newBucket(bucket._localDepth, bucket._bits);
                newBucket._values[newBucket._size] = pair<KeyType, Record>(key, record);
                newBucket._size++;
                newBucket._next = -1;
                dataFile.seekg(newBucketPos, ios::beg);
                dataFile.write((char *) &newBucket, sizeof(Bucket<Record>));

                // Enlazo el bucket actual al bucket nuevo, luego sobreescribo el bucket actual
                bucket._next = newBucketPos;
                dataFile.seekg(bucketPos, ios::beg);
                dataFile.write((char *) &bucket, sizeof(Bucket<Record>));
            } else {
                // Se crea el nuevo bucket de profundidad local aumentada en 1 que contendra "1" + _bits y se calcula su posicion
                Bucket<Record> newBucket(bucket._localDepth + 1, bucket._bits | (1LL << bucket._localDepth));
                dataFile.seekg(0, ios::end);
                long newBucketPos = dataFile.tellg();

                // Se aumenta en uno la profundidad local
                bucket._localDepth++;

                // Creo un vector para saber que valores voy a tener que volver a agregar
                vector<pair<KeyType, Record>> new_values_to_insert(bucket._size + 1);
                for (int i = 0; i < bucket._size; ++i) {
                    new_values_to_insert[i] = bucket._values[i];
                }
                new_values_to_insert[bucket._size] = pair<KeyType, Record> (key, record);

                // "Vacio" el bucket actual
                bucket._size = 0;

                splitBucket(newBucketPos, newBucket._bits, newBucket._localDepth);

                // Escribo el bucket modificado
                dataFile.seekg(bucketPos, ios::beg);
                dataFile.write((char *) &bucket, sizeof(Bucket<Record>));

                // Escribo el nuevo bucket
                dataFile.seekg(newBucketPos, ios::beg);
                dataFile.write((char *) &newBucket, sizeof(Bucket<Record>));

                for (auto& v: new_values_to_insert) {
                    this->insert(v.first, v.second);
                }
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

    void read_from_direction(long direction) {
        fstream dataFile(_dataFile, ios::binary | ios::in | ios::out);
        if (!dataFile.is_open()) throw invalid_argument("Cannot open data file");
        dataFile.seekg(direction, ios::beg);
        Bucket<Record> bucket{};
        dataFile.read((char* ) &bucket, sizeof(bucket));
        bucket.print();
        dataFile.seekg(0, ios::end);
        long position = dataFile.tellg();
        cout << "EOF position: " << position << endl;
        dataFile.close();
    }


    pair<bool, Record> search(KeyType key) {
        long bucketPos = getInitialBucketPosition(key);

        fstream dataFile(this->_dataFile, ios::binary | ios::in);
        if (!dataFile.is_open()) throw invalid_argument("Cannot open data file.");

        Bucket<Record> bucket;

        while (bucketPos != -1) {
            dataFile.seekg(bucketPos, ios::beg);
            dataFile.read((char *) &bucket, sizeof(bucket));
            auto result = bucket.search(toPosition(key));
            if (result.first == true) return result;
            bucketPos = bucket._next;
        }

        dataFile.close();

        Record not_found;

        not_found.empty_record();

        return pair<bool, Record>(false, not_found);
    }

    vector<Record> rangeSearch(KeyType keyA, KeyType keyB) {
        if (keyA > keyB) throw invalid_argument("Second key must be greater or equal to the first one.");
        vector<Record> result;
        for (KeyType k = keyA; k <= keyB; ++k) {
            auto e = this->search(k);
            if (e.first == true) result.push_back(e.second);
        }
        return result;
    }

    void insert(KeyType key, Record record) {
        _insert(toPosition(key), record);
    }

    void remove(KeyType key) {
        long bucketPos = getInitialBucketPosition(key);

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

#endif // EXTENDIBLEHASHING_H
