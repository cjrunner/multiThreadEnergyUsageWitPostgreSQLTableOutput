//
//  setupForMultiFit.cpp
//  energyUsageWithoutThe_PQXX_Bullshit
//
//  Created by Clifford Campo on 5/20/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <string>
#include <stdexcept>
#include <iterator>
#include <sstream>

#include <stdio.h>

#include <chrono>
//#include "setupForMultiFit.hpp"
#include "/Users/cjc/c++/energyUsage/multiThreadEnergyUsageWithPostgreSQLTableOutput/multiThreadEnergyUsageWithPostgreSQLTableOutput/setupForMultiFit.hpp"
#include "myPrototypes.hpp"
#include "baseClass.hpp"
std::ostringstream *current_Date_Time(BaseClass *);  //Latest c++ version replaces the C version currentDateTime
SetupForMultiFit::~SetupForMultiFit() {  //Destructor
    gsl_matrix_free(this->independentVariable);
    gsl_matrix_free(this->covarienceMatrix);
    gsl_matrix_set_zero(this->covarienceMatrix);
    gsl_vector_free(this->dependentVariable);
    gsl_vector_free(this->weights);
    gsl_vector_free(this->coefficients);
    gsl_multifit_linear_free(this->work);
    gsl_vector_free(this->computedDependetVariable);
    
}
SetupForMultiFit::SetupForMultiFit(BaseClass *thisIsReallyAPointerToBaseClass, size_t polynomialDegree,  size_t count, double *aOV)   { //Constructor
    this->bc = thisIsReallyAPointerToBaseClass;
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
    this->computedDependetVariable = gsl_vector_alloc(this->numberOfEntries);
    this->weights = gsl_vector_alloc(this->numberOfEntries);
    //    gsl_vector_set_zero(this->weights);
    this->coefficients = gsl_vector_alloc(this->onePlusPolynomialDegree);
    //    gsl_vector_set_zero(this->coefficients);
    this->work = gsl_multifit_linear_alloc(this->numberOfEntries, this->onePlusPolynomialDegree);
}
int    SetupForMultiFit::doMultiFit(void) {
    this->rc = gsl_multifit_wlinear(this->independentVariable, \
                                    this->weights, \
                                    this->dependentVariable, \
                                    this->coefficients, \
                                    this->covarienceMatrix, \
                                    this->ptrChiSquared, \
                                    this->work);
    return this->rc;
}
void  SetupForMultiFit::setIntoMatrix(gsl_matrix *whichMatrix, int row, int column, double value) {
    gsl_matrix_set(whichMatrix, row, column, value);
}
void  SetupForMultiFit::setIntoVector(gsl_vector *whichVector, int row, double value) {
    gsl_vector_set(whichVector, row, value);
}
double  SetupForMultiFit::getFromMatrix(gsl_matrix *whichMatrix, int row, int column) {
    return (gsl_matrix_get(whichMatrix, row, column));
}
double  SetupForMultiFit::getFromVector(gsl_vector *whichVector, int row) { 
    return ( gsl_vector_get(whichVector, row) );
}
int  SetupForMultiFit::outputPolynomial( const char *leadOffString, const char *trailingString) {
//2018-06-27T05:57:25 Added `BaseClass to Calling Sequence, so I can get to bc->outstring more easily.
    this->stringStreamForOutput << current_Date_Time(this->bc) << "\t" << leadOffString;
    for (this->row = 0; this->row < this->coefficients->size ; this->row++) {
        if (this->row > 0) {
            this->stringStreamForOutput << " + " << gsl_vector_get(this->coefficients, this->row) << "*T^" << this->row;
        } else {
            this->stringStreamForOutput << gsl_vector_get(this->coefficients, this->row) ;
        }
    }
    this->stringStreamForOutput << trailingString << "\n";
    return 0;
}
double  SetupForMultiFit::outputCovarianceMatrixAndReturnChi2( const char *leadOffString, const char *trailingString) {
    // unused    double firstC = *this->ptrChiSquared;
    //2018-06-27T05:57:25 Added `BaseClass to Calling Sequence, so I can get to bc->outstring more easily.
 //                *ptrbc->outstringOS << smf->outputCovarianceMatrixAndReturnChi2(ptrbc, "cov = [", cases[ptrbc->debugFlags.intMyCase] ) << "☜ χ-squared" << cases[ptrbc->debugFlags.intMyCase] << "\n";
    for (this->row = 0; this->row < this->covarienceMatrix->size1; this->row++) {

        this->stringStreamForOutput << current_Date_Time(this->bc) << "\t" << leadOffString;
        for (this->col = 0; this->col < this->covarienceMatrix->size2; this->col++) {
            stringStreamForOutput  << *(this->covarienceMatrix->data + this->covarienceMatrix->size1*this->row + this->col);
            //Here we will insert the value of covariance matrix into our array of data reserved for it;
            this->oneFloatValue = *(this->covarienceMatrix->data + this->covarienceMatrix->size1*this->row + this->col);
            *(this->ptrCovarianceMatrixArray + this->covarienceMatrix->size1 * this->row + this->col) = this->oneFloatValue;
            if (col < this->covarienceMatrix->size1 - 1) {
               stringStreamForOutput << ", "; //Output a comma to separate covarient values
            } else {
                //Output closed brackets if we've just output the rightmost covarient value for this row.
                stringStreamForOutput << "]\t " << trailingString << "\n";
            }
        }  //End of inner for loop (col)
    } // End of outter for loop (row)
    return *this->ptrChiSquared;
}
double SetupForMultiFit::returnChiSquared( ) {
    return *this->ptrChiSquared;
}
double  SetupForMultiFit::computeCorrelationBetweenIndependentAndDependentVariables ( ) {
   
    this->trialComputedDependentVariable=0;
    this->A = gsl_vector_get(this->coefficients, 0);
    this->B = gsl_vector_get(this->coefficients, 1);
    this->C = gsl_vector_get(this->coefficients, 2);
    this->D = gsl_vector_get(this->coefficients, 3);
    for (this->row=0; this->row < this->numberOfEntries; this->row++) {
       this->T = this->bc->indVarArray[this->row];
        this->trialComputedDependentVariable = this->A + (this->B  +  (this->C  + this->D *this->T )* this->T) * this->T; 
        this->bc->depVarArray[this->row]= trialComputedDependentVariable;
    }
    this->meanOfIndependentVariable = gsl_stats_mean(bc->BaseClass::indVarArray, 1, this->numberOfEntries);
    this->meanOfDependentVariables = gsl_stats_mean(bc->BaseClass::depVarArray, 1, this->numberOfEntries);
    this->correlationCoefficient = gsl_stats_correlation(bc->BaseClass::indVarArray, 1, bc->BaseClass::depVarArray, 1, this->numberOfEntries );

    return this->correlationCoefficient ;
}

