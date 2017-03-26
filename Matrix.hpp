//
//  Matrix.h
//  2DNavierStokes
//
//  Created by Nicholas Crabb on 3/23/17.
//  Copyright © 2017 Nicholas Crabb. All rights reserved.
//
#ifndef _MATRIX_H_
#define _MATRIX_H_
#include "Includes.hpp"

class Matrix{
public:
    Matrix();
    Matrix(int numRows, int numCols, vector<double> initVals = {});

    // Public Methods
    void RunMatrixTests(void);
    vector<double> solveAxb(vector<double> b);
    void rref(void);
    vector<double> getRow(int rowNum);
    vector<double> getCol(int colNum);
    void transpose(void);
    vector<double> eig(void);
    vector<Matrix> QRdecomp(void);
    vector<double> svd(void);
    
    Matrix operator=(const Matrix& rhs);
    Matrix operator+(const Matrix& b);
    Matrix operator-(const Matrix& b);
    Matrix operator*(const Matrix& b);
    bool operator==(const Matrix& b);
    double operator[](const int ij);
    
    int numRows;
    int numCols;
    vector<double> matrixEntries;

    friend std::ostream& operator<<(std::ostream& os, const Matrix A);
private:
    // Private methods used within public methods
    void swapRows(int row1, int row2);
    void scaleRow(int row, double scalar);
    void repRowWithDiff(int row1, int row2);
    void catRow(std::vector<double> newRow);
    void catCol(std::vector<double> newCol);
    bool isEmpty(void);
};

#endif
