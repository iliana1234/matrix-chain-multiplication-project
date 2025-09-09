#ifndef MATRIXINPUT_H
#define MATRIXINPUT_H

#include "matrixload.h"
#include "MatrixChainSolve.h"

class MatrixInput
{
public:
    MatrixInput(MatrixChainSolve *solve, MatrixLoad *load);

    void inputMatrix(); // Loads matrices in from manual input

private:
    MatrixChainSolve *matrixSolve;   // Declare the pointer variable pointing to the MatrixChainSolve object
    MatrixLoad *matrixLoad;          // Declare the pointer variable pointing to the MatrixLoad object
    void updateRowCol(int numCols);
    bool validateMatr(const QVector<QVector<int>>& allMatrices, int prevCols, int numRows);
    bool askToSaveMatr();
    bool askToAddAnotherMatr();
};

#endif // MATRIXINPUT_H
