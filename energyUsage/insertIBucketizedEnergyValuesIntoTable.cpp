//
//  insertIBucketizeEnergyValuesIntoTable.cpp
//  energyUsage
//
//  Created by Clifford Campo on 5/5/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//


//
//  insertIntoTable.cpp
//  c++Postgres
//
//  Created by Clifford Campo on 5/1/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//
#include <iostream>

#include <pqxx/pqxx>


#include "insertIBucketizedEnergyValuesIntoTable.hpp"
#include "myPrototypes.hpp"


#include "makeConnectionString.hpp"

using namespace std;
using namespace pqxx;
extern const char *dropTable;
extern const char *createTable;
extern const char *insertIntoTable;
int insertBucketizedEnergyValuesIntoTable(const char *connStr, \
                                          BitFlags *mcs, \
                                          double avgtemp, \
                                          double avgeu, \
                                          double stddeveu, \
                                          double mineu, \
                                          double maxeu, \
                                          int counteu, \
                                          int8_t disconnectFlag, \
                                          int8_t processingFlag) {
    //    int rc=0;
    char *sql; //Need to do const char *sql rather than just char *sql to make the C++11 compiler happy.
    char sqlbuffer[200];
    sql = sqlbuffer;

    pqxx::connection *ptrForInsert = new pqxx::connection (connStr);  /* << =================================== */ //W A R N I N G: new, unlike calloc, does not pre-initialize acquired memory to a known state.
    if (ptrForInsert->is_open()) {
        if (mcs->debug2) std::cout << "We successfully opened a second connection to database " << ptrForInsert->dbname() << " for doing inserts. " << std::endl;
        ptrForInsert->activate();
    } else {
        std::cerr << "We failed to another connection to database for doing simultaneous inserts into another table " << std::endl;
    }

    sprintf (sqlbuffer, insertIntoTable, avgtemp, avgeu, stddeveu, mineu, maxeu, counteu); //CREATE the insert into SQL statement
    if ( mcs->debug3  ) std::cout << "Insert looks like: " << sqlbuffer << std::endl;
    work WI(*ptrForInsert);  //Create a work object, WI.
    /* Execute the INSERT INTO SQL statement we just constructed with above sprintf statement */

    WI.exec( sqlbuffer );
    WI.commit();
    
    if (mcs->debug3) {
        if (mcs->debug3) std::cout << "Record inserted successfully" << endl;
    }

//    WI.~transaction(); //End the insert transaction by destroying the work object, WI.
    ptrForInsert->disconnect();
    return 0;
}
//  C L A S S    S E T U P F O R M U L T I F I T ' s      M E M B E R    F U N C T I O N S
SetupForMultiFit::~SetupForMultiFit() {  //Destructor
    gsl_matrix_free(this->independentVariable);
    gsl_matrix_free(this->covarienceMatrix);
    gsl_matrix_set_zero(this->covarienceMatrix);
    gsl_vector_free(this->dependentVariable);
    gsl_vector_free(this->weights);
    gsl_vector_free(this->coefficients);
    gsl_multifit_linear_free(this->work);
    
}
SetupForMultiFit::SetupForMultiFit(size_t polynomialDegree,  size_t count, double *aOV) { //Constructor 
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
int SetupForMultiFit::doMultiFit(void) {
    this->rc = gsl_multifit_wlinear(this->independentVariable, \
                                    this->weights, \
                                    this->dependentVariable, \
                                    this->coefficients, \
                                    this->covarienceMatrix, \
                                    this->ptrChiSquared, \
                                    this->work);
    return this->rc;
}
void SetupForMultiFit::captureIndependentVariableValues( int theRow, double  valueArray[]) {
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

void SetupForMultiFit::setIntoMatrix(gsl_matrix *whichMatrix, int row, int column, double value) {
    gsl_matrix_set(whichMatrix, row, column, value);
}
void SetupForMultiFit::setIntoVector(gsl_vector *whichVector, int row, double value) {
    gsl_vector_set(whichVector, row, value);
}
double SetupForMultiFit::getFromMatrix(gsl_matrix *whichMatrix, int row, int column) {
    return (gsl_matrix_get(whichMatrix, row, column));
}
double SetupForMultiFit::getFromVector(gsl_vector *whichVector, int row) {
    return ( gsl_vector_get(whichVector, row) );
}
int SetupForMultiFit::outputPolynomial(const char *leadOffString) {
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
double SetupForMultiFit::outputCovarianceMatrix(const char *leadOffString) {
    double firstC = *this->ptrChiSquared;
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
