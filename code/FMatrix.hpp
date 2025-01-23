#include<vector>
#include<string>
#include<sstream>
#include<iostream>

#ifndef F_MATRIX_HPP
#define F_MATRIX_HPP

namespace dxvlll_Matrix_24_11{
    using namespace std;

    template <typename ElementType>
    class FMatrix{ // We can make matrix of elements of any type
        private: // if this type has defined operators(+,-,*,==,!=,<<) and ElementType(long)
        long columnsCount;
        long linesCount;
        vector<vector<ElementType>> elements;// We store matrix as vector of lines
        void setLinesCount(long newCount);// actually, not used setter
        void setColumnsCount(long newCount);// actually, not used setter
        public:
        FMatrix(long linesNum, long columnsNum, ElementType defaultValue = 0);
        FMatrix();
        FMatrix(const FMatrix<ElementType>&other);// FMatrix don't have empty constructor.
                                    // We must define matrix dimentions when creating it.

        vector<ElementType> &getLine(long id); //Since we store matrix as vector of lines 
        const vector<ElementType> &getLine(long id)const; // it is quite easy to get matrix's line

        vector<ElementType*> getColumn(long id); // but it is more difficult to get matrix column.
        vector<const ElementType*> getColumn(long id)const; 
        
        template <size_t Rows, size_t Cols>// we can accept static arrays and set values to matrix from them
        void setElements(const ElementType (&elements)[Rows][Cols]);
        
        ElementType &elem(long ln, long col); // get access to element from line ln and 
        const ElementType &elem(long ln, long col)const;// column col (numeration starts from 0)
        ElementType getElemsSum() const; // calculate sum of all matrix elements
        ElementType getDeterminant() const; // calculate determinant
        FMatrix<ElementType> getTransposed() const;
        FMatrix<ElementType> makeMatrixWithoutLineCol(long ln, long col) const;// remove line ln and column col 
                            // (actually we create new matrix from current, but without mentioned line and column)
        void addVecToLine(const vector<ElementType> &vec, long ln);
        void swapLines(long ln1, long ln2);
        
        bool squareLike() const;
        
        long getLinesCount() const;
        long getColumnsCount() const;

        static bool compatibleForAdd(const FMatrix<ElementType>&first, const FMatrix<ElementType>&second);
        static bool compatibleForMult(const FMatrix<ElementType>&first, const FMatrix<ElementType>&second);

        FMatrix<ElementType>operator=(const FMatrix<ElementType>&other);
        
        string toString(bool withRowsColsInfo = true) const;
        static FMatrix<ElementType> getIdentityMatrix(long size); //get some analogy of '1' for matrix world
        void resizeAndClear(long linesNum, long columnsNum); // the only way to resize matrix also clears it.
                    // we can say that philosofically it is like creation of new matrix with other dimentions
    };
    
    
    template <typename T>// mathematical operators for comfortable working with matricess
    FMatrix<T> operator+(const FMatrix<T>&first, const FMatrix<T>&second);
    template <typename T>
    FMatrix<T> operator*(const FMatrix<T>&first, const FMatrix<T>&second);
    template <typename T>
    vector<T> operator*(const vector<T>&vec, const FMatrix<T>&matrix);
    template <typename T>
    vector<T> operator*(const FMatrix<T>&matrix, const vector<T>&vec);
    template <typename T>
    FMatrix<T> operator*(const T &num, const FMatrix<T>&matrix);
    template <typename T>
    FMatrix<T> operator*(const FMatrix<T>&matrix, const T &num);
    template <typename T>
    FMatrix<T> operator/(const FMatrix<T>&matrix, const T &num);
    template <typename T>
    FMatrix<T> operator+(const FMatrix<T>&matrix, const T &num);
    template <typename T>
    FMatrix<T> operator+(const T &num, const FMatrix<T>&matrix);
    template <typename T>// concatenate 2 matrices with same linesCount
    FMatrix<T> operator|(const FMatrix<T>&first, const FMatrix<T>&second);

    template <typename T>
    vector<T> operator*(const vector<T> &first, const T &second);
    template <typename ElementType>
    vector<ElementType> operator*(const ElementType &coef, const vector<ElementType> &vec);
    template <typename ElementType>

    vector<ElementType> operator+(const vector<ElementType> &vec1, const vector<ElementType> &vec2);
    template <typename ElementType>
    vector<ElementType> operator-(const vector<ElementType> &vec1, const vector<ElementType> &vec2);
    template <typename ElementType>
    bool operator==(const vector<ElementType> &vec1, const vector<ElementType> &vec2);
    template <typename ElementType>
    string vecToString(const vector<ElementType> &vec);
}
#include "FMatrix.cpp"

#endif // SOME_MATRIX_HPP
