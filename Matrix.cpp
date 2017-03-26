//
//  Matrix.cpp
//  2DNavierStokes
//
//  Created by Nicholas Crabb on 3/25/17.
//  Copyright © 2017 Nicholas Crabb. All rights reserved.
//

#include "Matrix.hpp"

// ================================ USEFUL MATRIX TEMPLATES ===========================================

// BUILDS AN ALL-ONES MATRIX
Matrix ones(int numRows, int numCols = -1){
	if( numCols == -1 ){
		numCols = numRows;
	}
	std::vector<double> onesEntries(numRows*numCols,1);
	Matrix onesMatrix(numRows,numCols,onesEntries);
	return onesMatrix;
}

// BUILD TRI-DIAGONAL MATRIX WITH GIVEN VALUES
Matrix triDiag(int n, double diag, double offDiag){
	Matrix tridiag(n,n);
	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			if( i == j ){
				tridiag.entries[i*n+j] = diag;
			}
			if( std::abs(i-j) == 1 ){
				tridiag.entries[i*n+j] = offDiag;
			}
		}
	}
	return tridiag;
}

// ============================= MISCELLANEOUS VECTOR OPERATIONS ======================================

// VECTOR ADDITION  -----------------------------------------------------------------------------------
std::vector<double> operator+(std::vector<double> lhs, std::vector<double> rhs){
    if( lhs.size() != rhs.size() ){
        std::cout << "ERROR IN operator-: dimension mismatch!" << std::endl;
        return {};
    }else{
        std::vector<double> sum(lhs.size());
        for(int i = 0; i < lhs.size(); i++){
            sum[i] = lhs[i] + rhs[i];
        }
        return sum;
    }
}//----------------------------------------------------------------------------------------------------

// VECTOR SUBTRACTION  --------------------------------------------------------------------------------
std::vector<double> operator-(std::vector<double> lhs, std::vector<double> rhs){
    if( lhs.size() != rhs.size() ){
        std::cout << "ERROR IN operator-: dimension mismatch!" << std::endl;
        return {};
    }else{
        std::vector<double> difference(lhs.size());
        for(int i = 0; i < lhs.size(); i++){
            difference[i] = lhs[i] - rhs[i];
        }
        return difference;
    }
}//----------------------------------------------------------------------------------------------------

// DOT PRODUCT OF TWO VECTORS  ------------------------------------------------------------------------
double dot(std::vector<double> u, std::vector<double> v){
    double dotProd = 0;
    if( u.size() == v.size() ){
        for(int i = 0; i < u.size(); i++){
            dotProd = dotProd + u[i]*v[i];
        }
    }else{
        std::cout << "ERROR IN dot(): dimension mismatch!" << std::endl;
    }
    return dotProd;
}//----------------------------------------------------------------------------------------------------

// NORM OF VECTOR  ------------------------------------------------------------------------------------
double norm(std::vector<double> u, double p = 2){
    double norm = 0;
    for( int i = 0; i < u.size(); i++ ){
        norm = norm + pow(fabs(u[i]),p);
    }
    return pow(norm,1.0/p);
}//----------------------------------------------------------------------------------------------------

// SCALAR MULTIPLICATION  ----------------------------------------------------------------------------
std::vector<double> scale(std::vector<double> u, double c){
    std::vector<double> scaledU(u.size());
    for(int i = 0; i < u.size(); i++){
        scaledU[i] = c*u[i];
    }
    return scaledU;
}//----------------------------------------------------------------------------------------------------

// ====================================================================================================
// VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV  MATRIX CLASS  VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
// ====================================================================================================

// ================================== PUBLIC MATRIX METHODS: ==========================================
// DEFAULT CONSTRUCTOR  -------------------------------------------------------------------------------
Matrix::Matrix(){
    numRows = 0;
    numCols = 0;
}//----------------------------------------------------------------------------------------------------

