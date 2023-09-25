#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("BD");
    setWindowState(Qt::WindowMaximized);

    table = new QTableWidget();

    table->setColumnCount(4);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setHorizontalHeaderLabels(QStringList{tr("bookID"),
                                                 tr("title"),
                                                 tr("authors"),
                                                 tr("average_rating")});

    ui->BlocksDiv->addWidget(table);

    // Push buttons
    this->connect(ui->getQueryButton, &QPushButton::clicked, this, &MainWindow::getQuery);

    readTablesFromCSV();  // tmb cambia el current path
    createStructures();
    loadTablesNameGUI();
}

MainWindow::~MainWindow()
{
    writeTablesToCSV();

    delete ui;
}

void MainWindow::readTablesFromCSV() {
    QDir::setCurrent("../GUIProyectoBD2");

    cout << QDir::currentPath().toStdString() << endl;

    ifstream input_file("tables.csv");

    if (input_file.is_open()) {
        input_file.seekg(0, ios::end);

        // Si el archivo no esta vacio
        if (input_file.tellg() != 0) {
            input_file.seekg(0, ios::beg);
            string line;
            unordered_map<string, string> tables;

            while (getline(input_file, line)) {
                string data = "";
                string index_struct = "";
                int col = 0;

                for (const auto& c : line) {
                    if (c == ',') {
                        switch(col) {
                            case 0:
                                index_struct = data;
                                break;
                        }
                        data = "";
                        col++;
                    }
                    else {
                        data += c;
                    }
                }

                tables[index_struct] = data;
            }

            table_avl_name = tables["avl"];
            table_seq_name = tables["seq"];
            table_hash_name = tables["hash"];

            cout << "table avl name: " << table_avl_name << endl;
        }

        input_file.close();
    }
    else {
        cout << "Cant open file\n";
    }
}

void MainWindow::writeTablesToCSV() {
    ofstream output_file("tables.csv");

    if (output_file.is_open()) {
        // Write the table names to the file
        output_file << "avl," << table_avl_name << endl;
        output_file << "seq," << table_seq_name << endl;
        output_file << "hash," << table_hash_name << endl;

        output_file.close();
        cout << "Tables have been written to 'tables.csv'." << endl;
    } else {
        cout << "Cannot open 'tables.csv' for writing." << endl;
    }
}

void MainWindow::createStructures() {
    if (table_avl_name != "") {
        createTableAVL("books.csv", "");
    }
    if (table_seq_name != "") {
        createTableSeq("smallBooks.csv", "");
    }
    if (table_hash_name != "") {
        createTableHash("books.csv", "");
    }
}

void MainWindow::loadTablesNameGUI() {
    QString text = "<html><head/><body><p align=\"center\"><span style=\" font-family:'Segoe UI'; font-size:16pt; color:#ececec;\">";

    if (table_avl_name != "") {
        QString text_avl = text + QString::fromStdString(table_avl_name) + "</span></p></body></html>";
        ui->Table1->setText(text_avl);
    }
    if (table_seq_name != "") {
        QString text_seq = text + QString::fromStdString(table_seq_name) + "</span></p></body></html>";
        ui->Table2->setText(text_seq);
    }
    if (table_hash_name != "") {
        QString text_hash = text + QString::fromStdString(table_hash_name) + "</span></p></body></html>";
        ui->Table3->setText(text_hash);
    }
}

pair<string, unordered_map<string, string>> MainWindow::parseQuery(string query_text) {
    SQLScanner scanner(query_text);
    SQLParser parser(&scanner);

    SQLStatement* statement;
    SQLStatement* previous_statement;

    while (true) {
        statement = parser.parseSQLStatement();

        if (statement->getSQLStatementType() == "END") {
            delete statement;
            break;
        }

        previous_statement = statement;
    }

    unordered_map<string, string> attributes = previous_statement->getAttributes();
    string type = previous_statement->getSQLStatementType();

    delete previous_statement;

    return make_pair(type, attributes);
}

