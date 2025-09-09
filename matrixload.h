#ifndef MATRIXLOAD_H
#define MATRIXLOAD_H

#include "MatrixChainSolve.h"

#include <QVector>

class MatrixLoad {
public:
    MatrixLoad(MatrixChainSolve *solve);

    void loadMatrices(const QString &filename);         // Opens and reads matrices from a .txt file
    void saveMatricesToFile();                          // Saves matrices to a .txt file
    const QVector<QString>& getSavedFiles() const;      // Getter to access savedFiles getter outside of this class
private:
    static const QString MATRIX_FOLDER_NAME;    // String to hold folder name for saved matrix files
    static const QString MATRIX_FILE_NAME;      // String to hold file name to save the matrices in
    QVector<QString> savedFiles;                // Stores file paths of saved matrix files

    MatrixChainSolve *matrixSolve;              // Declare the pointer variable pointing to the MatrixChainSolve object

    int getFileID();                            // Finds the last ID of all saved files + 1
};

#endif // MATRIXLOAD_H
