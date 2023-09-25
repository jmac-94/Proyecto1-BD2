#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <utility>
#include <string>
#include <chrono>

#include <QDir>

using namespace std;

#include <QMainWindow>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>

#include "SQLParser.h"

#include "AVLFile/AVLTreeFile.h"
#include "SeqFile/SequentialFile.h"
#include "ExtendibleHashing/ExtendibleHashing.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Methods
    void readTablesFromCSV();
    void writeTablesToCSV();
    void createStructures();
    void loadTablesNameGUI();

    pair<string, unordered_map<string, string>> parseQuery(string query_text);
    void getQuery();

    // Create
    void createTableAVL(string file_name, string indexed_column);
    void createTableSeq(string file_name, string indexed_column);
    void createTableHash(string file_name, string indexed_column);

    AVLTreeFile<int>* createAVLFromAtribute(const string& file_name, string attribute);
    SequentialFile* createSeqFileFromAttribute(const string& file_name, const string& meta_path, const string& main_path);
    ExtendibleHashing<long>* createExtendibleHashingFromAtribute(const string& filename, string atribute, int depth);

    // Select
    void selectTableAVL(string table_name, Condition* condition);
    void selectTableSeq(string table_name, Condition* condition);
    void selectTableHash(string table_name, Condition* condition);

    // Insert
    void insertTableAVL(string table_name, vector<string> values);
    void insertTableSeq(string table_name, vector<string> values);
    void insertTableHash(string table_name, vector<string> values);

    // Delete
    void deleteTableAVL(string table_name, Condition* condition);

    void displayRecords(vector<Record> &records);

private:
    Ui::MainWindow *ui;

    // attributes
    QTableWidget* table;

    // Structures tables
    string table_avl_name = "";
    string table_seq_name = "";
    string table_hash_name = "";

    // Structures
    AVLTreeFile<int>* table_avl{};
    SequentialFile* table_seq{};
    ExtendibleHashing<long>* table_hash{};
};

#endif // MAINWINDOW_H
