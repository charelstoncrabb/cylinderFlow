//
//  Matrix.cpp
//  2DNavierStokes
//
//  Created by Nicholas Crabb on 3/25/17.
//  Copyright © 2017 Nicholas Crabb. All rights reserved.
//

#include "Matrix.hpp"


// ================================== PUBLIC MATRIX METHODS: ==========================================
Matrix::Matrix(int numRows, int numCols, vector<double> initVals) : numRows(numRows), numCols(numCols){
    if( ( initVals.size() == 0 ) && ( initVals.size() != numRows*numCols ) ){
        matrixEntries.resize(numRows*numCols);
        for(int i = 0; i < numRows*numCols; i++){
            matrixEntries[i] = 0;
        }
    }else{
        matrixEntries = initVals;
    }
}

// SOLVE LINEAR SYSTEM Ax = b
vector<double> Matrix::solveAxb(vector<double> b){
    vector<double> x;
    Matrix system(numRows,numCols,matrixEntries);
    system.catCol(b);
    system.rref();
    // TODO: Check for inconsistency
    for(int i = 0; i < system.numRows; i++){
        x.push_back(system.matrixEntries[i*system.numCols+numRows]);
    }
    return x;
}//----------------------------------------------------------------------------------------------------

// REDUCED ROW-ECHELON FORM OF MATRIX
void Matrix::rref(void){
    vector<int> pivInd = {0,0};
    vector<vector<int>> pivotIndList;
    int ij = pivInd[0]*numCols+pivInd[1];
    double pivot = matrixEntries[ij];
    bool pivSet = false;
    while( pivInd[0] < numRows && pivInd[1] < numCols ){
        ij = pivInd[0]*numCols+pivInd[1];
        if( matrixEntries[ij] ){
            pivSet = true;
            pivot = matrixEntries[ij];
            pivotIndList.push_back(pivInd);
        }else{
            int k = pivInd[0]+1, kj = k*numCols + pivInd[1];
            while( !matrixEntries[kj] && k < numRows ){
                kj = k*numCols + pivInd[1];
                k++;
            }
            if( k < numRows ){
                this->swapRows(k,pivInd[0]);
                pivSet = true;
            }else{
                pivInd[1]++;
            }
        }
        if( pivSet ){
            for( int l = 0; l < numRows; l++){
                int lj = l*numCols+pivInd[1];
                if( matrixEntries[lj] ){
                    scaleRow(l,1.0/matrixEntries[lj]);
                }
            }
            for( int l = 0; l < numRows; l++){
                int lj = l*numCols+pivInd[1];
                if( l != pivInd[0] && matrixEntries[lj] ){
                    repRowWithDiff(l,pivInd[0]);
                }
            }
            pivInd[0]++;
            pivSet = false;
        }
    }
    for(int k = 0; k < pivotIndList.size(); k++){
        int i = pivotIndList[k][0];
        int ij = pivotIndList[k][0]*numCols+pivotIndList[k][1];
        scaleRow(i,1.0/matrixEntries[ij]);
    }
}//----------------------------------------------------------------------------------------------------

// CONCATENATE GIVEN ROW VECTOR TO BOTTOM OF MATRIX
void Matrix::catRow(std::vector<double> newRow){
    if( newRow.size() != numCols ){
        std::cout << "ERROR: dimension inconsistent in catRow!" << std::endl;
    }else{
        int k = 0;
        matrixEntries.resize((numRows+1)*numCols);
        for( int ij = numRows*numCols; ij < matrixEntries.size(); ij++ ){
            matrixEntries[ij] = newRow[k];
            k++;
        }
        numRows++;
    }
}//----------------------------------------------------------------------------------------------------

