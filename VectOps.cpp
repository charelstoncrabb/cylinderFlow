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
        for(size_t i = 0; i < lhs.size(); i++){
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
        for(size_t i = 0; i < lhs.size(); i++){
            difference[i] = lhs[i] - rhs[i];
        }
        return difference;
    }
}//----------------------------------------------------------------------------------------------------

// DOT PRODUCT OF TWO VECTORS  ------------------------------------------------------------------------
double dot(std::vector<double> u, std::vector<double> v){
    double dotProd = 0;
    if( u.size() == v.size() ){
        for(size_t i = 0; i < u.size(); i++){
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
    for(size_t i = 0; i < u.size(); i++ ){
        norm = norm + pow(fabs(u[i]),p);
    }
    return pow(norm,1.0/p);
}//----------------------------------------------------------------------------------------------------

// SCALAR MULTIPLICATION  ----------------------------------------------------------------------------
std::vector<double> scale(std::vector<double> u, double c){
    std::vector<double> scaledU(u.size());
    for(size_t i = 0; i < u.size(); i++){
        scaledU[i] = c*u[i];
    }
    return scaledU;
}//----------------------------------------------------------------------------------------------------

// POINTWISE POWER  -----------------------------------------------------------------------------------
std::vector<double> vectPow(std::vector<double> u, double p){
    for(size_t i = 0; i < u.size(); i++){
        u[i] = pow(u[i],p);
    }
    return u;
}//----------------------------------------------------------------------------------------------------

// CONTINUOUS RANGE OF VECTOR  ------------------------------------------------------------------------
// RETURNS SUBVECTOR u[min:max]
template<class T>
std::vector<T*> range(std::vector<T*> u, size_t min, size_t max){
    std::vector<T*> subU(max-min+1);
    for(size_t i = 0; i < max-min+1; i++){
        subU[i] = u[i+min];
    }
    return subU;
}//----------------------------------------------------------------------------------------------------

// SUBINDEXING OF VECTOR  -----------------------------------------------------------------------------
// RETURNS SUBVECTOR u[inds]
template<class T>
std::vector<T*> subind(std::vector<T*> u, std::vector<int> inds){
    size_t N = inds.size();
    std::vector<T*> subU(N);
    // TODO: Check that inds is not outside range of u
    for(size_t i = 0; i < N; i++){
        subU[i] = u[inds[i]];
    }
    return subU;
}//----------------------------------------------------------------------------------------------------

// CONCATENATES GIVEN VECTOR IN ORDER {u,v}  ----------------------------------------------------------
template<class T>
std::vector<T*> cat(std::vector<T*> u, std::vector<T*> v){
    std::vector<T*> catted(u.size()+v.size());
    for(size_t i = 0; i < u.size(); i++)
        catted[i] = u[i];
    for(size_t i = 0; i < v.size(); i++)
        catted[i+u.size()] = v[i];
    return catted;
}//----------------------------------------------------------------------------------------------------

// REMOVES VECTOR ELEMENT AT INDEX ind2rm  ------------------------------------------------------------
template<class T>
std::vector<T*> rmEl(std::vector<T*> u, size_t ind2rm){
    std::vector<T*> newVec(u.size()-1);
	size_t i;
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
	size_t N = u.size();
    T* temp;
    for(size_t i = N; i > 0; i--){
        for(size_t j = 0; j < i-1; j++){
            if( *u[j] < *u[j+1] ){
                temp = u[j+1];
                u[j+1] = u[j];
                u[j] = temp;
            }
        }
    }
}//----------------------------------------------------------------------------------------------------

// FORWARD DECLARATIONS FOR TEMPLATE INSTANTIATION
template std::vector<Node*> range(std::vector<Node*>, size_t, size_t);
template std::vector<Node*> cat(std::vector<Node*>,std::vector<Node*>);
template std::vector<Facet*> cat(std::vector<Facet*>,std::vector<Facet*>);
template std::vector<Node*> rmEl(std::vector<Node*>, size_t);
template void revSort(std::vector<Node*>);

