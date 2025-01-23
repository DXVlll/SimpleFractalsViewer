
#ifndef SOME_TRANSFORM_CPP
#define SOME_TRANSFORM_CPP

#include "SomeTransform.hpp"

namespace dxvll_space_2d{

    SomeTransform::SomeTransform():SomeTransform(FMatrix<double>::getIdentityMatrix(3)){}
    SomeTransform::SomeTransform(const FMatrix<double> &mx){
        if (!mx.squareLike() || mx.getColumnsCount() != 3){
            throw invalid_argument("you are trying to create 2d transform with incorrect matrix.");
        }
        this->matrix.resizeAndClear(mx.getLinesCount(), mx.getColumnsCount());
        this->matrix = mx;
    }
    SomeTransform::SomeTransform(const SomeTransform &other){
        this->matrix.resizeAndClear(other.matrix.getLinesCount(), other.matrix.getColumnsCount());
        this->matrix = other.matrix;
        this->rotAngle = other.rotAngle;
        this->scaleCoef = other.scaleCoef;
        this->translateBias = other.translateBias;
        this->inverseMatrix.resizeAndClear(other.inverseMatrix.getLinesCount(), other.inverseMatrix.getColumnsCount());
        this->inverseMatrix = other.inverseMatrix;
    }

    void SomeTransform::recalculateMatrices(){
        this->matrix.resizeAndClear(3,3);
        matrix = FMatrix<double>::getIdentityMatrix(3);
        //if (scaleCoef.x() == 0 || scaleCoef.y() == 0){
            //throw invalid_argument("you are trying to create 2d transform with incorrect matrix.");
        //}
        FMatrix<double> scaleMx(FMatrix<double>::getIdentityMatrix(3));
        scaleMx.elem(0,0) = scaleCoef.x();
        scaleMx.elem(1,1) = scaleCoef.y();
        FMatrix<double> rotMx(FMatrix<double>::getIdentityMatrix(3));
        rotMx.elem(0,0) = cos(rotAngle); rotMx.elem(0,1) = -sin(rotAngle);
        rotMx.elem(1,0) = sin(rotAngle); rotMx.elem(1,1) = cos(rotAngle);
        FMatrix<double> translateMx(FMatrix<double>::getIdentityMatrix(3));
        translateMx.elem(2,0) = translateBias.x();
        translateMx.elem(2,1) = translateBias.y();
        //cout << "mxs before uniting \n (scale) \n" << scaleMx.toString(false);
        //cout << " (rot) \n" << rotMx.toString(false);
        //cout << " (translate)\n" << translateMx.toString(false) << endl;
        inverseMatrix.resizeAndClear(3,3);
        matrix = rotMx*scaleMx;
        matrix.elem(2,0) = translateMx.elem(2,0);
        matrix.elem(2,1) = translateMx.elem(2,1);
        inverseMatrix = findInverse(matrix);
    }


    
    const FMatrix<double> &SomeTransform::getMatrix()const{
        return this->matrix;
    }
    /*void SomeTransform::setMatrix(const FMatrix<double> &mx){
        if (!mx.squareLike() || mx.getColumnsCount() != 3){
            throw invalid_argument("you are trying to make 2d transform with incorrect matrix.");
        }
        this->matrix = mx;
    }*/

    PointV SomeTransform::apply(const PointV &p, const PointV &origin)const{
        PointV res = p - origin;
        res.getVec() = res.getVec()*matrix;
        res = res + origin;
        return res;
    }
    PointV SomeTransform::cancel(const PointV &p, const PointV &origin)const{
        PointV res = p - origin;
        res.getVec() = res.getVec()*inverseMatrix;
        res = res + origin;
        return res;
    }

    void SomeTransform::setRotation(double angle){
        this->rotAngle = angle;
    }
    void SomeTransform::setScale(PointV coef){
        //if (coef.x() == 0 || coef.y() == 0){
            //throw invalid_argument("you are trying to create 2d transform with incorrect matrix.");
        //}
        this->scaleCoef = coef;
    }
    void SomeTransform::setTranslation(PointV bias){
        this->translateBias = bias;
    }

    const double &SomeTransform::getRotation()const{
        return this->rotAngle;
    }
    const PointV &SomeTransform::getScale()const{
        return this->scaleCoef;
    }
    const PointV &SomeTransform::getTranslation()const{
        return this->translateBias;
    }

    FMatrix<double> SomeTransform::findInverse(const FMatrix<double> &input){
        if (!input.squareLike()){
            throw invalid_argument("We can't find inverse matrix of non-quadratic matrix.");
        }
        long linesCount = input.getLinesCount();
        long columnsCount = input.getColumnsCount();
        FMatrix<double> equationsSystem = input | FMatrix<double>::getIdentityMatrix(linesCount);
        FMatrix<double> almostRes = applyGauss(equationsSystem, linesCount);
        FMatrix<double> res(linesCount, columnsCount, 0.0);
        for (long i = 0; i < linesCount; ++i){
            for (long j = 0; j < columnsCount; ++j){
                res.elem(i,j) = almostRes.elem(i, j+columnsCount);
            }
        }
        return res;
    }
    FMatrix<double> SomeTransform::applyGauss(const FMatrix<double> &input, long variablesCount){
        FMatrix<double> res = input;
        long linesCount = res.getLinesCount();
        if (variablesCount > linesCount){
            throw invalid_argument("We can't apply gauss method "
                    "to given matrix with given number of variables.");
        }
        long lim = variablesCount;

        for (long i = 0; i < variablesCount; ++i){
            if (res.elem(i,i) == 0){
                int j = i+1;
                while((j < linesCount) && (res.elem(j,i) == 0)){
                    ++j;
                }
                if (j == linesCount)
                    throw invalid_argument("It looks like we can't solve given equation system.");
                res.swapLines(i,j);
            }
            if (res.elem(i,i) != 1){
                vector<double> &vec = res.getLine(i);
                vec = vec*(1.0/res.elem(i,i));
            }
            for (long j = i+1; j < linesCount; ++j){
                vector<double> &vec = res.getLine(i);
                res.addVecToLine(vec*(0.0 - res.elem(j,i)), j);
            }
        }
        for (long i = 1; i < variablesCount; ++i){
            for (long j = i-1; j >= 0; --j){
                vector<double> &vec = res.getLine(i);
                res.addVecToLine(vec*(0.0 - res.elem(j,i)), j);
            }
        }
        return res;
    }

    SomeTransform SomeTransform::operator=(const SomeTransform & other){
        this->matrix.resizeAndClear(other.matrix.getLinesCount(), other.matrix.getColumnsCount());
        this->inverseMatrix.resizeAndClear(other.inverseMatrix.getLinesCount(), other.inverseMatrix.getColumnsCount());
        this->matrix = other.matrix;
        this->inverseMatrix = other.inverseMatrix;
        this->rotAngle = other.rotAngle;
        this->scaleCoef = other.scaleCoef;
        this->translateBias = other.translateBias;
        return (*this);
    }

    string SomeTransform::toString(bool details) const {
        ostringstream oss; // Using ostringstream to build the string
        
        oss << "Transform (rot:" << rotAngle << " scaleCoef:" << scaleCoef.toString() << " bias:" << translateBias.toString() << ")";
        
        if (details){
            oss << ":" << endl << "matrix:\n" << this->matrix.toString(false);
            oss << endl << "inverseMatrix:\n" << this->inverseMatrix.toString(false);
        }
        
        return oss.str(); // Return the accumulated string
    }
}

#endif // SOME_TRANSFORM_CPP