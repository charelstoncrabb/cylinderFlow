//
//  Matrix.cpp
//  2DNavierStokes
//
//  Created by Nicholas Crabb on 3/25/17.
//  Copyright © 2017 Nicholas Crabb. All rights reserved.
//

#include "Matrix.hpp"

// ============================= MISCELLANEOUS VECTOR OPERATIONS ======================================
// DOT PRODUCT OF TWO VECTORS
double dot(std::vector<double> u, std::vector<double> v){
    double dotProd = 0;
    if( u.size() == v.size() ){
        for(int i = 0; i < u.size(); i++){
            dotProd = dotProd + u[i]*v[i];
        }
    }else{
        std::cout << "ERROR IN dot(): vectors different lengths!" << std::endl;
    }
    return dotProd;
}//----------------------------------------------------------------------------------------------------

// NORM OF VECTOR
double norm(std::vector<double> u, double p = 2){
    double norm = 0;
    for( int i = 0; i < u.size(); i++ ){
        norm = norm + pow(u[i],p);
    }
    return pow(norm,1.0/p);
}//----------------------------------------------------------------------------------------------------

// SCALAR MULTIPLICATION
std::vector<double> scale(std::vector<double> u, double c){
    std::vector<double> scaledU(u.size());
    for(int i = 0; i < u.size(); i++){
        scaledU[i] = c*u[i];
    }
    return scaledU;
}//----------------------------------------------------------------------------------------------------

// VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV  MATRIX CLASS  VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
// ====================================================================================================
// ================================== PUBLIC MATRIX METHODS: ==========================================
// DEFAULT CONSTRUCTOR
Matrix::Matrix(){
    numRows = 0;
    numCols = 0;
}//----------------------------------------------------------------------------------------------------

// INITILIZED CONSTRUCTOR
Matrix::Matrix(int numRows, int numCols, vector<double> initVals) : numRows(numRows), numCols(numCols){
    if( ( initVals.size() == 0 ) && ( initVals.size() != numRows*numCols ) ){
        matrixEntries.resize(numRows*numCols);
        for(int i = 0; i < numRows*numCols; i++){
            matrixEntries[i] = 0;
        }
    }else{
        matrixEntries = initVals;
    }
}//----------------------------------------------------------------------------------------------------

// SOLVE LINEAR SYSTEM Ax = b
vector<double> Matrix::solveAxb(vector<double> b){
    vector<double> x(numRows);
    Matrix system(numRows,numCols,matrixEntries);
    std::cout << system << std::endl;
    system.catCol(b);
    std::cout << system << std::endl;
    system.rref();
    std::cout << system << std::endl;
    // TODO: Check for inconsistency
    for(int i = 0; i < system.numRows; i++){
        x[i] = system.matrixEntries[i*system.numCols+numRows];
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

// RETURNS THE ROW CORRESPONDING GIVEN ROW NUMBER
vector<double> Matrix::getRow(int rowNum){
    vector<double> row(numCols,0);
    for(int i = 0; i < numCols; i++){
        row[i] = matrixEntries[i*numCols+rowNum];
    }
    return row;
}//----------------------------------------------------------------------------------------------------

// RETURNS THE COLUMN CORRESPONDING TO GIVEN COLUMN NUMBER
vector<double> Matrix::getCol(int colNum){
    vector<double> col;
    for(int i = 0; i < numRows; i++){
        col[i] = matrixEntries[colNum*numCols+i];
    }
    return col;
}//----------------------------------------------------------------------------------------------------

// EIGENVALUE SOLVER VIA QR ALGORITHM -- MATRIX MUST BE SQUARE
vector<double> Matrix::eig(void){
    vector<double> eigenvalues;
    return eigenvalues;
}//----------------------------------------------------------------------------------------------------

// QR DECOMPOSITION USING GRAM-SCHIDT PROCESS
vector<Matrix> Matrix::QRdecomp(void){
    vector<Matrix> QR(2);
    Matrix Q(0,0), R(0,0);
    // PSEUDO:
    // Set e1 = a1/norm(a1)
    // For each column j in this:
    //   uj = aj - sum_k<j dot(ak,ek)ek
    //   ej = uj/norm(uj)
    //   colj of Q = ej, rowj of R = coeffs dot(ak,ek)
    vector<double> uj = this->getCol(1);
    double ujNorm = norm(uj);
    vector<double> ej = scale(uj,1.0/ujNorm);
    QR[0] = Q;
    R.transpose();
    QR[1] = R;
    return QR;
}

// SINGULAR VALUE DECOMPOSITION
vector<double> Matrix::svd(void){
    vector<double> singularValues;
    return singularValues;
}//----------------------------------------------------------------------------------------------------

// TRANSPOSES MATRIX
void Matrix::transpose(void){
    Matrix tpTemp(numCols,numRows);
    for(int i = 0; i < numRows; i++){
        for(int j = 0; j < numCols; j++){
            tpTemp.matrixEntries[j*numRows+i] = matrixEntries[i*numCols+j];
        }
    }
    numRows = tpTemp.numRows;
    numCols = tpTemp.numCols;
    matrixEntries = tpTemp.matrixEntries;
}//----------------------------------------------------------------------------------------------------

// CONCATENATE GIVEN ROW VECTOR TO BOTTOM OF MATRIX
void Matrix::catRow(std::vector<double> newRow){
    if( this->isEmpty() ){
        Matrix newMat(1,(int)newRow.size(),newRow);
        *this = newMat;
    }else{
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
    }
}//----------------------------------------------------------------------------------------------------

// CONCATENATE GIVEN COLUMN VECTOR TO RIGHT OF MATRIX
void Matrix::catCol(std::vector<double> newCol){
    if( this->isEmpty() ){
        Matrix newMat((int)newCol.size(),1,newCol);
        *this = newMat;
    }else{
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

bool Matrix::isEmpty(void){
    bool emptyFlag = false;
    if( !numRows || !numCols ){
        emptyFlag = true;
    }
    return emptyFlag;
}

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

// ================================= MISCELLANEOUS FUNCTIONS ==========================================

// FORMATTED OUTPUT <<
std::ostream & operator<<(std::ostream& os, const Matrix A){
    for(int i = 0; i < A.numRows; i++){
        for(int j = 0; j < A.numCols; j++){
            os << A.matrixEntries[i*A.numCols+j] << ' ';
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
    int n = 3;
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

