#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <regex>
#include <QRegularExpression>
#include <cmath>
#include <QFile>
#include <QFileDialog>
#include <QStandardItem>
using namespace std;



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    operatorCounts.clear();
    variableCounts.clear();
    ui->textBrowser->clear();
    ui->textBrowser_2->clear();
    ui->textBrowser_5->clear();
    currentText=ui->textEdit->toPlainText().toStdString();

}
vector<string> extractVariables(string line) {
    vector<string> variables;
    regex varRegex(R"((\b[a-zA-Z_]\w*\b)\s*=\s*)");
    sregex_iterator regexIt(line.begin(), line.end(), varRegex);
    sregex_iterator endRegexIt;

    while (regexIt != endRegexIt) {
        variables.push_back((*regexIt)[1]);
        ++regexIt;
    }

    return variables;
}

// Function to count variable usage in a line of code using regex
void countVariableUsage(string line, unordered_map<string, int>& variableCounts) {
    for (auto& it : variableCounts) {
        regex usageRegex(R"(\b)" + it.first + R"(\b)");
        sregex_iterator regexIt(line.begin(), line.end(), usageRegex);
        sregex_iterator endRegexIt;
        while (regexIt != endRegexIt) {
            it.second++;
            ++regexIt;
        }
    }
}

int countSemicolons(string code) {
    regex semicolonRegex(";");
    return distance(sregex_iterator(code.begin(), code.end(), semicolonRegex),
        sregex_iterator());
}

void countOperators(string line, unordered_map<string, int>& operatorCounts) {
    // Arithmetic operators
        regex opRegex(R"(([+\-*/%])|(\+\+)|(--))");
        sregex_iterator regexIt(line.begin(), line.end(), opRegex);
        sregex_iterator endRegexIt;
        while (regexIt != endRegexIt) {
            operatorCounts[regexIt->str()]++;
            ++regexIt;
        }

        // Comparison operators
        opRegex = regex(R"(([!=]=)|(<=)|(>=)|(<>)|(<)|(>))");
        regexIt = sregex_iterator(line.begin(), line.end(), opRegex);
        while (regexIt != endRegexIt) {
            operatorCounts[regexIt->str()]++;
            ++regexIt;
        }

        // Logical operators
        opRegex = regex(R"(&&|\|\||!)");
        regexIt = sregex_iterator(line.begin(), line.end(), opRegex);
        while (regexIt != endRegexIt) {
            operatorCounts[regexIt->str()]++;
            ++regexIt;
        }

        // Bitwise operators
        opRegex = regex(R"([&|^~])");
        regexIt = sregex_iterator(line.begin(), line.end(), opRegex);
        while (regexIt != endRegexIt) {
            operatorCounts[regexIt->str()]++;
            ++regexIt;
        }

        // Semicolon operator
        opRegex = regex(R"(;)"); // new regex for semicolon operator
        regexIt = sregex_iterator(line.begin(), line.end(), opRegex);
        while (regexIt != endRegexIt) {
            operatorCounts[regexIt->str()]++;
            ++regexIt;
        }

        // Conditional jump operators
        opRegex = regex(R"(\b(if|else|while|do|for|break|continue)\b)");
        regexIt = sregex_iterator(line.begin(), line.end(), opRegex);
        while (regexIt != endRegexIt) {
            operatorCounts[regexIt->str()]++;
            ++regexIt;
        }
}
void MainWindow::findVariables(){
    string inp=currentText;
    vector<string> variables = extractVariables(inp);


    for (const auto& variable : variables) {
        variableCounts[variable] = 0;
    }

    countVariableUsage(inp, variableCounts);

    QString mainMsg="";

    for (const auto& variable : variableCounts) {
        mainMsg+= QString::fromStdString(variable.first) + " appears " + QString::number(variable.second) + " times." + "\n";
    }

    ui->textBrowser_2->setText(mainMsg);
   }

void MainWindow::findOperators(){

    string inp = ui->textEdit->toPlainText().toStdString();


    countOperators(inp, operatorCounts);

    QString mainMsg="";
    mainMsg+="Operator counts: \n" ;
    for (const auto& it : operatorCounts) {
        mainMsg+= QString::fromStdString(it.first) + ": " + QString::number(it.second) +"\n";
    }

    ui->textBrowser_5->setText(mainMsg);
}



void MainWindow::on_pushButton_2_clicked()
{
    auto fileName=QFileDialog::getOpenFileName(0,"Выберите файл");
    QFile file(fileName);

    if(file.open(QIODevice::ReadOnly)){
        QTextStream stream(&file);

        currentText.clear();

        string line;
        while(!stream.atEnd()){
            line=stream.readLine().toStdString();
            currentText+=line;
            currentText+='\n';
        }
    }

    else{
        qDebug()<<"file not opened";
    }


    file.close();

    ui->textEdit->setText(QString::fromStdString(currentText));
}

void MainWindow::on_textEdit_cursorPositionChanged()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    ui->label_2-> setText("Стр "+QString::number(cursor.blockNumber())+" Стл "+QString::number(cursor.positionInBlock()));
}


void MainWindow::on_pushButton_3_clicked()
{

    findVariables();
}


void MainWindow::on_pushButton_5_clicked()
{
    findOperators();
}


void MainWindow::countSpecialMetrics(){
    int uniqueVariables = variableCounts.size();
    int uniqueOperators = operatorCounts.size();

    int sumVariables = 0;

    for (auto& elem : variableCounts) {
         sumVariables += elem.second;
    }

    int sumOperators = 0;

    for (auto& elem : operatorCounts) {
         sumOperators += elem.second;
    }

    double programmScope = (sumOperators+sumVariables)*log2(uniqueVariables+uniqueOperators);

    QString msg = "Словарь программы: " + QString::number(uniqueVariables)
            + " + " + QString::number(uniqueOperators) + " = "
            + QString::number(uniqueOperators+uniqueVariables)+ "\n"
            + "Длина программы: " + QString::number(sumVariables)
            + " + " + QString::number(sumOperators) + " = "
            + QString::number(sumVariables+sumOperators)+ "\n"
            + "Объем программы: " +QString::number(sumOperators+sumVariables)
            + " * log2(" + QString::number(uniqueOperators+uniqueVariables)+
            ") = " +QString::number(programmScope);

    ui->textBrowser->setText(msg);
}

void MainWindow::on_pushButton_4_clicked()
{
    countSpecialMetrics();
}