// CONCATENATE GIVEN COLUMN VECTOR TO RIGHT OF MATRIX
void Matrix::catCol(std::vector<double> newCol){
    if( newCol.size() != numRows ){
        std::cout << "ERROR: dimension inconsistent in catCol!" << std::endl;
    }else{
        Matrix temp(numRows,numCols,matrixEntries);
        matrixEntries.resize(numRows*(numCols+1));
        for( int i = 0; i < numRows; i++ ){
            for( int j = 0; j < numCols+1; j++ ){
                if( j == numCols ){
                    matrixEntries[i*(numCols+1)+j] = newCol[i];
                }else{
                    matrixEntries[i*(numCols+1)+j] = temp.matrixEntries[i*numCols+j];
                }
            }
        }
        numCols++;
    }
}//----------------------------------------------------------------------------------------------------


// ================================= PRIVATE MATRIX METHODS: ==========================================

// SWAP TWO GIVEN ROWS OF MATRIX
void Matrix::swapRows(int row1, int row2){
    double temp;
    for(int j = 0; j < numCols; j++){
        temp = matrixEntries[row1*numCols+j];
        matrixEntries[row1*numCols+j] = matrixEntries[row2*numCols+j];
        matrixEntries[row2*numCols+j] = temp;
    }
}//----------------------------------------------------------------------------------------------------

// SCALE A GIVEN ROW BY A GIVEN SCALAR
void Matrix::scaleRow(int row, double scalar){
    for(int j = 0; j < numCols; j++){
        matrixEntries[row*numCols+j] = scalar*matrixEntries[row*numCols+j];
    }
}//----------------------------------------------------------------------------------------------------

// REPLACES ROW1 WITH ROW1 - ROW2
void Matrix::repRowWithDiff(int row1, int row2){
    for(int j = 0; j < numCols; j++){
        matrixEntries[row1*numCols+j] = matrixEntries[row1*numCols+j] - matrixEntries[row2*numCols+j];
    }
}//----------------------------------------------------------------------------------------------------


// ================================ OVERLOADED OPERATORS: =============================================

// ASSIGNMENT =
Matrix Matrix::operator=(const Matrix& rhs){
    this->matrixEntries.resize(rhs.numRows*rhs.numCols);
    this->matrixEntries = rhs.matrixEntries;
    return *this;
}//----------------------------------------------------------------------------------------------------

// ADDITION +
Matrix Matrix::operator+(const Matrix& b){
    Matrix sum(this->numRows,this->numCols);
    if( ( b.numRows == this->numRows ) && ( b.numCols == this->numRows ) ){
        for( int i = 0; i < this->matrixEntries.size(); i++ ){
            sum.matrixEntries[i] = b.matrixEntries[i] + this->matrixEntries[i];
        }
    }
    return sum;
}//----------------------------------------------------------------------------------------------------

// SUBTRACTION -
Matrix Matrix::operator-(const Matrix& b){
    Matrix difference(this->numRows,this->numCols);
    if( ( b.numRows == this->numRows ) && ( b.numCols == this->numRows ) ){
        for( int i = 0; i < this->matrixEntries.size(); i++ ){
            difference.matrixEntries[i] = b.matrixEntries[i] - this->matrixEntries[i];
        }
    }
    return difference;
}//----------------------------------------------------------------------------------------------------

// MULTIPLICATION *
Matrix Matrix::operator*(const Matrix& b){
    int ij,ik,kj;
    Matrix prod(this->numRows,b.numCols);
    if( b.numRows == this->numCols ){
        for( int i = 0; i < this->numRows; i++ ){
            for( int j = 0; j < b.numCols; j++ ){
                ij = i*prod.numCols+j;
                for( int k = 0; k < b.numRows; k++ ){
                    ik = i*this->numCols + k;
                    kj = k*b.numCols + j;
                    prod.matrixEntries[ij] = prod.matrixEntries[ij] + this->matrixEntries[ik]*b.matrixEntries[kj];
                }
            }
        }
    }
    return prod;
}//----------------------------------------------------------------------------------------------------

// LOGICAL ==
bool Matrix::operator==(const Matrix& b){
    if( numRows != b.numRows || numCols != b.numCols ){
        return false;
    }else{
        for( int ij = 0; ij < numRows*numCols; ij++ ){
            if( matrixEntries[ij] != b.matrixEntries[ij] ){
                return false;
            }
        }
    }
    return true;
}//----------------------------------------------------------------------------------------------------

