//
//  insertIBucketizedEnergyValuesIntoTable.hpp
//  energyUsage
//
//  Created by Clifford Campo on 5/5/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#ifndef insertIBucketizedEnergyValuesIntoTable_hpp
#define insertIBucketizedEnergyValuesIntoTable_hpp

#include <gsl/gsl_multifit.h>
#include "makeConnectionString.hpp" //Need this to resolve the inherited MakeConnectionString class.

enum bucketColNames  {_avgtemp, _avgeu, _stddeveu, _mineu, _maxeu, _countu}; 
class SetupForMultiFit : public MakeConnectionString {
private:
    size_t polynomialDegree; //From constructor parameter;
    size_t numberOfEntries;  // From count
    const double *arrayOfValues; //Pointer to array of current values, per enum, above, bucketColNames
    size_t &maxNumberOfRows = numberOfEntries;  // Use a c++-type reference rather than a C-type pointer.

public:

    size_t  onePlusPolynomialDegree;
    const double initialValue = 1;
    double dblwork;
    double computedValue;
    double chiSquared;
    double *ptrChiSquared;
    double TSS; //total sum of squares
    double goodnessOfFit;
    size_t loopCounter;
    gsl_multifit_linear_workspace *work;
    gsl_matrix *independentVariable;
    gsl_matrix *covarienceMatrix;
    gsl_vector *dependentVariable;
    gsl_vector *weights;
    gsl_vector *coefficients;
    int rc;
//    SetupForMultiFit(size_t polynomialDegree, size_t count, double *);
//    ~SetupForMultiFit();
//void setIntoMatrix(gsl_matrix *, int, int, double);
//void setIntoVector(gsl_vector *, int, double);
//double getFromMatrix(gsl_matrix *, int, int);
//double getFromVector(gsl_vector *, int);
//void captureIndependentVariableValues( int, double * );//
// int doMultiFit(void );
//int outputPolynomial(const char *);
//double outputCovarianceMatrix(const char *);  //Prints out covarience matrix and returns value of χ-squared.
    ~SetupForMultiFit() {  //Destructor
        gsl_matrix_free(this->independentVariable);
        gsl_matrix_free(this->covarienceMatrix);
        gsl_matrix_set_zero(this->covarienceMatrix);
        gsl_vector_free(this->dependentVariable);
        gsl_vector_free(this->weights);
        gsl_vector_free(this->coefficients);
        gsl_multifit_linear_free(this->work);
        
    }
    SetupForMultiFit(size_t polynomialDegree,  size_t count, double *aOV) { //Constructor
        this->ptrChiSquared = &chiSquared;
        this->arrayOfValues = aOV;
        this->polynomialDegree = polynomialDegree;
        this->onePlusPolynomialDegree = 1+polynomialDegree;
        this->numberOfEntries = count;                         //Number of rows
        this->independentVariable = gsl_matrix_alloc(this->numberOfEntries, this->onePlusPolynomialDegree );
        gsl_matrix_set_zero(this->independentVariable);
        this->covarienceMatrix = gsl_matrix_alloc(this->onePlusPolynomialDegree, this->onePlusPolynomialDegree);
        //    gsl_matrix_set_zero(this->covarienceMatrix);
        this->dependentVariable = gsl_vector_alloc(this->numberOfEntries);
        //   gsl_vector_set_zero(this->dependentVariable);
        this->weights = gsl_vector_alloc(this->numberOfEntries);
        //    gsl_vector_set_zero(this->weights);
        this->coefficients = gsl_vector_alloc(this->onePlusPolynomialDegree);
        //    gsl_vector_set_zero(this->coefficients);
        this->work = gsl_multifit_linear_alloc(this->numberOfEntries, this->onePlusPolynomialDegree);
    }
int    doMultiFit(void) { 
        this->rc = gsl_multifit_wlinear(this->independentVariable, \
                                        this->weights, \
                                        this->dependentVariable, \
                                        this->coefficients, \
                                        this->covarienceMatrix, \
                                        this->ptrChiSquared, \
                                        this->work);
        return this->rc;
    }
    void setIntoMatrix(gsl_matrix *whichMatrix, int row, int column, double value) {
        gsl_matrix_set(whichMatrix, row, column, value);
    }
    void setIntoVector(gsl_vector *whichVector, int row, double value) {
        gsl_vector_set(whichVector, row, value);
    }
    double getFromMatrix(gsl_matrix *whichMatrix, int row, int column) {
        return (gsl_matrix_get(whichMatrix, row, column));
    }
    double getFromVector(gsl_vector *whichVector, int row) {
        return ( gsl_vector_get(whichVector, row) );
    }
    int outputPolynomial(const char *leadOffString) {
        std::cout << leadOffString;
        for (int coeff = 0; coeff < this->coefficients->size ; coeff++) {
            if (coeff > 0) {
                std::cout << " + " << gsl_vector_get(this->coefficients, coeff) << "*T^" << coeff;
            } else {
                std::cout << gsl_vector_get(this->coefficients, coeff) ;
            }
        }
        std::cout << std::endl;
        return 0;
    }
    double outputCovarianceMatrix(const char *leadOffString) {
        // unused    double firstC = *this->ptrChiSquared;
        for (int row = 0; row < this->covarienceMatrix->size1; row++) {
            std::cout << leadOffString;
            for (int col = 0; col < this->covarienceMatrix->size2; col++) {
                std::cout  << *(this->covarienceMatrix->data + this->covarienceMatrix->size1*row + col);
                if (col < this->covarienceMatrix->size1 - 1) {
                    std::cout << ", "; //Output a comma to separate covarient values
                } else {
                    //Output closed brackets if we've just output the rightmost covarient value for this row.
                    std::cout << "]" << std::endl;
                }
            }  //End of inner for loop (col)
        } // End of outter for loop (row)
        return *this->ptrChiSquared;
    }

    
void captureIndependentVariableValues( int theRow, double  valueArray[]) {
        this->dblwork = *(valueArray +  _stddeveu);
        std::cout << "weights, or *(valueArray +  _stddeveu), Looks like: " << *(valueArray +  _stddeveu) << std::endl;
        gsl_vector_set(this->weights, theRow, this->dblwork);
        this->dblwork = *(valueArray +  _avgeu);
        gsl_vector_set(this->dependentVariable, theRow, this->dblwork );
        this->computedValue = this->initialValue;
        for(this->loopCounter = 0; this->loopCounter< this->onePlusPolynomialDegree; this->loopCounter++)  {   // Note that this->loopCounter Serves as the column counter, too.
            gsl_matrix_set(this->independentVariable, theRow, this->loopCounter, this->computedValue);
            this->computedValue *= valueArray[_avgtemp]; //For each column in this current row, place progressively increasing powers of the independent variable
        }
        
    }
};
#endif /* insertIBucketizedEnergyValuesIntoTable_hpp */



