
#ifndef F_MATRIX_CPP
#define F_MATRIX_CPP

#include "FMatrix.hpp"

namespace dxvlll_Matrix_24_11{

    
    template <typename ElementType>
    FMatrix<ElementType>::FMatrix(long linesNum, long columnsNum, ElementType defaultValue)
            : elements(linesNum, std::vector<ElementType>(columnsNum, defaultValue)){
        this->linesCount = linesNum;
        this->columnsCount = columnsNum;
        
    }
    template <typename ElementType>
    FMatrix<ElementType>::FMatrix():FMatrix(0,0){}

    template <typename ElementType>
    FMatrix<ElementType>::FMatrix(const FMatrix<ElementType> &other)
                : FMatrix(other.linesCount, other.columnsCount){
        
        for (int i = 0; i < other.linesCount; ++i){
            for (int j = 0; j < other.columnsCount; ++j){
                this->elements.at(i).at(j) = other.elements.at(i).at(j);
            }
        }
    }
    template <typename ElementType>
    vector<ElementType> &FMatrix<ElementType>::getLine(long id){//Since we store matrix as vector of lines 
        return this->elements.at(id);// it is quite easy to get matrix's line
    }
    template <typename ElementType>
    const vector<ElementType> &FMatrix<ElementType>::getLine(long id)const{
        return this->elements.at(id);
    }
    
    template <typename ElementType>
    vector<ElementType*> FMatrix<ElementType>::getColumn(long id){//The only way to resize matrix is to clear it.
        vector<ElementType*> res(this->linesCount, nullptr);
        for (int i = 0; i < linesCount; ++i){// Brcause of that we have something like guarantee that if column's element points to a garbage,
            res.at(i) = &this->elements.at(i).at(id);// then that column does not exist anymore.
        }
        return res;
    }
    template <typename ElementType>
    vector<const ElementType*> FMatrix<ElementType>::getColumn(long id)const{
        vector<const ElementType*> res(this->linesCount, nullptr);
        for (int i = 0; i < linesCount; ++i){
            res.at(i) = &this->elements.at(i).at(id);
        }
        return res;
    }
    
    
    template <typename ElementType>
    template <size_t Rows, size_t Cols>// we can accept static arrays and set values to matrix from them
    void FMatrix<ElementType>::setElements(const ElementType (&elements)[Rows][Cols]){
        if (Rows != linesCount || Cols != columnsCount) {
            throw std::invalid_argument("Array 'elements' dimensions do not match matrix dimensions");
        }
        for (int i = 0; i < Rows; ++i){
            for (int j = 0; j < Cols; ++j){
                elem(i,j) = elements[i][j];
            }
        }
    }
    
    template <typename ElementType>
    ElementType &FMatrix<ElementType>::elem(long ln, long col){// get access to element
        return this->elements.at(ln).at(col);// from line ln and column col (numeration starts from 0)
    }
    template <typename ElementType>
    const ElementType &FMatrix<ElementType>::elem(long ln, long col) const{
        return this->elements.at(ln).at(col);
    }
    template <typename ElementType>// calculate sum of all matrix elements
    ElementType FMatrix<ElementType>::getElemsSum() const{
        ElementType sum;
        for (int i = 0; i < this->linesCount; ++i){
            for (int j = 0; j < this->columnsCount; ++j){
                sum = sum + this->elements.at(i).at(j);
            }
        }
        return sum;
    }
    template <typename ElementType> // calculate determinant
    ElementType FMatrix<ElementType>::getDeterminant() const{
        if (!this->squareLike()){
            throw invalid_argument("We know only how to get determinant of quadratic matrices.");
        }
        ElementType res = 0;
        if (linesCount == 2){
            res = (this->elem(0,0)*this->elem(1,1)) - (this->elem(1,0)*this->elem(0,1));
        } else {
            
            for (int i = 0; i < linesCount; ++i){
                if (i % 2 == 0)
                    res = res + this->elem(i,0)*(this->makeMatrixWithoutLineCol(i,0).getDeterminant());
                else
                    res = res - this->elem(i,0)*(this->makeMatrixWithoutLineCol(i,0).getDeterminant());
            }
        }
       
        return res;
    }
    template <typename ElementType>
    FMatrix<ElementType> FMatrix<ElementType>::getTransposed()const{
        FMatrix<ElementType> res(columnsCount, linesCount);
        for (int i = 0; i < linesCount; ++i){
            for (int j = 0; j < columnsCount; ++j){
                res.elem(j,i) = this->elem(i,j);
            }
        }
        return res;
    }
    template <typename ElementType> // We define this function only to use then it in getDeterminant()
    FMatrix<ElementType> FMatrix<ElementType>::makeMatrixWithoutLineCol(long ln, long col) const{// remove line ln and column col 
                            // (actually we create new matrix from current, but without mentioned line and column)
        FMatrix res(this->linesCount-1, this->columnsCount-1);
        for (int i = 0; i < res.linesCount; ++i){
            for (int j = 0; j < res.columnsCount; ++j){
                if (i < ln){}
                res.elem(i,j) = this->elem((i<ln)?i:(i+1),(j<col)?j:(j+1));
            }
        }
        return res;
    }
    template <typename ElementType>// one of allowed actions when seaching for standart generating matrix
    void FMatrix<ElementType>::addVecToLine(const vector<ElementType> &vec, long ln){
        if (vec.size() != columnsCount){
            throw invalid_argument("addVecToLine: size of vector is not good.");
        }
        vector<ElementType> &line = getLine(ln);
        for (int i = 0; i < vec.size(); ++i){
            line.at(i) = line.at(i) + vec.at(i);
        }
        return;//also we can multiply line by number before adding by using specially defined for that operator
    }
    template <typename ElementType>// one of allowed actions when seaching for standart generating matrix
    void FMatrix<ElementType>::swapLines(long ln1, long ln2){
        swap(this->getLine(ln1), this->getLine(ln2));
        
        return; 
    }

