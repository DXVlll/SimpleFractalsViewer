
#ifndef SOME_TRANSFORM_HPP
#define SOME_TRANSFORM_HPP

#include "PointV.hpp"
#include "FMatrix.hpp"
#include<string>
#include<sstream>
#include <cmath>

namespace dxvll_space_2d{
    using namespace dxvlll_Matrix_24_11;

    class SomeTransform{
        private:
        FMatrix<double> matrix;
        FMatrix<double> inverseMatrix;
        double rotAngle;
        PointV scaleCoef;
        PointV translateBias;

        
        public:
        SomeTransform();
        SomeTransform(const FMatrix<double> &mx);
        SomeTransform(const SomeTransform &other);

        void recalculateMatrices();
        const FMatrix<double> &getMatrix()const;

        PointV apply(const PointV &p, const PointV &origin)const;
        PointV cancel(const PointV &p, const PointV &origin)const;

        void setRotation(double angle);
        void setScale(PointV coef);
        void setTranslation(PointV bias);
        const double &getRotation()const;
        const PointV &getScale()const;
        const PointV &getTranslation()const;
        

        FMatrix<double> findInverse(const FMatrix<double> &input);
        FMatrix<double> applyGauss(const FMatrix<double> &input, long variablesCount);

        SomeTransform operator=(const SomeTransform & other);
        string toString(bool details = true) const;
    };
}

#endif // SOME_TRANSFORM_HPP