// INITILIZED CONSTRUCTOR  ----------------------------------------------------------------------------
Matrix::Matrix(int numRows, int numCols, std::vector<double> initVals) : numRows(numRows), numCols(numCols){
    if( ( initVals.size() == 0 ) && ( initVals.size() != numRows*numCols ) ){
        entries.resize(numRows*numCols);
        for(int i = 0; i < numRows*numCols; i++){
            entries[i] = 0;
        }
    }else{
        entries = initVals;
    }
}//----------------------------------------------------------------------------------------------------

// SOLVE LINEAR SYSTEM Ax = b  ------------------------------------------------------------------------
std::vector<double> Matrix::solveAxb(std::vector<double> b){
    std::vector<double> x(numRows);
    Matrix system(numRows,numCols,entries);
    std::cout << system << std::endl;
    system.catCol(b);
    std::cout << system << std::endl;
    system.rref();
    std::cout << system << std::endl;
    // TODO: Check for inconsistency
    for(int i = 0; i < system.numRows; i++){
        x[i] = system.entries[i*system.numCols+numRows];
    }
    return x;
}//----------------------------------------------------------------------------------------------------

// REDUCED ROW-ECHELON FORM OF MATRIX  ----------------------------------------------------------------
void Matrix::rref(void){
    std::vector<int> pivInd = {0,0};
    std::vector<std::vector<int>> pivotIndList;
    int ij = pivInd[0]*numCols+pivInd[1];
    double pivot = entries[ij];
    bool pivSet = false;
    while( pivInd[0] < numRows && pivInd[1] < numCols ){
        ij = pivInd[0]*numCols+pivInd[1];
        if( entries[ij] ){
            pivSet = true;
            pivot = entries[ij];
            pivotIndList.push_back(pivInd);
        }else{
            int k = pivInd[0]+1, kj = k*numCols + pivInd[1];
            while( !entries[kj] && k < numRows ){
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
                if( entries[lj] ){
                    scaleRow(l,1.0/entries[lj]);
                }
            }
            for( int l = 0; l < numRows; l++){
                int lj = l*numCols+pivInd[1];
                if( l != pivInd[0] && entries[lj] ){
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
        scaleRow(i,1.0/entries[ij]);
    }
}//----------------------------------------------------------------------------------------------------

// RETURNS THE ROW CORRESPONDING GIVEN ROW NUMBER  ----------------------------------------------------
std::vector<double> Matrix::getRow(int rowNum){
    std::vector<double> row(numCols);
    for(int i = 0; i < numCols; i++){
        row[i] = entries[rowNum*numCols+i];
    }
    return row;
}//----------------------------------------------------------------------------------------------------

// RETURNS THE COLUMN CORRESPONDING TO GIVEN COLUMN NUMBER  ------------------------------------------
std::vector<double> Matrix::getCol(int colNum){
    std::vector<double> col(numRows);
    for(int i = 0; i < numRows; i++){
        col[i] = entries[i*numCols+colNum];
    }
    return col;
}//----------------------------------------------------------------------------------------------------

// RETURNS THE DIAGONAL OF MATRIX  --------------------------------------------------------------------
std::vector<double> Matrix::getDiag(void){
	int N = fmin(numRows,numCols);
	std::vector<double> diag(N);
	for(int i = 0; i < N; i++){
		diag[i] = entries[i*numCols+i];
	}
	return diag;
}
//----------------------------------------------------------------------------------------------------

// EIGENVALUE SOLVER VIA QR ALGORITHM -- MATRIX MUST BE SYMMETRIC  -----------------------------------
std::vector<double> Matrix::eigQR(double tolerance, bool verbose){
	if( numRows == numCols ){
		std::fstream outfile("QR_ITERATIONS.out",std::fstream::out | std::fstream::trunc);
		int itrCount = 1;
		std::vector<double> currEigs(numCols), prevEigs(numCols);
		Matrix Ak = *this;
		std::vector<Matrix> QR = Ak.QRdecomp();// QR[0].transpose();
		Ak = QR[1]*QR[0];
		prevEigs = Ak.getDiag();
		double currTol = 999999;
		while( currTol >= tolerance ){
			if(verbose){
				outfile << "At iteration: " << itrCount << std::endl <<
				"Q*R = " << std::endl << QR[0]*QR[1] << "Current error = "
				<< currTol << std::endl << std::endl;
			}
			QR = Ak.QRdecomp();// QR[0].transpose();
			Ak = QR[1]*QR[0];
			currEigs = Ak.getDiag();
			currTol = norm(currEigs-prevEigs,1);
			prevEigs = currEigs;
			itrCount++;
		}
		if(verbose){
			std::cout << "eigReal() convergence to " << currTol << " in " << itrCount << " iterations." << std::endl;
		}
		outfile.close();
		return currEigs;
	}else{
		std::vector<double> err;
		std::cout << "ERROR IN eig(): matrix must be square!" << std::endl;
		return err;
	}
}//----------------------------------------------------------------------------------------------------

// EIGENVALUE SOLVER USING ARNOLDI ITERATION  ---------------------------------------------------------
std::vector<double> Matrix::eigAI(double tolerance, bool verbose){
	std::vector<double> currEigs;
	// TODO: Implement Arnoldi Iteration for general matrices
	return currEigs;
}//----------------------------------------------------------------------------------------------------

// QR DECOMPOSITION USING GRAM-SCHIDT PROCESS  --------------------------------------------------------
std::vector<Matrix> Matrix::QRdecomp(void){
    std::vector<Matrix> QR(2);
    Matrix Q(0,0), R(numCols,numCols), U(0,0);
    // PSEUDO:
    // Set e1 = a1/norm(a1)
    // For each column j in this:
    //   uj = aj - sum_k<j dot(ak,ek)ek
    //   ej = uj/norm(uj)
    //   colj of Q = ej, colj of R = coeffs dot(aj,ek)
    std::vector<double> aj = this->getCol(0);
    std::vector<double> uj = aj, uk;
	U.catCol(uj);
    std::vector<double> ek = scale(uj,1.0/norm(uj));
    Q.catCol(ek);
	R.entries[0] = norm(uj);
    for(int j = 1; j < numCols; j++){
        aj = this->getCol(j);
        uj = aj;
        for(int k = 0; k < j; k++){
            uk = U.getCol(k);
			R.entries[k*numCols+j] = dot(aj,Q.getCol(k));
            uj = uj - scale(uk,dot(uk,aj)/dot(uk,uk));
        }
		U.catCol(uj);
		Q.catCol(scale(uj,1.0/norm(uj)));
		R.entries[j*numCols+j] = dot(aj,Q.getCol(j));
    }
    QR[0] = Q;
    QR[1] = R;
    return QR;
}

// SINGULAR VALUE DECOMPOSITION
std::vector<double> Matrix::svd(void){
    std::vector<double> singularValues;
    // TODO: Finish SVD logic
    return singularValues;
}//----------------------------------------------------------------------------------------------------

// TRANSPOSES MATRIX  ---------------------------------------------------------------------------------
void Matrix::transpose(void){
    Matrix tpTemp(numCols,numRows);
    for(int i = 0; i < numRows; i++){
        for(int j = 0; j < numCols; j++){
            tpTemp.entries[j*numRows+i] = entries[i*numCols+j];
        }
    }
    numRows = tpTemp.numRows;
    numCols = tpTemp.numCols;
    entries = tpTemp.entries;
}//----------------------------------------------------------------------------------------------------

// CONCATENATE GIVEN ROW VECTOR TO BOTTOM OF MATRIX  --------------------------------------------------
void Matrix::catRow(std::vector<double> newRow){
    if( this->isEmpty() ){
        Matrix newMat(1,(int)newRow.size(),newRow);
        *this = newMat;
    }else{
        if( newRow.size() != numCols ){
            std::cout << "ERROR in catRow(): dimension mismatch!" << std::endl;
        }else{
            int k = 0;
            entries.resize((numRows+1)*numCols);
            for( int ij = numRows*numCols; ij < entries.size(); ij++ ){
                entries[ij] = newRow[k];
                k++;
            }
            numRows++;
        }
    }
}//----------------------------------------------------------------------------------------------------

// CONCATENATE GIVEN COLUMN VECTOR TO RIGHT OF MATRIX  ------------------------------------------------
void Matrix::catCol(std::vector<double> newCol){
    if( this->isEmpty() ){
        Matrix newMat((int)newCol.size(),1,newCol);
        *this = newMat;
    }else{
        if( newCol.size() != numRows ){
            std::cout << "ERROR in newCol(): dimension mismatch" << std::endl;
        }else{
            Matrix temp(numRows,numCols,entries);
            entries.resize(numRows*(numCols+1));
            for( int i = 0; i < numRows; i++ ){
                for( int j = 0; j < numCols+1; j++ ){
                    if( j == numCols ){
                        entries[i*(numCols+1)+j] = newCol[i];
                    }else{
                        entries[i*(numCols+1)+j] = temp.entries[i*numCols+j];
                    }
                }
            }
            numCols++;
        }
    }
}//----------------------------------------------------------------------------------------------------


// ================================= PRIVATE MATRIX METHODS: ==========================================

// SWAP TWO GIVEN ROWS OF MATRIX  ---------------------------------------------------------------------
void Matrix::swapRows(int row1, int row2){
    double temp;
    for(int j = 0; j < numCols; j++){
        temp = entries[row1*numCols+j];
        entries[row1*numCols+j] = entries[row2*numCols+j];
        entries[row2*numCols+j] = temp;
    }
}//----------------------------------------------------------------------------------------------------

// SCALE A GIVEN ROW BY A GIVEN SCALAR  ---------------------------------------------------------------
void Matrix::scaleRow(int row, double scalar){
    for(int j = 0; j < numCols; j++){
        entries[row*numCols+j] = scalar*entries[row*numCols+j];
    }
}//----------------------------------------------------------------------------------------------------

// REPLACES ROW1 WITH ROW1 - ROW2  --------------------------------------------------------------------
void Matrix::repRowWithDiff(int row1, int row2){
    for(int j = 0; j < numCols; j++){
        entries[row1*numCols+j] = entries[row1*numCols+j] - entries[row2*numCols+j];
    }
}//----------------------------------------------------------------------------------------------------

// LOGICAL IS MATRIX EMPTY  ---------------------------------------------------------------------------
bool Matrix::isEmpty(void){
    bool emptyFlag = false;
    if( !numRows || !numCols ){
        emptyFlag = true;
    }
    return emptyFlag;
}//----------------------------------------------------------------------------------------------------

// ================================ OVERLOADED OPERATORS: =============================================

// ASSIGNMENT =  --------------------------------------------------------------------------------------
Matrix Matrix::operator=(const Matrix& rhs){
    entries.resize(rhs.numRows*rhs.numCols);
    entries = rhs.entries;
	numRows = rhs.numRows;
	numCols = rhs.numCols;
    return *this;
}//----------------------------------------------------------------------------------------------------

// ADDITION +  ----------------------------------------------------------------------------------------
Matrix Matrix::operator+(const Matrix& b){
    Matrix sum(this->numRows,this->numCols);
    if( ( b.numRows == this->numRows ) && ( b.numCols == this->numRows ) ){
        for( int i = 0; i < this->entries.size(); i++ ){
            sum.entries[i] = b.entries[i] + this->entries[i];
        }
    }
    return sum;
}//----------------------------------------------------------------------------------------------------

// SUBTRACTION -  -------------------------------------------------------------------------------------
Matrix Matrix::operator-(const Matrix& b){
    Matrix difference(this->numRows,this->numCols);
    if( ( b.numRows == this->numRows ) && ( b.numCols == this->numRows ) ){
        for( int i = 0; i < this->entries.size(); i++ ){
            difference.entries[i] = b.entries[i] - this->entries[i];
        }
    }
    return difference;
}//----------------------------------------------------------------------------------------------------

// MULTIPLICATION *  ----------------------------------------------------------------------------------
Matrix Matrix::operator*(const Matrix& b){
    if( numCols == b.numRows ){
        int ij,ik,kj;
        Matrix prod(this->numRows,b.numCols);
        if( b.numRows == this->numCols ){
            for( int i = 0; i < this->numRows; i++ ){
                for( int j = 0; j < b.numCols; j++ ){
                    ij = i*prod.numCols+j;
                    for( int k = 0; k < b.numRows; k++ ){
                        ik = i*this->numCols + k;
                        kj = k*b.numCols + j;
                        prod.entries[ij] = prod.entries[ij] + this->entries[ik]*b.entries[kj];
                    }
                }
            }
        }
        return prod;
    }else{
        Matrix err(0,0);
        std::cout << "ERROR in *: dimension mismatch!" << std::endl;
		return err;
    }
}//----------------------------------------------------------------------------------------------------

// LOGICAL ==  ----------------------------------------------------------------------------------------
bool Matrix::operator==(const Matrix& b){
    if( numRows != b.numRows || numCols != b.numCols ){
        return false;
    }else{
        for( int ij = 0; ij < numRows*numCols; ij++ ){
            if( entries[ij] != b.entries[ij] ){
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
            os << A.entries[i*A.numCols+j] << ' ';
        }
        os << std::endl;
    }
    return os;
}//----------------------------------------------------------------------------------------------------

// FORMATTED VECTOR OUTPUT <<  ------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os,std::vector<double> vect){
    for(int i = 0; i < vect.size(); i++){
        std::cout << vect[i] << std::endl;
    }
    return os;
}

// MATRIX INDEXING []
double Matrix::operator[](const int ij){
    return entries[ij];
}//----------------------------------------------------------------------------------------------------



// RUN VARIOUS ACCEPTANCE TESTS FOR MATRIX CLASS  -----------------------------------------------------
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
    Matrix testMat8 = triDiag(n,2.0,1.0), testMat9 = triDiag(n,2.0,1.0);
    std::vector<double> b;
    for(int i = 0; i < n; i++){
        b.push_back((double)i+1);
    }
    std::cout << "A = " << std::endl << testMat9 << std::endl;
    std::cout << "b = " << std::endl << b << std::endl;
    startcputime = std::clock();
    std::cout << "x = " << std::endl << testMat9.solveAxb(b) << std::endl;
    cpu_duration = (std::clock() - startcputime) / (double)CLOCKS_PER_SEC;
    std::cout << "Finished in " << cpu_duration << " seconds [CPU Clock] " << std::endl; // About 60X faster than MATLAB
	
	startcputime = std::clock();
	std::vector<Matrix> QR = testMat9.QRdecomp();
	std::cout << "QR DECOMPOSITION TESTS:" << std::endl;
	std::cout << "Finished in " << cpu_duration << " seconds [CPU Clock] " << std::endl;
	std::cout << "A = " << std::endl << testMat9 << std::endl;
	std::cout << "Q = " << std::endl << QR[0] << std::endl;
	std::cout << "R = " << std::endl << QR[1] << std::endl;
	std::cout << "Q*R = " << std::endl << QR[0]*QR[1] << std::endl;

    Matrix testMat10(5,4,{2,1,0,0,  1,2,1,0,  0,1,2,1,  0,0,1,2,  0,0,0,1});
	std::cout << "Computing QR of A = " << std::endl << testMat10 << std::endl;
	startcputime = std::clock();
    std::vector<Matrix> QR2 = testMat10.QRdecomp();
    std::cout << "Finished in " << cpu_duration << " seconds [CPU Clock] " << std::endl;
    std::cout << "A = " << std::endl << testMat10 << std::endl;
    std::cout << "Q = " << std::endl << QR2[0] << std::endl;
    std::cout << "R = " << std::endl << QR2[1] << std::endl;
    std::cout << "Q*R = " << std::endl << QR2[0]*QR2[1] << std::endl;
	
	double tol = 0.001;
	std::cout << "EIGENVALUE TESTS:" << std::endl;
	std::cout << "Computing eigenvalues of " << std::endl << "A = " << std::endl << testMat9 << std::endl;
	startcputime = std::clock();
	std::vector<double> eigs = testMat9.eigQR(tol);
	std::cout << "eigs = " << std::endl << eigs << std::endl;
	std::cout << "Finished in " << cpu_duration << " seconds [CPU Clock] " << std::endl;
}//----------------------------------------------------------------------------------------------------