    template <typename ElementType>
    bool FMatrix<ElementType>::squareLike() const{
        return linesCount == columnsCount;
    }
    template <typename ElementType>
    long FMatrix<ElementType>::getLinesCount() const{
        return this->linesCount;
    }
    template <typename ElementType>
    void FMatrix<ElementType>::setLinesCount(long newCount){
        this->linesCount = newCount;
    }
    template <typename ElementType>
    long FMatrix<ElementType>::getColumnsCount() const{
        return this->columnsCount;
    }
    template <typename ElementType>
    void FMatrix<ElementType>::setColumnsCount(long newCount){
        this->columnsCount = newCount;
    }

    template <typename ElementType>
    bool FMatrix<ElementType>::compatibleForAdd(const FMatrix<ElementType> &first, const FMatrix<ElementType> &second){
        return (first.linesCount == second.linesCount) && (first.columnsCount == second.columnsCount);
    }
    template <typename ElementType>
    bool FMatrix<ElementType>::compatibleForMult(const FMatrix<ElementType> &first, const FMatrix<ElementType> &second){
        return (first.columnsCount == second.linesCount);
    }
    template <typename ElementType>
    void FMatrix<ElementType>::resizeAndClear(long linesNum, long columnsNum){// the only way to resize matrix also clears it.
        this->linesCount = linesNum;    // we can say that philosofically it is like creation of new matrix with other dimentions
        this->columnsCount = columnsNum;
        this->elements.clear();
        this->elements.resize(linesNum, vector<ElementType>(columnsNum, 0));
        return;
    }
    template <typename ElementType>
    FMatrix<ElementType> FMatrix<ElementType>::operator=(const FMatrix<ElementType> &other){
        if (!compatibleForAdd((*this), other)){
            cout << "m1: " << this->toString();
            cout << "m2: " << other.toString();
            throw invalid_argument("can't assign matrix of different dimensions to current matrix.");
        }
        if (this != (&other)){
            //FMatrix<ElementType> res(other.linesCount, other.columnsCount);
            //this->elements.resize(other.linesCount);
            for (int i = 0; i < other.linesCount; ++i){
                //this->elements.at(i).resize(other.columnsCount);
                for (int j = 0; j < other.columnsCount; ++j){
                    this->elements.at(i).at(j) = other.elements.at(i).at(j);
                }
            }
            //return res;
        }
        return (*this);
    }
    template <typename ElementType>
    FMatrix<ElementType> operator+(const FMatrix<ElementType> &first, const FMatrix<ElementType> &second){
        FMatrix<ElementType> res(first.getLinesCount(), second.getColumnsCount(), 0);
        if (FMatrix<ElementType>::compatibleForAdd(first, second)){
            for (int i = 0; i < first.getLinesCount(); ++i){
                for (int j = 0; j < second.getColumnsCount(); ++j){
                    res.elem(i, j) = first.elem(i, j) + second.elem(i, j);
                }
            }
        }
        return res;
    }
    template <typename ElementType>
    FMatrix<ElementType> operator*(const FMatrix<ElementType> &first, const FMatrix<ElementType> &second){
        FMatrix<ElementType> res(first.getLinesCount(), second.getColumnsCount(), 0);
        //cout << "I am here\n";
        if (FMatrix<ElementType>::compatibleForMult(first, second)){
            for (int i = 0; i < first.getLinesCount(); ++i){
                for (int j = 0; j < second.getColumnsCount(); ++j){
                    const vector<ElementType> &line = first.getLine(i);
                    vector<const ElementType*> col = second.getColumn(j);
                    ElementType sum = 0;
                    for (int k = 0; k < line.size(); ++k){
                        sum = sum + line.at(k)*(*col.at(k));
                        //cout << sum << "<s, " << line.at(k) << "|" << (*col.at(k)) <<  "\n";
                    }
                    //cout << "sum:" << sum << endl;
                    
                    res.elem(i, j) = sum;
                }
            }
        } else {
            throw invalid_argument("We can't multiply matrices: not compatible dimensions.");
        }
        return res;
    }
    template <typename ElementType>
    vector<ElementType> operator*(const vector<ElementType>&vec, const FMatrix<ElementType>&matrix){
        vector<ElementType> res(matrix.getColumnsCount(), 0);
        int lim = vec.size();
        if (lim >= matrix.getLinesCount()){
            lim = matrix.getLinesCount();
            //throw invalid_argument("We can't multiply vector by matrix: not compatible dimensions.");
        }
        if (vec.size() != matrix.getLinesCount()){
            throw invalid_argument("We can't multiply vector by matrix: not compatible dimensions.");
        }
        for (int i = 0; i <  matrix.getColumnsCount(); ++i){
            vector<const ElementType*> column = matrix.getColumn(i);
            for (int j = 0; j < lim; ++j){
                res.at(i) = res.at(i) + vec.at(j)*(*column.at(j));
            }
        }
        //cout << "matrix mult:" << vecToString(vec) << "\n * \n" << matrix.toString() << "= \n" << vecToString(res);
        return res;
    }
    template <typename T>
    vector<T> operator*(const FMatrix<T>&matrix, const vector<T>&vec){
        vector<T> res(matrix.getLinesCount(), 0);
        int lim = vec.size();
        if (lim >= matrix.getColumnsCount()){
            lim = matrix.getColumnsCount();
            //throw invalid_argument("We can't multiply matrix by vector: not compatible dimensions.");
        }
        if (vec.size() != matrix.getColumnsCount()){
            throw invalid_argument("We can't multiply matrix by vector: not compatible dimensions.");
        }
        for (int i = 0; i <  matrix.getLinesCount(); ++i){
            vector<const T> line = matrix.getLine(i);
            for (int j = 0; j < lim; ++j){
                res.at(i) = res.at(i) + vec.at(j)*line.at(j);
            }
        }
        return res;
    }
    template <typename ElementType>
    FMatrix<ElementType> operator*(const ElementType &num, const FMatrix<ElementType> &matrix){
        FMatrix<ElementType> res(matrix.getLinesCount(), matrix.getColumnsCount(), 0);
        for (int i = 0; i < matrix.getLinesCount(); ++i){
            for (int j = 0; j < matrix.getColumnsCount(); ++j){
                res.elem(i, j) = num*matrix.elem(i, j);
            }
        }
        return res;
    }
    template <typename ElementType>
    FMatrix<ElementType> operator*(const FMatrix<ElementType> &matrix, const ElementType &num){
        return num*matrix;
    }
    template <typename ElementType>
    FMatrix<ElementType> operator/(FMatrix<ElementType> &matrix, const ElementType &num){
        FMatrix<ElementType> res(matrix.getLinesCount(), matrix.getColumnsCount(), 0);
        for (int i = 0; i < matrix.getLinesCount(); ++i){
            for (int j = 0; j < matrix.getColumnsCount(); ++j){
                res.elem(i, j) = matrix.elem(i, j)/num;
            }
        }
        return res;
    }
    template <typename ElementType>
    FMatrix<ElementType> operator+(const FMatrix<ElementType> &matrix, const ElementType &num){
        FMatrix<ElementType> res(matrix.getLinesCount(), matrix.getColumnsCount(), 0);
        for (int i = 0; i < matrix.getLinesCount(); ++i){
            for (int j = 0; j < matrix.getColumnsCount(); ++j){
                res.elem(i, j) = matrix.elem(i, j)+num;
            }
        }
        return res;
    }
    template <typename ElementType>
    FMatrix<ElementType> operator+(const ElementType &num, const FMatrix<ElementType> &matrix){
        return matrix+num;
    }
    template <typename ElementType>// concatenate 2 matrices with same linesCount
    FMatrix<ElementType>operator|(const FMatrix<ElementType>&first, const FMatrix<ElementType>&second){
        if (first.getLinesCount() != second.getLinesCount()){
            throw invalid_argument("Matrices not compatible for concatenation.");
        }
        FMatrix<ElementType> res(first.getLinesCount(), first.getColumnsCount()+second.getColumnsCount());
        long linesCount = res.getLinesCount();
        long columnsCount = res.getColumnsCount();
        long firstLim = first.getColumnsCount();
        for (long i = 0; i < linesCount; ++i){
            for (long j = 0; j < columnsCount; ++j){
                if (j < firstLim){
                    res.elem(i,j) = first.elem(i,j);
                } else {
                    res.elem(i,j) = second.elem(i,j-firstLim);
                }
                
            }
        }
        return res;
    }
    template <typename ElementType>
    string FMatrix<ElementType>::toString(bool withRowsColsInfo) const {
        ostringstream oss; // Using ostringstream to build the string
        if (withRowsColsInfo){
            oss << "FMatrix (" << linesCount << " x " << columnsCount << "):\n";
        }

        for (long i = 0; i < linesCount; ++i) {
            for (long j = 0; j < columnsCount; ++j) {
                oss << elements[i][j]; // Accessing each element
                if (j < columnsCount - 1) {
                    oss << " "; // Space between elements in the same row
                }
            }
            oss << "\n"; // New line after each row
        }

        return oss.str(); // Return the accumulated string
    }
    template <typename ElementType> //get some analogy of '1' for matrix world
    FMatrix<ElementType> FMatrix<ElementType>::getIdentityMatrix(long size){
        FMatrix<ElementType> res(size,size,0);
        for (long i = 0; i < size; ++i){
            res.elem(i,i) = 1;
        }// We don't need analogical function for '0'
        return res; //because all newly created matrices by default will be filled with 0 values
    }

