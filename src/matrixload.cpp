#include "matrixload.h"
#include <QFileDialog>             // For file dialog handling
#include <QTextStream>             // For file reading/writing in file streams
#include <QMessageBox>             // For displaying Q message boxes
#include <QDir>                    // For creating opening directories
#include <QRegularExpression>      // To match strings
#include <QInputDialog>            // To get the input of the user

// Hold the names of the matrix file and folder used for the file path
const QString MatrixLoad::MATRIX_FOLDER_NAME = "Matrices";
const QString MatrixLoad::MATRIX_FILE_NAME = "saved_matrices";

// Constructor to set the passed parameter value to the MatrixChainSolve member variable of type pointer to object
// It has one input parameter: MatrixChainSolve *solve a pointer to a MatrixChainSolve object
MatrixLoad::MatrixLoad(MatrixChainSolve *solve) : matrixSolve(solve) {}

// Open a .txt file and set the matrices and rows/columns to the allMatrices and matrRowsCols member variables of type vector
// It has one parameter: const QString &filename that contains the directory of the file that was chosen from the user
// It is declared as constant to prevent ac
// The function is void so it has no return value, its purpose is to fill the two vectors with the data from the passed .txt file
void MatrixLoad::loadMatrices(const QString &filename) {
    QFile file(filename); // Create a file object to hold the file in
    // Try to open the file in read mode print warning message and return in case we weren't able to open the file
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "File open Error", "File failed to open for reading.");
        return;
    }

    QTextStream in(&file); // Open file stream to read from
    // Clear any previous saved data in the memory
    matrixSolve->clearMatrData();
    matrixSolve->clearCostOrder();

    int numRows, numCols;               // Declare variables to store the number of rows and columns of the matrices
    bool isFirstMatrix = true;          // Used to know if we are reading the first matrix
    QVector<QVector<int>> allMatrTemp;  // Temporary vector to hold all matrices in
    QVector<int> rowsColsTemp;          // Temporary vector to hold rows and cols in

    // Start reading from the file until the end is reached
    while (!in.atEnd()) {
        in >> numRows >> numCols; // Read the rows and columns of the matrix
        QVector<int> matr(numRows * numCols); // Store rows and columns temporarily in a 1d vector
        for (int i = 0; i < numRows * numCols; ++i)
            in >> matr[i]; // Read all matrix elements

        // After reading the current matrix check if the dimentions are correctly input
        if (rowsColsTemp.size() >= 2) { // Check if there is more than 1 matrix
            int prevCols = rowsColsTemp.last(); // Get last matrix's columns
            if (prevCols != numRows) {
                QMessageBox::warning(nullptr, "Matrix Dimension Error",
                                     QString("Matrix number: %1 has %2 rows, but the previous matrix has %3 columns. Matrix number: %1 rows must be equal to matrix number: %4 columns.")
                                         .arg(allMatrTemp.size())      // This is %1
                                         .arg(numRows)                 // This is %2
                                         .arg(prevCols)                // This is %3
                                         .arg(allMatrTemp.size()-1));  // This is %4
                file.close();
                return;
            }
        }

        allMatrTemp.append(matr); // Append the current matrix to the temporary vector

        if (isFirstMatrix) {
            rowsColsTemp.append(numRows);   // Add the first matrix's row to the vector
            isFirstMatrix = false;
        }
        rowsColsTemp.append(numCols);       // Append the current matrix's columns to the vector

    }
    file.close(); // Close the file


    matrixSolve->setMatrRowsCols(rowsColsTemp);  // Append the rows and cols to the real vector
    matrixSolve->setAllMatrices(allMatrTemp);    // After all matrices are set set the real vector

    // Check if the user wants to save the matrices to a file
    QMessageBox::StandardButton answer;
    answer = QMessageBox::question(nullptr, "Save Matrices",
                                   "Do you want to save the matrices you selected?",
                                   QMessageBox::Ok | QMessageBox::Cancel);
    if (answer == QMessageBox::Ok) {
        // Save the matrices to a file
        saveMatricesToFile();
    }
}

// Method that saves the matrices from the allMatrices and matrRowsCols vectors to files matrices to files
// The function is void so it has no return value and it has no input parameters
// Its purpose is to Iterate through information from member variables of type vector and set it to .txt files
void MatrixLoad::saveMatricesToFile() {
    const QVector<QVector<int>>& allMatrices = matrixSolve->getAllMatrices();
    const QVector<int>& matrRowsCols = matrixSolve->getMatrRowsCols();

    // Check if the folder exists and if it doesnt create it
    QDir dir;
    if (!dir.exists(MATRIX_FOLDER_NAME)) {
        dir.mkdir(MATRIX_FOLDER_NAME);
    }

    int nextID = getFileID(); // Find the last ID + 1 of all saved files and return it

    // Create the file path
    QString filePath = MATRIX_FOLDER_NAME + "/" + MATRIX_FILE_NAME + "_" + QString::number(nextID) + ".txt";
    QFile outFile(filePath); // Create a file object of the file path

    if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {  // Open the file to write in it
        QTextStream out(&outFile);  // Open the file output stream

        // Iterate through the matrices to save them in the new file
        for (int i = 0; i < allMatrices.size(); ++i) {
            int numRows = matrRowsCols[i];       // Get the number of rows of the current matrix
            int numCols = matrRowsCols[i + 1];   // Get the number of columns of the current matrix
            out << numRows << " " << numCols << "\n";  // Write the number of matrix rows and columns in the file

            const QVector<int> &matr = allMatrices[i];  // Get the matrix data of current matrix
            for (int val : matr) {
                out << val << " ";  // Write the values of the matrix
            }
            out << "\n"; // End the line after each matrix
        }

        outFile.close();  // Close the file

        // Add each saved file path to the saved files vector, to delete later if cancel button was clicked
        savedFiles.append(filePath);
    } else {
        QMessageBox::warning(nullptr, "Error", "Couldn't save the matrices to a file.");
    }
}
// Getter to access savedFiles getter outside of this class
const QVector<QString>& MatrixLoad::getSavedFiles() const {
    return savedFiles;
}
// Get the current file ID by accessing all files to a given directory and finding the highest ID of the .txt files with a given name in the directory
// This function has no input parameters
// The function has a return type integer that returns the value of the found highest ID +1 to use it when creating the next file
int MatrixLoad::getFileID() {
    QDir dir(MatrixLoad::MATRIX_FOLDER_NAME);  // Get the name of the folder that is used
    QFileInfoList files = dir.entryInfoList(QDir::Files);  // Get all the files in the folder

    int maxFileID = 0;
    // Match the file names to the given string
    QRegularExpression regex("saved_matrices_(\\d+)\\.txt");

    // Loop through all files in the folder to find the highest file ID
    for (int i = 0; i < files.size(); ++i) { // Iterate until all files
        const QFileInfo &fileInfo = files.at(i); // Reference the copy of the current file
        QString fileName = fileInfo.fileName(); // Get the name of the file only
        QRegularExpressionMatch match = regex.match(fileName); // try to match the current file to: "saved_matrices_(\\d+)\\.txt"

        // If the file name of the current file was matched
        if (match.hasMatch()) {
            int lastID = match.captured(1).toInt(); // Get the captured value and turn it to int
            maxFileID = qMax(maxFileID, lastID); // Get the highest found id between the lastID maxFileID
        }
    }

    return maxFileID + 1;  // Return the highest found ID + 1 to create the new avaiable ID
}
