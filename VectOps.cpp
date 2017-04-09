#include "VectOps.hpp"
#include "Mesh.hpp"
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
double norm(std::vector<double> u, double p){
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

// POINTWISE POWER  -----------------------------------------------------------------------------------
std::vector<double> vectPow(std::vector<double> u, double p){
    for(int i = 0; i < u.size(); i++){
        u[i] = pow(u[i],p);
    }
    return u;
}//----------------------------------------------------------------------------------------------------

// CONTINUOUS RANGE OF VECTOR  ------------------------------------------------------------------------
// RETURNS SUBVECTOR u[min:max]
template<class T>
std::vector<T*> range(std::vector<T*> u, int min, int max){
    std::vector<T*> subU(max-min+1);
    for(int i = 0; i < max-min+1; i++){
        subU[i] = u[i+min];
    }
    return subU;
}//----------------------------------------------------------------------------------------------------

// SUBINDEXING OF VECTOR  -----------------------------------------------------------------------------
// RETURNS SUBVECTOR u[inds]
template<class T>
std::vector<T*> subind(std::vector<T*> u, std::vector<int> inds){
    int N = (int)inds.size();
    std::vector<T*> subU(N);
    // TODO: Check that inds is not outside range of u
    for(int i = 0; i < N; i++){
        subU[i] = u[inds[i]];
    }
    return subU;
}//----------------------------------------------------------------------------------------------------

// CONCATENATES GIVEN VECTOR IN ORDER {u,v}  ----------------------------------------------------------
template<class T>
std::vector<T*> cat(std::vector<T*> u, std::vector<T*> v){
    std::vector<T*> catted(u.size()+v.size());
    for(int i = 0; i < u.size(); i++)
        catted[i] = u[i];
    for(int i = 0; i < v.size(); i++)
        catted[i+u.size()] = v[i];
    return catted;
}//----------------------------------------------------------------------------------------------------

// REMOVES VECTOR ELEMENT AT INDEX ind2rm  ------------------------------------------------------------
template<class T>
std::vector<T*> rmEl(std::vector<T*> u, int ind2rm){
    std::vector<T*> newVec(u.size()-1);
    int i;
    for(i = 0; i < ind2rm; i++)
        newVec[i] = u[i];
    i++;
    for(; i < u.size(); i++)
        newVec[i-1] = u[i];
    return newVec;
}//----------------------------------------------------------------------------------------------------

// SORTS VECTOR IN REVERSE ORDER  ---------------------------------------------------------------------
template<class T>
void revSort(std::vector<T*> u){
    int N = (int)u.size();
    T* temp;
    for(int i = N; i > 0; i--){
        for(int j = 0; j < i-1; j++){
            if( *u[j] < *u[j+1] ){
                temp = u[j+1];
                u[j+1] = u[j];
                u[j] = temp;
            }
        }
    }
}//----------------------------------------------------------------------------------------------------

// FORWARD DECLARATIONS FOR TEMPLATE INSTANTIATION
template std::vector<Node*> range(std::vector<Node*>, int, int);
template std::vector<Node*> cat(std::vector<Node*>,std::vector<Node*>);
template std::vector<Facet*> cat(std::vector<Facet*>,std::vector<Facet*>);
template std::vector<Node*> rmEl(std::vector<Node*>, int);
template void revSort(std::vector<Node*>);