void MainWindow::getQuery()
{
    QString query_text = ui->CommandTextEdit->toPlainText();

    SQLScanner scanner(query_text.toStdString());
    SQLParser parser(&scanner);

    SQLStatement* statement;
    statement = parser.parseSQLStatement();

    unordered_map<string, string> queryAttributes = statement->getAttributes();

    string statement_type = statement->getSQLStatementType();

    // Measure time
    auto start = chrono::steady_clock::now();

    if (statement_type == "CREATE") {
        CreateTableStatement* createTableStatement = dynamic_cast<CreateTableStatement*>(statement);

        string table_name = queryAttributes["table_name"];
        string file_name = queryAttributes["file_name"];
        file_name = file_name.substr(1, file_name.size() - 2);
        string index_type = queryAttributes["index_type"];
        string indexed_column = queryAttributes["indexed_column"];

        if (index_type == "avl") {
            table_avl_name = table_name;
            createTableAVL(file_name, indexed_column);
        }
        else if (index_type == "sequential") {
            table_seq_name = table_name;
            createTableSeq(file_name, indexed_column);
        }
        else if (index_type == "hash") {
            table_hash_name = table_name;
            createTableHash(file_name, indexed_column);
        }

        delete createTableStatement;
        loadTablesNameGUI();
    }
    else if (statement_type == "SELECT") {
        SelectStatement* selectStatement = dynamic_cast<SelectStatement*>(statement);

        string table_name = queryAttributes["table_name"];
        Condition* condition = selectStatement->condition;

        if (table_name == table_avl_name) {
            selectTableAVL(table_name, condition);
        }
        else if (table_name == table_seq_name) {
            selectTableSeq(table_name, condition);
        }
        else if (table_name == table_hash_name) {
            selectTableHash(table_name, condition);
        }

        delete condition;
    }
    else if (statement_type == "INSERT") {
        InsertStatement* insertStatement = dynamic_cast<InsertStatement*>(statement);

        string table_name = queryAttributes["table_name"];
        vector<string> values = insertStatement->values;

        if (table_name == table_avl_name) {
            insertTableAVL(table_name, values);
        }
        else if (table_name == table_seq_name) {
            insertTableSeq(table_name, values);
        }
        else if (table_name == table_hash_name) {
            insertTableHash(table_name, values);
        }

        delete insertStatement;
    }
    else if (statement_type == "DELETE") {
        DeleteStatement* deleteStatement = dynamic_cast<DeleteStatement*>(statement);

        string table_name = queryAttributes["table_name"];
        Condition* condition = deleteStatement->condition;

        if (table_name == table_avl_name) {
            deleteTableAVL(table_name, condition);
        }
        else if (table_name == table_seq_name) {
            // selectTableSeqFile(file_name, table_name, index_type, indexed_column);
        }
        else if (table_name == table_hash_name) {
            // selectTableHash(file_name, table_name, index_type, indexed_column);
        }

        delete deleteStatement;
    }

    auto end = chrono::steady_clock::now();
    double duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

    QString text = "<html><head/><body><p align=\"center\"><span style=\" font-family:'Segoe UI'; font-size:14pt; font-weight:700; color:#ececec;\">";
    text += QString::number(duration) + " microsegundos";
    text += "</span></p></body></html>";
    ui->SecondsDelayedLabel->setText(text);
}

// CREATE

void MainWindow::createTableAVL(string file_name, string indexed_column) {
    table_avl = createAVLFromAtribute(file_name, "bookID");
}

void MainWindow::createTableHash(string file_name, string indexed_column) {
    table_hash = createExtendibleHashingFromAtribute(file_name, "bookID", 10);
}

void MainWindow::createTableSeq(string file_name, string indexed_column) {
    table_seq = createSeqFileFromAttribute(file_name, "meta.bin", "main.bin");
}

// CREATE FROM CSV

AVLTreeFile<int>* MainWindow::createAVLFromAtribute(const string& file_name, string attribute) {
    ifstream file(file_name);
    if (!file.is_open()) throw invalid_argument("Cannot open " + file_name + " file.");

    AVLTreeFile<int>* avlTreeFile = new AVLTreeFile<int>{"init.bin"};

    bool first = true;

    if (attribute == "bookID") {
        string line;

        Record record;

        while (getline(file, line)){
            if (!first) {
                record.read(line);

                NodeAVL<int> node(record);

                avlTreeFile->add(node);
            }
            else {
                first = false;
            }
        }
    }

    return avlTreeFile;
}

ExtendibleHashing<long>* MainWindow::createExtendibleHashingFromAtribute(const string& filename, string atribute, int depth) {
    ifstream file(filename);
    if (!file.is_open()) throw invalid_argument("Cannot open " + filename + " file.");

    bool first = true;

    ExtendibleHashing<long>* hash = new ExtendibleHashing<long>{filename.substr(0, filename.size() - 4), depth};

    if (atribute == "bookID") {
        string line;

        Record record;

        while (getline(file, line)){
            if (!first) {
                record.read(line);
                hash->insert(record.bookID, record);
            } else {
                first = false;
            }
        }
    }

    return hash;
}