    template <typename ElementType>
    vector<ElementType> operator*(const vector<ElementType> &vec, const ElementType &coef){
        vector<ElementType> res = vec;
        for (int i = 0; i < res.size(); ++i){
            res.at(i) = vec.at(i)*coef;
        }
        return res;
    }
    template <typename ElementType>
    vector<ElementType> operator*(const ElementType &coef, const vector<ElementType> &vec){
        return vec*coef;
    }

    template <typename ElementType>
    vector<ElementType> operator+(const vector<ElementType> &vec1, const vector<ElementType> &vec2){
        if (vec1.size() != vec2.size()){
            throw invalid_argument("We can't add given 2 vectors.");
        }
        vector<ElementType> res(vec1.size());
        for (long i = 0; i < res.size(); ++i){
            res.at(i) = vec1.at(i) + vec2.at(i);
        }
        return res;
    }
    template <typename ElementType>
    vector<ElementType> operator-(const vector<ElementType> &vec1, const vector<ElementType> &vec2){
        if (vec1.size() != vec2.size()){
            throw invalid_argument("We can't substract given 2 vectors.");
        }
        vector<ElementType> res(vec1.size());
        for (long i = 0; i < res.size(); ++i){
            res.at(i) = vec1.at(i) - vec2.at(i);
        }
        return res;
    }
    template <typename ElementType>
    bool operator==(const vector<ElementType> &vec1, const vector<ElementType> &vec2){
        if (vec1.size() != vec2.size()){
            return false;
        }
        for (long i = 0; i < vec1.size(); ++i){
            if (vec1.at(i) != vec2.at(i)){
                return false;
            }
        }
        return true;
    }
    template <typename ElementType>
    string vecToString(const vector<ElementType> &vec){
        ostringstream oss;
        oss << "(";
        if (vec.size() > 0)
            oss << vec.at(0);
        for (long j = 1; j < vec.size(); ++j) {
            oss << ", " << vec.at(j);
            
        }
        oss << ")";
        oss << "\n"; 

        return oss.str();
    }
}

#endif // SOME_MATRIX_CPP