double  SetupForMultiFit::computeGoodnessOfResults ( ) {
    //computeGoodnessOfResults defines GoodnessOfResults as the square of the accumulated normalized difference between the expected (modeled)
    //and observed dependent variables ( ∑(((modeledResultForEachIndependentVariable - eachObservedDependentVariable)/modeledResultForEachIndependentVariable))^2 ).
    //Return the square root of the sum of the squares of the percent difference between the expected (modeled) and observed dependent variables.
    double trialComputedDependentVariable=0;
    double accumulatedVarience2 = 0;
    double delta=0;
    double deltaNormalized=0;
    const double A = gsl_vector_get(this->coefficients, 0);
    const double B = gsl_vector_get(this->coefficients, 1);
    const double C = gsl_vector_get(this->coefficients, 2);
    const double D = gsl_vector_get(this->coefficients, 3);
    double T;
    for (this->row=0; this->row <  this->bc->grc; this->row++) {
        T = *(this->bc->ptrIndVariableArray + this->row);
        trialComputedDependentVariable = A + (B  +  (C  + D *T )* T) * T;
        *(this->bc->ptrExpDepVarArray + this->row)= trialComputedDependentVariable;
        delta = (*(this->bc->ptrExpDepVarArray + this->row) - *(this->bc->ptrObsDepVarArray + this->row));
        *(this->bc->ptrExpMinusObsDepVarArray + this->row) = delta;
        deltaNormalized =  delta/(*(this->bc->ptrExpDepVarArray + this->row) );
        accumulatedVarience2 += ( deltaNormalized *  deltaNormalized ); //Accumulate the square of normalized varience.
    }
    
    return sqrt(accumulatedVarience2) ; //Return the square root of the sum of the squares of the normalized varience
}
double SetupForMultiFit::computeTrace(double *someMatrix, int onePlusPolynomialDegree) {
    int &matrixSize = onePlusPolynomialDegree;
    this->traceOfCovarientMatrix = 0;
    for (this->loopCounter =0; this->loopCounter < matrixSize; this->loopCounter++  ) {
        this->traceOfCovarientMatrix += *(someMatrix + (matrixSize + 1) * this->loopCounter ); //∑ along the diagonal of the covarient matrix.
    }
    return this->traceOfCovarientMatrix; 
}
void  SetupForMultiFit::captureIndependentVariableValues( int theRow, double  valueArray[]) {    
    this->dblwork = *(valueArray +  _stddeveu);
    if (this->bc->debugFlags.debug2) {
        std::cout << theRow << ". Weights, or *(valueArray +  _stddeveu), Looks like: " << *(valueArray +  _stddeveu) << std::endl;
    }
    gsl_vector_set(this->weights, theRow, this->dblwork);
    this->dblwork = *(valueArray +  _avgeu);
    gsl_vector_set(this->dependentVariable, theRow, this->dblwork );
    this->computedValue = this->initialValue;
    for(this->loopCounter = 0; this->loopCounter< this->onePlusPolynomialDegree; this->loopCounter++)  {   // Note that this->loopCounter Serves as the column counter, too.
        gsl_matrix_set(this->independentVariable, theRow, this->loopCounter, this->computedValue);
        this->computedValue *= valueArray[_avgtemp]; //For each column in this current row, place progressively increasing powers of the independent variable
    }
    
}
double SetupForMultiFit::getCoefficient(int coeffID) {
    return gsl_vector_get(this->coefficients, coeffID );
}
double SetupForMultiFit::getCorrelationCoefficient(void) {
    return this->correlationCoefficient;
}
double SetupForMultiFit::getChi2(void) {
    return *this->ptrChiSquared;
}