SequentialFile* MainWindow::createSeqFileFromAttribute(const string& file_name, const string& meta_path, const string& main_path) {
    ifstream file(file_name);
    if (!file.is_open()) throw invalid_argument("Cannot open " + file_name + " file.");

    bool first = true;

    auto sf = new SequentialFile(meta_path, main_path, 300);

    string line;

    Record record;

    while (getline(file, line)){
        if (!first) {
            record.read(line);
            sf->insertRecord(record);
        } else {
            first = false;
        }
        cout << record.bookID << endl;
    }

    return sf;
}

// SELECT

void MainWindow::selectTableAVL(string table_name, Condition* condition) {
    vector<Record> records;

    if (condition->getConditionType() == "equals") {
        ConditionEquals* condition_equals = dynamic_cast<ConditionEquals*>(condition);
        NodeAVL<int> node = table_avl->search(stoi(condition_equals->value));

        records.push_back(node.record);
    }
    else if (condition->getConditionType() == "between") {
        ConditionBetween* condition_between = dynamic_cast<ConditionBetween*>(condition);
        vector<NodeAVL<int>> nodes = table_avl->rangeSearch(stoi(condition_between->left), stoi(condition_between->right));

        for (auto& node: nodes) {
            records.push_back(node.record);
        }
    }

    displayRecords(records);
}

void MainWindow::selectTableSeq(string table_name, Condition* condition) {
    vector<Record> records;

    if (condition->getConditionType() == "equals") {
        ConditionEquals* condition_equals = dynamic_cast<ConditionEquals*>(condition);
        std::optional<Record> record = table_seq->searchRecord(stoi(condition_equals->value));

        records.push_back(*record);
    }
    else if (condition->getConditionType() == "between") {
        ConditionBetween* condition_between = dynamic_cast<ConditionBetween*>(condition);
        vector<Record> rs = table_seq->rangeSearchRecords(stoi(condition_between->left), stoi(condition_between->right));

        for (auto& record: rs) {
            records.push_back(record);
        }
    }

    displayRecords(records);
}

void MainWindow::selectTableHash(string table_name, Condition* condition) {
    vector<Record> records;

    if (condition->getConditionType() == "equals") {
        ConditionEquals* condition_equals = dynamic_cast<ConditionEquals*>(condition);
        pair<bool, Record> found_record = table_hash->search(stoi(condition_equals->value));

        records.push_back(found_record.second);
    }
    else if (condition->getConditionType() == "between") {
        ConditionBetween* condition_between = dynamic_cast<ConditionBetween*>(condition);
        vector<Record> rs = table_hash->rangeSearch(stoi(condition_between->left), stoi(condition_between->right));

        for (auto& record: rs) {
            records.push_back(record);
        }
    }

    displayRecords(records);
}

// INSERT

void MainWindow::insertTableAVL(string table_name, vector<string> values) {
    Record record;
    string line = "";

    line = values[0] + "," + values[1] + "," + values[2] + "," + values[3];

    record.read(line);

    NodeAVL<int> node(record);
    table_avl->add(node);
}

void MainWindow::insertTableSeq(string table_name, vector<string> values) {
    Record record;
    string line = "";

    line = values[0] + "," + values[1] + "," + values[2] + "," + values[3];

    record.read(line);

    table_seq->insertRecord(record);
}

void MainWindow::insertTableHash(string table_name, vector<string> values) {
    Record record;
    string line = "";

    line = values[0] + "," + values[1] + "," + values[2] + "," + values[3];

    record.read(line);

    table_hash->insert(record.bookID, record);
}

// DELETE
void MainWindow::deleteTableAVL(string table_name, Condition* condition) {
    if (condition->getConditionType() == "equals") {
        ConditionEquals* condition_equals = dynamic_cast<ConditionEquals*>(condition);
        bool removed = table_avl->remove(stoi(condition_equals->value));
        cout << removed << endl;
    }
}

void MainWindow::displayRecords(vector<Record> &records) {
    table->setRowCount(records.size());

    for (int i = 0; i < records.size(); i++) {
        table->setItem(i, 0, new QTableWidgetItem(QString::number(records[i].bookID)));
        table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(string(records[i].title))));
        table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(string(records[i].authors))));
        table->setItem(i, 3, new QTableWidgetItem(QString::number(records[i].average_rating)));
    }

    QString text = "<html><head/><body><p align=\"center\"><span style=\" font-family:'Segoe UI'; font-size:14pt; font-weight:700; color:#ececec;\">";
    text += QString::number(records.size()) + " records";
    text += "</span></p></body></html>";

    ui->AmountRecordsLabel->setText(text);
}
