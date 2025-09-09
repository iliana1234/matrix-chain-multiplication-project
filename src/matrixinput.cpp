#include "matrixinput.h"
#include <QFileDialog>             // For file dialog handling
#include <QTextStream>             // For file reading/writing in file streams
#include <QMessageBox>             // For displaying Q message boxes
#include <QDir>                    // For creating opening directories
#include <QRegularExpression>      // To match strings
#include <QInputDialog>            // To get the input of the user

// Constructor to set the passed parameter value to the MatrixChainSolve and MatrixLoad member variables of type pointer to object
// It has two input parameters: MatrixChainSolve *solve and MatrixLoad *load which are pointers to a MatrixChainSolve and MatrixLoad objects
MatrixInput::MatrixInput(MatrixChainSolve *solve, MatrixLoad *load)
    : matrixSolve(solve), matrixLoad(load) {}

// Method to manualy input matrices. It accepts no parameters and it has no return type.
// It's purpose is to take input of the user for the rows and columns of a matrix, to make sure that all input is valid, to convert the
// user entered string into integers, to append the new matrix to a temporarily declared matrix vector and call setAllMatrices
// with it as an argument.
void MatrixInput::inputMatrix() {
    QVector<QVector<int>> allMatrTemp = matrixSolve->getAllMatrices(); // Copy the allMatrices to a temporary vector to hold all matrices in
    bool isContinueInput = true;

    // Clear any previous saved data in the memory
    matrixSolve->clearCostOrder();

    while (isContinueInput) {
        // Ask user to input nuber of rows if ok is not pressed cancel the dialog window
        bool isOk;
        int numRows = QInputDialog::getInt(nullptr, "Input Matrix", "Enter number of rows:", 1, 1, 100, 1, &isOk);
        if (!isOk) return;

        // Ask user to input nuber of columns if ok is not pressed cancel the dialog window
        int numCols = QInputDialog::getInt(nullptr, "Input Matrix", "Enter number of columns:", 1, 1, 100, 1, &isOk);
        if (!isOk) return;

        QVector<int> matr; // Declare temporary matrix vector
        // For each row ask user to input the given number of columns
        // Each matrix must have the same number of rows as the previous matrix's columns
        for (int row = 0; row < numRows; ++row) {
            QString strRow = QInputDialog::getText(nullptr, "Matrix Input",
                                                   QString("Enter row %1 values (space-separated):").arg(row + 1), QLineEdit::Normal, "", &isOk);
            if (!isOk) return;

            // Make sure user returns same amount of numbers as the amount of columns
            QStringList rowsVal = strRow.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts); // Make individual numbers space seperated
            if (rowsVal.size() != numCols) {
                QMessageBox::warning(nullptr, "Input Error", "Incorrect number of values for row.");
                return;
            }

            // For each value in the row convert it to integer
            for (int col = 0; col < rowsVal.size(); ++col) {
                const QString valueInput = rowsVal.at(col);
                bool isNumb;
                int valueInt = valueInput.toInt(&isNumb);
                // In case a value is not in the valid format meaning it cannot be turned to int close the dialog box
                if (!isNumb) {
                    QMessageBox::warning(nullptr, "Input Error", "Invalid format of number.");
                    return;
                }
                matr.append(valueInt); // If the number is valid format append it to the temporary matrix vector
            }
        }

        allMatrTemp.append(matr);  // Append the current matrix to the temporary vector

        QVector<int> rowColstemp = matrixSolve->getMatrRowsCols(); // Copy the matrRowsCols to a temporary vector to hold rows and cols in
        // Add rows to the matrix rows and columns vector if this is the first matrix added for the current run
        if (rowColstemp.isEmpty()) {
            rowColstemp.append(numRows);                // Add first matrix's rows in case the rowColstemp is empty
            matrixSolve->setMatrRowsCols(rowColstemp); // Add the matrix columns to the vector
        } else {
            int prevCols = rowColstemp.back();                      // Use latest column value from the vector
            if (!validateMatr(allMatrTemp, prevCols, numRows)) {    // Raise warning in case the current matrix rows isn't equal to previous matrix columns
                return;  // Stop input if the rows don't match the columns
            }
        }
        updateRowCol(numCols);

        if (!askToAddAnotherMatr()) {
            isContinueInput = false;
        }
    }
    matrixSolve->setAllMatrices(allMatrTemp);  // After all matrices are set set the real vector

    if (askToSaveMatr()) {
        // Save the matrices to a file
        matrixLoad->saveMatricesToFile();
    }
}

//This method takes 1 parameter of type int,the number of columns.
//It has no return value. It declares a QVector of type int and it appends the number of columns to it
//After that it calls setMatrRowsCols with the temporary variable it initialzed as the argument.
void MatrixInput::updateRowCol(int numCols) {
    QVector<int> rowColsTemp = matrixSolve->getMatrRowsCols();
    rowColsTemp.append(numCols);                         // Append only columns after the first matrix in the temporary vector
    matrixSolve->setMatrRowsCols(rowColsTemp);            // Add the matrix columns to the vector
}

//This method takes 3 parameters. The first is a 2D QVector allMatrices, the other two are of type int
//for the previous columns and rows. It checks whether the prevCols are != to the numRows and prints out a warning if they don't.
//It has a boolean return type. It returns false if the columns and rows aren't equal and true if they are.
bool MatrixInput::validateMatr(const QVector<QVector<int>>& allMatrices, int prevCols, int numRows) {
    if (prevCols != numRows) {
        QMessageBox::warning(nullptr, "Matrix Dimension Error",
                             QString("Matrix number: %1 has %2 rows, but previous matrix has %3 columns. Matrix number: %1 rows must be equal to matrix number: %4 columns.")
                                 .arg(allMatrices.size())      // This is %1
                                 .arg(numRows)                 // This is %2
                                 .arg(prevCols)                // This is %3
                                 .arg(allMatrices.size()-1));  // This is %4
        return false;
    }
    return true;
}

//This method accepts no parameters. It's purpose is to initialize a QMessageBox and ask the user if they want
//to add another matrix. It's return type is boolean, returned true if the user inputs 'Yes'.
bool MatrixInput::askToAddAnotherMatr() {
    // Ask if the user wants to input another matrix
    QMessageBox::StandardButton answer;
    answer = QMessageBox::question(nullptr, "Add Another Matrix",
                                    "Would you like to input another matrix?",
                                    QMessageBox::Yes | QMessageBox::No);
    return answer == QMessageBox::Yes;
}


//This method accepts no parameters. It's purpose is to initialize a QMessageBox and ask the user if they want to save
//the matrcices into a file. It's return type is boolean, and the method returns true if the user inputs 'Ok'.
bool MatrixInput::askToSaveMatr() {
    // Check if the user wants to save the matrices to a file
    QMessageBox::StandardButton answer;
    answer = QMessageBox::question(nullptr, "Save Matrix",
                                   "Do you want to save the matrices you added?",
                                   QMessageBox::Ok | QMessageBox::Cancel);
    return answer == QMessageBox::Ok;
}
