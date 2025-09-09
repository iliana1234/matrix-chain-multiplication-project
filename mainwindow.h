#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "matrixload.h"
#include "matrixinput.h"
#include "MatrixChainSolve.h"

#include <QMainWindow>
#include <QVector>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    // Create constructor and destructor
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Create all functions that respond to signals/are connected to UI events
    void loadMatrFromFile();
    void solveMatrices();
    void inputMatr();
    void okButton();
    void cancelButton();

private:
    Ui::MainWindow *ui;

    void displayMatrResult(const QVector<QVector<int>> &matrix); // Displays the final result of the matrix calculations
    void visualizeOrderCost();                                   // Displays the order and cost results
    void displayOptParenthesization();                           // Displays the optimal parenthesization

    // Declare the needed pointers to the objects of the other classes
    MatrixChainSolve *matrixSolve;
    MatrixLoad *matrixLoad;
    MatrixInput *matrixInput;

};

#endif // MAINWINDOW_H
