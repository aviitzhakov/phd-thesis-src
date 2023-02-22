#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <cstdlib>

template <class T> 
class Matrix {

public:
    Matrix ( int n, int m );

    ~Matrix();

    const T& get ( int i, int j ) const;

    void set ( int i, int j, const T& element );

    size_t nrows ( ) const;

    size_t ncols ( ) const;
    
    Matrix<T> permute ( std::vector<int> perm ) const;

    std::vector<T> upperTriangle() const;

    std::vector<T> lowerTriangle() const;
    
private:
    std::vector<T>  m_elements;
    size_t          m_nRows;
    size_t          m_nCols;
};



template<class T>
Matrix<T>::Matrix(int n, int m) :
    m_nRows(n),
    m_nCols(m),
    m_elements(n*m)
{
}

template<class T>
Matrix<T>::~Matrix()
{
}

template<class T>
const T& Matrix<T>::get(int i, int j) const {
    return m_elements[i * m_nCols + j];
}

template<class T>
void Matrix<T>::set(int i, int j, const T& value) {
    m_elements[i * m_nCols + j] = value;
}

template<class T>
size_t Matrix<T>::nrows() const{
    return m_nRows;
}


template<class T>
size_t Matrix<T>::ncols() const {
    return m_nCols;
}


template<class T>
Matrix<T> Matrix<T>::permute( std::vector<int> perm ) const {
    Matrix<T> permuted( nrows(), ncols() );
    for (int i=0; i < nrows(); ++i) {
        for (int j=0; j < ncols(); ++j) {
            int pi = i < perm.size() ? perm[i] : i;
            int pj = j < perm.size() ? perm[j] : j;
            permuted.set(i, j, get(pi, pj));
        }
    }
    return permuted;
}


template<class T>
std::vector<T> Matrix<T>::upperTriangle() const {
    std::vector<T> ut;
    for (int i=0; i < nrows(); ++i) {
        for (int j=i+1; j < ncols(); ++j) {
            ut.push_back(get(i,j));
        }
    }
    return ut;
}

template<class T>
std::vector<T> Matrix<T>::lowerTriangle() const {
    std::vector<T> lt;
    for (int i=0; i < ncols(); ++i) {
        for (int j=0; j < i; ++j) {
            lt.push_back(get(i,j));
        }
    }
    return lt;
}

#endif