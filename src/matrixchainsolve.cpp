#include "MatrixChainSolve.h"
#include <QMessageBox>         // For displaying warnings
#include <QRegularExpression>  // To match strings
#include <QString>             // For QString
#include <QVector>             // For QVector
#include <QStringList>         // For the string splitting
#include <climits>             // For INT_MAX use

// This function accepts no parameters. It calls optimalOrderCost
// and it returns a QVector of the return of the optimalMultiplications function.
QVector<QVector<int>> MatrixChainSolve::solveMatrices() {
    optimalOrderCost();  // Calculate and print the optimal order and optimal cost tables

    return optimalMultiplication(0, allMatrices.size() - 1); // Calculate the final matrix result
}

// It calculates the optimal order and the minimal costs of the multiplication of the chain matrices, and fills in the cost and order vectors.
// The function has no parameters.
// The function is void so it has no return value, its purpose is to fill the two vectors order, cost with the calculations
void MatrixChainSolve::optimalOrderCost() {
    int size = matrRowsCols.size() - 1; // Get the number of matrices -1 to remove the rows
    clearCostOrder();
    cost.resize(size, QVector<int>(size, 0)); // Initialize the cost vector to have size number of rows and cols and set their values to 0
    order.resize(size, QVector<int>(size, 0)); // Initialize the order vector to have size number of rows and cols and set their values to 0

    // Calculate the optimal multiplication order and minimal cost for all matrices
    for (int l = 2; l <= size; ++l) { // There must be at leas 2 matrices, if there is only one matrix there is no need to calculate
        for (int i = 0; i < size - l + 1; ++i) {
            int j = i + l - 1;
            cost[i][j] = INT_MAX; // Set the initial minimal cost very hight to set it lower later
            for (int k = i; k < j; ++k) {
                int q = cost[i][k] + cost[k + 1][j] + matrRowsCols[i]*matrRowsCols[k+1]*matrRowsCols[j+1]; // Multiply the matrices dimenstions
                // If the current cost is less than the one already filled in the optimal cost vector then fill the new value in it
                if (q < cost[i][j]) {
                    cost[i][j] = q; // Fill the optimal cost vector with the current result of multiplication
                    order[i][j] = k; // Fill the optimal order with the current result of k
                }
            }
        }
    }
}

// This function has no parameters. It declares a QString paren and calls calcParens function passing it as the third argument.
// It returns paren.
QString MatrixChainSolve::getOptParenthesization() {
    QString paren; // Declare a string to hold the parens
    calcParens(0, allMatrices.size() - 1, paren); // Call function to calculate the parenthesizationLabel
    return paren;
}

//This function takes 2 integers, the i iterrator and the j iterrator and a QString paren as parameters and it has no return type.
//Function to calculate the parenthesization using a reference to the parens string.
void MatrixChainSolve::calcParens(int i, int j, QString &paren) {
    // In case there is only one matrix just concatenate A and i+1 to make to set the parens string as A1
    if (i == j) {
        paren += "A" + QString::number(i + 1);
        // In case there are more than one matrices
    } else {
        paren += "("; // Start the string with (
        // Recursively add the first and second half of the order vector which holds the optimal order of multiplications
        calcParens(i, order[i][j], paren);
        calcParens(order[i][j] + 1, j, paren);
        paren += ")"; // End the string with )
    }
}

//This function takes two paramets of type int, an i iterrator and a j iterrator.
//It's return type is a 2 dimensinal QVector. It recursively finds the optimal matrix chain multiplication for the matrices
//i to j. It uses the order table to find the optimal solit point to minimize the amount
//of multiplications
//It returns the resulting QVector optimal multiplication.
QVector<QVector<int>> MatrixChainSolve::optimalMultiplication(int i, int j) {
    // If theres only one matrix it returns it as a 2D matrix
    if (i == j) {
        int numRows = matrRowsCols[i];      // Number of rows for matrix i
        int numCols = matrRowsCols[i + 1];  // Number of columns for matrix i
        QVector<QVector<int>> matr(numRows, QVector<int>(numCols));
        // Reconstructs the matrix from a 1D format
        for (int r = 0; r < numRows; ++r) {
            for (int c = 0; c < numCols; ++c) {
                matr[r][c] = allMatrices[i][r * numCols + c];
            }
        }
        return matr;
    }

    // Splits the matrix chain at its optimal point and it recursively finds the products
    // of the left and right subchains
    QVector<QVector<int>> leftMatr = optimalMultiplication(i, order[i][j]);
    QVector<QVector<int>> rightMatr = optimalMultiplication(order[i][j] + 1, j);

    int numRows = leftMatr.size();          // rows of the left matrix
    int cols = rightMatr[0].size();         // columns of the right matrix
    int shareDims = leftMatr[0].size();     // the shared dimesion

    // Initialzing the resulting matrix
    QVector<QVector<int>> matrRes(numRows, QVector<int>(cols, 0));

    // Matrix multiplication
    for (int r = 0; r < numRows; ++r) {
        for (int c = 0; c < cols; ++c) {
            for (int k = 0; k < shareDims; ++k) {
                matrRes[r][c] += leftMatr[r][k] * rightMatr[k][c];
            }
        }
    }

    return matrRes;     // returns the result
}

// Clear data from member variables
void MatrixChainSolve::clearCostOrder() {
    cost.clear();
    order.clear();
}
void MatrixChainSolve::clearMatrData() {
    allMatrices.clear();
    matrRowsCols.clear();
}

// Create the needed getters
const QVector<QVector<int>>& MatrixChainSolve::getAllMatrices() const {
    return allMatrices;
}
const QVector<int>& MatrixChainSolve::getMatrRowsCols() const {
    return matrRowsCols;
}
const QVector<QVector<int>>& MatrixChainSolve::getCostMatr() const {
    return cost;
}

const QVector<QVector<int>>& MatrixChainSolve::getOrderMatr() const {
    return order;
}

// Create the needed setters
void MatrixChainSolve::setAllMatrices(const QVector<QVector<int>>& matrices) {
    allMatrices = matrices;
}
void MatrixChainSolve::setMatrRowsCols(const QVector<int>& rowsCols) {
    matrRowsCols = rowsCols;
}
