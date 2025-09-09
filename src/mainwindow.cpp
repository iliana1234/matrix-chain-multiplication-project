#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "matrixload.h"
#include "matrixinput.h"
#include "MatrixChainSolve.h"
#include <QFileDialog>          // For file dialog handling
#include <QMessageBox>         // For displaying Q message boxes

// Load some default statuses on the widgets when program is first run
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    // Create objects of the other classes and pass the needed objects to their constructors
    matrixSolve = new MatrixChainSolve();
    matrixLoad = new MatrixLoad(matrixSolve);
    matrixInput = new MatrixInput(matrixSolve, matrixLoad);

    ui->setupUi(this);

    this->setWindowTitle("Matrix Chain Multiplication"); // Change the title of the window

    // Connect slot functions to the buttons on click events
    connect(ui->readMatrButton, &QPushButton::clicked, this, &MainWindow::loadMatrFromFile);
    connect(ui->solveMatrButton, &QPushButton::clicked, this, &MainWindow::solveMatrices);
    connect(ui->inputMatrButton, &QPushButton::clicked, this, &MainWindow::inputMatr);

    // Connect the ok and cancel buttons to their functions
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &MainWindow::okButton);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &MainWindow::cancelButton);

    // Disable the solve matrix button until a file is loaded or the user has put an input
    ui->solveMatrButton->setEnabled(false);
    // Set default text in parenthesizationLabel
    ui->parenthesizationLabel->setText("Optimal Parenthesization:");

    // Add the items in the combobox
    ui->chooseModeComboBox->addItem("Load from file");
    ui->chooseModeComboBox->addItem("User input");

    ui->chooseModeComboBox->setCurrentIndex(0); // Set a default item
    ui->stackedWidget->setCurrentIndex(0); // Set a default widget
    connect(ui->chooseModeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            ui->stackedWidget, &QStackedWidget::setCurrentIndex);
}

MainWindow::~MainWindow() {
    delete ui;
    // Delete the created object pointers stored in the heap when the program is closed
    delete matrixLoad;
    delete matrixInput;
    delete matrixSolve;
}

// This slot function opens and loads Matrices from a file, if the matrices were loaded correctly it enables the Solve matrix button
// The function is void so it has no return value and it has no input parameters
void MainWindow::loadMatrFromFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Matrix File"); // Open file selection dialog
    matrixLoad->loadMatrices(fileName); // Call function that reads the matrices and pass the file location of the opened file

    // In case matrises were read successfully enable the solve matrix button
    if (!matrixSolve->getAllMatrices().isEmpty() && matrixSolve->getMatrRowsCols().size() > 1) {
        ui->solveMatrButton->setEnabled(true);
    }
}

// Function to manualy input matrices using dialog boxes, if at least one matrix was loaded correctly it enables the Solve matrix button
// The function is void so it has no return value and it has no input parameters
void MainWindow::inputMatr() {
    matrixInput->inputMatrix();
    // If the matrix was input correctly set the solve matrix button to be enabled
    if (!matrixSolve->getAllMatrices().isEmpty() && matrixSolve->getMatrRowsCols().size() > 1) {
        ui->solveMatrButton->setEnabled(true);
    }
}

// This function solves the matrix chain multiplication problem, the optimal order and minimal cost using the stored matrices.
// It then displays the optimal multiplication order, minimal cost and final resulting matrix in the UI tables.
// The function is void so it has no return value and it has no input parameters
void MainWindow::solveMatrices() {
    QVector<QVector<int>> matrRes = matrixSolve->solveMatrices();
    visualizeOrderCost();           // Print the results in the two tables
    displayMatrResult(matrRes);    // Print the final matrix result
    displayOptParenthesization(); // Print the optimal parenthesization
}

// Display the values from the optimal value and optimal cost vectors to the UI tables
// The function is void so it has no return value and it has no input parameters
void MainWindow::visualizeOrderCost() {
    const QVector<QVector<int>>&  cost = matrixSolve->getCostMatr();
    const QVector<QVector<int>>&  order = matrixSolve->getOrderMatr();
    int size = cost.size();  // Get the size of the cost vector

    ui->costTable->setRowCount(size);    // Create the number of the Rows in the table
    ui->costTable->setColumnCount(size); // Create the number of the Columns in the table

    ui->orderTable->setRowCount(size);    // Create the number of the Rows in the table
    ui->orderTable->setColumnCount(size); // Create the number of the Columns in the table

    // Loop through all values in the cost and order vectors and display them in the order and cost tables
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (j >= i) {
                // Fill the rows and columns of the tables with the current values of the vectors
                ui->costTable->setItem(i, j, new QTableWidgetItem(QString::number(cost[i][j])));      // Fill the table with current element in vector
                ui->orderTable->setItem(i, j, new QTableWidgetItem(QString::number(order[i][j] + 1)));// Fill the table with current element in vector + 1 to be more readable
            }
        }
    }
}

// Display the optimal parenthesization for the matrix chain multiplication to the parenthesizationLabel UI label
// The function is void so it has no return value and it has no input parameters
void MainWindow::displayOptParenthesization() {
    QString parenthesization = matrixSolve->getOptParenthesization();  // Get the calculated parenthesization value
    ui->parenthesizationLabel->setText("Optimal Parenthesization: " + parenthesization); // // Set the calculated parens to the parenthesizationLabel label
}

// Display the final matrix result in the resultMatrTable UI table
// It has one parameter: const QVector<QVector<int>> &matr a 2D vector holding the values of the result of the multiplication
void MainWindow::displayMatrResult(const QVector<QVector<int>> &matr) {
    int numRows = matr.size();      // Gets the nmber of rows
    int numCols = matr[0].size();   // Gets the number of columns

    // Sets the row and coumn counts in the widget to be of the same dimensions as the matrix
    ui->resultMatrTable->setRowCount(numRows);
    ui->resultMatrTable->setColumnCount(numCols);

    // Adds a new QTableWidget, converting the values of the matrix to a string
    // and places them in the correct cells of the widget table
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            ui->resultMatrTable->setItem(i, j, new QTableWidgetItem(QString::number(matr[i][j])));
        }
    }
}

// This function keeps all saved data in case ok is clicked
// The function is void so it has no return value and it has no input parameters
void MainWindow::okButton() {
    // When the ok button is clicked the program is closed
    QApplication::quit(); // Close the program
}

// This function deletes all saved files in the directory for the current run of the program
// The function is void so it has no return value and it has no input parameters
void MainWindow::cancelButton() {
    const QVector<QString>& savedFiles = matrixLoad->getSavedFiles();

    // Check if the savedFiles vector is empty
    if (savedFiles.isEmpty()) {
        // If no files were saved, close the program directly
        QApplication::quit(); // Close the program
    } else {
        // Ask the user for confirmation for saved files to be removed
        QMessageBox::StandardButton answer;
        answer = QMessageBox::question(this, "Confirm Cancel",
                                       "Are you sure you want to cancel all saved files?",
                                       QMessageBox::Yes | QMessageBox::No);

        if (answer == QMessageBox::Yes) {
            // Loop through all saved file paths in the savedFiles vector
            for (int i = 0; i < savedFiles.size(); ++i) {
                QFile::remove(savedFiles[i]);  // Remove the current saved file
            }

            QApplication::quit(); // Close the program
        }
    }
}
