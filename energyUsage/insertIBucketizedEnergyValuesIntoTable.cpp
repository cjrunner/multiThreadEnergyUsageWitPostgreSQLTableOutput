//
//  insertIBucketizeEnergyValuesIntoTable.cpp
//  energyUsage
//
//  Created by Clifford Campo on 5/5/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#include "insertIBucketizedEnergyValuesIntoTable.hpp"
//
//  insertIntoTable.cpp
//  c++Postgres
//
//  Created by Clifford Campo on 5/1/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//
#include <iostream>

#ifndef __PQXX_PQXX__
#define __PQXX_PQXX__
#include <pqxx/pqxx>
#endif

#ifndef __MYPROTOTYPES__
#define __MYPROTOTYPES__
#include "myPrototypes.hpp"
#endif

using namespace std;
using namespace pqxx;
extern const char *dropTable;
extern const char *createTable;
extern const char *insertIntoTable;
int insertBucketizedEnergyValuesIntoTable(pqxx::connection *ptrC, \
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
    int rcCOpen;
    if (ptrC->is_open()) {
        std::cout << "Database, " << ptrC->dbname() << ", was already opened as we entered " __FILE__ <<  std::endl;
    } else {
        try {
            std::cout << "Database " << ptrC->dbname() << " was NOT opened as we entered " << __FILE__ << "so we'll try to open it." << std::endl;
            ptrC->activate();   //Connect to the LocalWeather Database
            rcCOpen = ptrC->is_open();
            if (rcCOpen) {
                std::cout << "Opened database: " << ptrC->dbname() << std::endl;
            }
            
            
        } catch (const std::exception &e) {
            std::cout << __FILE__ " failed to open database. " << std::endl;
            std::cerr << e.what() << std::endl;
            return 1;
        } //End of Try/catch
    } // End of if/else statement

  
    work W(*ptrC);  //Create a work object, W.

    if (processingFlag & DOINSERTINTOTABLE) {
    /* Execute SQL query */
        sprintf (sqlbuffer, insertIntoTable, avgtemp, avgeu, stddeveu, mineu, maxeu, counteu); //CREATE the insert into SQL statement
        W.exec( sql );
        W.commit();
    }
    cout << "Record inserted successfully" << endl;
    if (disconnectFlag) {
        std::cout << "In file, " __FILE__ ", we're disconnecting from the database because the disconnectFlag is set to: " << (int)disconnectFlag << std::endl;
        ptrC->disconnect();
    }
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