// FORMATTED OUTPUT <<
std::ostream & operator<<(std::ostream& os, const Matrix outMat){
    for(int i = 0; i < outMat.numRows; i++){
        for(int j = 0; j < outMat.numCols; j++){
            os << outMat.matrixEntries[i*outMat.numCols+j] << ' ';
        }
        os << endl;
    }
    return os;
}//----------------------------------------------------------------------------------------------------

// FORMATTED VECTOR OUTPUT <<
std::ostream& operator<<(std::ostream& os,std::vector<double> vect){
    for(int i = 0; i < vect.size(); i++){
        std::cout << vect[i] << std::endl;
    }
    return os;
}

// MATRIX INDEXING []
double Matrix::operator[](const int ij){
    return matrixEntries[ij];
}//----------------------------------------------------------------------------------------------------

// RUN VARIOUS ACCEPTANCE TESTS FOR MATRIX CLASS
void Matrix::RunMatrixTests(void) {
    // Arithmetic Tests
    std::cout << "ARITHMETIC TESTS:" << std::endl;
    Matrix testMat1(2,2,{2.1,1,1,2}), testMat2(2,2,{0,1,1,0});
    Matrix testMat3(2,2), testMat4(2,2);
    testMat3 = testMat1 + testMat2;
    testMat4 = testMat1 * testMat2;
    //    std::cout << "Test Matrix #1:" << std::endl << testMat1 << std::endl;
    //    std::cout << "Test Matrix #2:" << std::endl << testMat2 << std::endl;
    std::cout << "Test Matrix #3 ( #1 + #2 ):" << std::endl << testMat3 << std::endl;
    std::cout << "Test Matrix #4 ( #1 * #2 ):" << std::endl << testMat4 << std::endl;
    std::clock_t startcputime = std::clock();
    
    // Gaussian Elimination Test
    std::cout << "GAUSSIAN ELIMINATION TESTS:" << std::endl;
    Matrix testMat5(5,4,{1,1,1,1,1,  1,2,1,1,1,  1,1,1,1,1,  1,1,1,1,1,   1,1,1,1,1});
    std::cout << "Test Matrix #5:" << std::endl << testMat5 << std::endl;
    testMat5.rref();
    std::cout << "Test Matrix #6 ( rref(#5) ):" << std::endl << testMat5 << std::endl;
    double cpu_duration = (std::clock() - startcputime) / (double)CLOCKS_PER_SEC;
    std::cout << "Finished in " << cpu_duration << " seconds [CPU Clock] " << std::endl; // About 40X faster than MATLAB
    
    // Concatenation Tests
    std::cout << "CONCATENATION TESTS:" << std::endl;
    Matrix testMat6(2,3,{1,2,3,4,5,6});
    Matrix testMat7(2,3,{1,2,3,4,5,6});
    std::vector<double> newRow = {7,7,7};
    std::vector<double> newCol = {7,7};
    testMat6.catRow(newRow);
    std::cout << testMat6 << std::endl;
    testMat7.catCol(newCol);
    std::cout << testMat7 << std::endl;
    
    // System Solver Tests
    std::cout << "SOLVING Ax=b TESTS:" << std::endl;
    int n = 10;
    Matrix testMat8(n,n);
    std::vector<double> b;
    for(int i = 0; i < n; i++){
        b.push_back((double)i+1);
        for(int j = 0; j < n; j++){
            if( i == j ){
                testMat8.matrixEntries[i*n+j] = 2.0;
            }
            if( std::abs(i-j) == 1 ){
                testMat8.matrixEntries[i*n+j] = 1.0;
            }
        }
    }
    std::cout << "A = " << std::endl << testMat8 << std::endl;
    std::cout << "b = " << std::endl << b << std::endl;
    startcputime = std::clock();
    std::cout << "x = " << std::endl << testMat8.solveAxb(b) << std::endl;
    cpu_duration = (std::clock() - startcputime) / (double)CLOCKS_PER_SEC;
    std::cout << "Finished in " << cpu_duration << " seconds [CPU Clock] " << std::endl; // About 60X faster than MATLAB
}

