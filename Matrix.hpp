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
#include "Math.hpp"
#include "VectOps.hpp"

class Matrix{
public:
    Matrix();
    Matrix(int numRows, int numCols = -1, std::vector<double> initVals = {});

    // Public Methods
    void RunMatrixTests(void);
    std::vector<double> solveAxb(std::vector<double> b);
    void rref(void);
    std::vector<double> getRow(int rowNum);
    std::vector<double> getCol(int colNum);
    std::vector<double> getDiag(void);
    void transpose(void);
    std::vector<double> charPoly(void);
    double det(void);
    std::vector<double> eigQR(double tolerance, bool verbose = false);
    std::vector<double> eigCP(double tolerance, bool verbose = false);
    std::vector<double> eigAI(double tolerance, bool verbose = false);
    std::vector<Matrix> QRdecomp(void);
    std::vector<double> svd(double tolerance, bool verbose = false);
    
    Matrix operator=(const Matrix& rhs);
    Matrix operator+(const Matrix& b);
    Matrix operator-(const Matrix& b);
    Matrix operator*(const Matrix& b);
    bool operator==(const Matrix& b);
    double operator[](const int ij);
    
    int numRows;
    int numCols;
    std::vector<double> entries;

    friend std::ostream& operator<<(std::ostream& os, const Matrix A);

    Matrix principalSubmatrix(std::vector<int> exInds);
    void swapRows(int row1, int row2);
    void scaleRow(int row, double scalar);
    void repRowWithDiff(int row1, int row2);
    void catRow(std::vector<double> newRow);
    void catCol(std::vector<double> newCol);
    void rmRow(int rowNum);
    void rmCol(int colNum);
    bool isEmpty(void);
};

#endif
