#ifndef MATRIX_CHAINSOLVER_H
#define MATRIX_CHAINSOLVER_H

#include <QVector>

class MatrixChainSolve {
public:
    QVector<QVector<int>> solveMatrices();
    QString getOptParenthesization();

    // Declare the getters for the member variables
    const QVector<QVector<int>>& getAllMatrices() const;
    const QVector<int>& getMatrRowsCols() const;
    const QVector<QVector<int>>& getCostMatr() const;
    const QVector<QVector<int>>& getOrderMatr() const;

    // Declare the setters for the member variables
    void setAllMatrices(const QVector<QVector<int>>& matrices);
    void setMatrRowsCols(const QVector<int>& dimensions);
    // To clear member variable data
    void clearCostOrder();
    void clearMatrData();
private:
    QVector<QVector<int>> allMatrices;  // Holds all matrices
    QVector<int> matrRowsCols;          // Holds matrix row/col sizes
    QVector<QVector<int>> cost, order;  // Minimal costs and optimal order vectors
    QVector<QVector<int>> optimalMultiplication(int i, int j);      // Calculation of the final matrix result

    void optimalOrderCost();                                        // Calculate the optimal multiplication order and cost
    void calcParens(int i, int j, QString &parens);                 // Calculate the Optimal Parenthesization
};

#endif // MATRIX_CHAINSOLVER_H
