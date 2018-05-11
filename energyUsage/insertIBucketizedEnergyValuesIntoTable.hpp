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
    size_t loopCounter;
    gsl_multifit_linear_workspace *work;
    gsl_matrix *independentVariable;
    gsl_matrix *covarienceMatrix;
    gsl_vector *dependentVariable;
    gsl_vector *weights;
    gsl_vector *coefficients;
    int rc;
    SetupForMultiFit(size_t polynomialDegree, size_t count, double *);
    ~SetupForMultiFit();
void setIntoMatrix(gsl_matrix *, int, int, double);
void setIntoVector(gsl_vector *, int, double);
double getFromMatrix(gsl_matrix *, int, int);
double getFromVector(gsl_vector *, int);
void captureIndependentVariableValues( int, double * );
int doMultiFit(void );
int outputPolynomial(const char *);
double outputCovarianceMatrix(const char *);  //Prints out covarience matrix and returns value of χ-squared.

};
#endif /* insertIBucketizedEnergyValuesIntoTable_hpp */



