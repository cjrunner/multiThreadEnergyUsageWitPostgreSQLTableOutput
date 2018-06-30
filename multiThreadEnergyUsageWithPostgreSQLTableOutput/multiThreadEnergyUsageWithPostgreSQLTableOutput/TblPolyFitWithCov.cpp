//
//  TblPolyFitWithCov.cpp
//  multiThreadEnergyUsageWithPostgreSQLTableOutput
//
//  Created by Clifford Campo on 6/26/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#include "TblPolyFitWithCov.hpp"
//
//  tblPolyFit.cpp
//  multiThreadEnergyUsage
//
//  Created by Clifford Campo on 6/15/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//


#include <stdio.h>
#include <iostream>
#include <netinet/in.h>
#include "/usr/local/pgsql/pgsql101/include/libpq-fe.h"
#include "setupForMultiFit.hpp"
#include "tblPolyFit.hpp"
#include "varyingType.hpp"
TblPolyFitWithCov::TblPolyFitWithCov(BaseClass *baseClass, int nv, int rsltFmt) {
    bc = baseClass;  //Initialize a pointer to our base class.
    nParams = nv;
    resultFormat = rsltFmt;
    paramValues = new const char [nv]{};
    //                              1       2         3           4         5           6         7           8           9         10         11         12         13         14         15          16        17         18
    paramTypes = new const Oid[nv]{INT2OID,FLOAT8OID, FLOAT8OID, FLOAT8OID, FLOAT8OID, FLOAT4OID, FLOAT4OID,  FLOAT4OID, FLOAT4OID,  FLOAT4OID, FLOAT4OID,  FLOAT4OID, FLOAT4OID,  FLOAT4OID, FLOAT4OID,  FLOAT4OID, FLOAT4OID,  FLOAT4OID, FLOAT4OID,  FLOAT4OID, FLOAT4OID, INT2OID, FLOAT8OID, FLOAT8OID};
//        19           20         21      22       23          24
    paramLengths = new const  int[nv]{2,8,8,8,8, 4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4, 2,8,8};
    paramFormats = new const int [nv]{1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1};
}
TblPolyFitWithCov::TblPolyFitWithCov(short pd, int nv, int rsltFmt, bool withCov) {
    //      R U N    M O D E       R U N    M O D E       R U N    M O D E       R U N    M O D E

    this->ptrToCovarianceMatrix = &this->copyOfCovarianceMatrix[0]; //Make our own copy of a pointer to the covariance matrix. \
    This class, TblPolyFit, deptends upon selectFromTable.cpp to have filled in the cavariance matrix \
    with values of type float.
    
    ptrCovNbo = new float[pd * pd]{};  //Get another array, that mirrors the original array of covariance data, but this array will have the covariance data in NetworkByteOrder.
    
    
    //At this point all covariance data should be in Network Byte Order.
    
    //connString = cs;
    nParams = nv;
    resultFormat = rsltFmt;
    paramValues = new const char [nv]{};
    paramTypes = new const Oid[nv]{INT2OID,FLOAT8OID, FLOAT8OID, FLOAT8OID, FLOAT8OID, FLOAT4ARRAYOID, INT2OID, FLOAT8OID, FLOAT8OID};
    paramLengths = new const  int[nv]{2,8,8,8,8, 16*4, 2,8,8};
    paramFormats = new const int [nv]{1,1,1,1,1,   1,  1,1,1};
    /**/
}

TblPolyFitWithCov::~TblPolyFitWithCov() {
#ifdef DOTABLETBLPOLYFITWITHCOV
    //    if(ptrSUMF == NULL) delete [] ptrCov;  //Only in test mode do we allocate ptrCov, so now we must delete it because ptrSUMF == NULL => test mode.
    delete [] ptrCovNbo;
#endif
    //   if (paramValues != nullptr || paramValues != (const char *)'\0') delete [] paramValues;
    if (paramTypes != nullptr) delete [] paramTypes;
    if (paramLengths != nullptr)delete [] paramLengths;
    if (paramFormats != nullptr) delete [] paramFormats;
}

    
int TblPolyFitWithCov::doInsertInto(const char *connString) {
    VaryingType<short> *tshort =new VaryingType<short>();
    tshort->in64.d64 = POLYNOMIALDEGREE;
    tshort->VaryingType<short>::toNetworkByteOrder();
    this->nbop1 = tshort->out64.d64;
    // --------------------------------------------
    tshort->in64.d64 = this->bc->debugFlags.intMyCase; //Reuse the short object that we created above
    tshort->VaryingType<short>::toNetworkByteOrder();
    this->nbop6 = tshort->out64.d64;
    // --------------------------------------------
    VaryingType<double> *tdouble =new VaryingType<double>();  //This tdouble object, for type double, should serve us for making input parameters p2, p3, p4, p5, p7, and p8 in network byte order.
    tdouble->in64.d64 = this->copyOfCoefficients[0];
    tdouble->VaryingType<double>::toNetworkByteOrder();
    this->nbop2 = tdouble->out64.d64;
    // --------------------------------------------
    tdouble->in64.d64 = this->copyOfCoefficients[1];
    tdouble->VaryingType<double>::toNetworkByteOrder();
    this->nbop3 = tdouble->out64.d64;
    // --------------------------------------------
    tdouble->in64.d64 = this->copyOfCoefficients[2];
    tdouble->VaryingType<double>::toNetworkByteOrder();
    this->nbop4 = tdouble->out64.d64;
    // --------------------------------------------
    tdouble->in64.d64 = this->copyOfCoefficients[3];
    tdouble->VaryingType<double>::toNetworkByteOrder();
    this->nbop5 = tdouble->out64.d64;
    
    // -------------------------------------------- Now, convert all {[(1+polynomialdegree)*(1+polynomialdegree)]=>16 for polynomialdegree = 3} elements comprising covariance matrix.
     VaryingType<float> *tfloat =new VaryingType<float>();  //This tdouble object, for type double, should serve us for making input parameters p2, p3, p4, p5, p7, and p8 in network byte order.
    for (this->intLoopCounter = 0; this->intLoopCounter < NUMBEROFCOVARIANCEMATRIXENTRIES; this->intLoopCounter++) {
        //*(this->ptrCovNbo + i)
        //Note: The constructor should have converted the test data (the test data is defined in the constructor) to network byte order.
        tfloat->in64.d64 = *(this->ptrToCovarianceMatrix+this->intLoopCounter);
        tfloat->VaryingType<float>::toNetworkByteOrder();
        *(this->ptrCovNbo+this->intLoopCounter) = tfloat->out64.d64;
    }
    // --------------------------------------------
    // We did the second short, above.
    // --------------------------------------------
    tdouble->in64.d64 = this->correlationCoefficient;  //correlation coefficient.
    tdouble->VaryingType<double>::toNetworkByteOrder();
    this->nbop7 = tdouble->out64.d64;
    // --------------------------------------------
    tdouble->in64.d64 = this->chi2;  //chi-squared
    tdouble->VaryingType<double>::toNetworkByteOrder();
    this->nbop8 = tdouble->out64.d64;
    // --------------------------------------------
    // Note how c++ initializes an array ***without the use of an equal sign*** between the closing `]` and the opening `{`.
    Oid *arrayOfOid = new Oid[24]{INT2OID,FLOAT8OID, FLOAT8OID, FLOAT8OID, FLOAT8OID, FLOAT4OID, FLOAT4OID,  FLOAT4OID, FLOAT4OID,  FLOAT4OID, FLOAT4OID,  FLOAT4OID, FLOAT4OID,  FLOAT4OID, FLOAT4OID,  FLOAT4OID, FLOAT4OID,  FLOAT4OID, FLOAT4OID,  FLOAT4OID, FLOAT4OID, INT2OID, FLOAT8OID, FLOAT8OID};
    this->paramTypes = arrayOfOid;
    //What follows is the crazyiest type casting I've ever seen. See The PostgreSQL 9.0 Reference Manual, Volume 2: Programming Guide, bottom of page 82. I highlighted this in yellow.
    const char **arrayOfparamValues = new const char *[24]{ (char *)&this->nbop1, (char *)&this->nbop2, (char *)&this->nbop3, (char *)&this->nbop4, (char *)&this->nbop5, \
        (char *)&(*(this->ptrCovNbo + 0)), (char *)&(*(this->ptrCovNbo + 1)), (char *)&(*(this->ptrCovNbo + 2)), (char *)&(*(this->ptrCovNbo + 3)), \
        (char *)&(*(this->ptrCovNbo + 4)), (char *)&(*(this->ptrCovNbo + 5)), (char *)&(*(this->ptrCovNbo + 6)), (char *)&(*(this->ptrCovNbo + 7)), \
        (char *)&(*(this->ptrCovNbo + 8)), (char *)&(*(this->ptrCovNbo + 9)), (char *)&(*(this->ptrCovNbo + 10)), (char *)&(*(this->ptrCovNbo + 11)), \
        (char *)&(*(this->ptrCovNbo + 12)), (char *)&(*(this->ptrCovNbo + 13)), (char *)&(*(this->ptrCovNbo + 14)), (char *)&(*(this->ptrCovNbo + 15)), \
        (char *)&this->nbop6, (char *)&this->nbop7, (char *)&this->nbop8};
    this->paramValues = *arrayOfparamValues; 
    int *arrayOfParamLengths = new  int[24]{sizeof(short), sizeof(double), sizeof(double), sizeof(double), sizeof(double), sizeof(float), sizeof(float),  sizeof(float), sizeof(float),  sizeof(float), sizeof(float), sizeof(float), sizeof(float),  sizeof(float), sizeof(float),  sizeof(float), sizeof(float),  sizeof(float), sizeof(float),  sizeof(float), sizeof(float), sizeof(short), sizeof(double), sizeof(double)};
    this->paramLengths = arrayOfParamLengths;
    const int *arrayOfParamFormats = new const int[24]{BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS, BINARYRESULTS};
    this->paramFormats = arrayOfParamFormats;
    // --------------------------------------------
    this->conn  = PQconnectdb(connString);
    if(PQstatus(this->conn) != CONNECTION_OK) {
        std::cout << "Connection to database failed: " << PQerrorMessage(this->conn) << std::endl;
        PQfinish(this->conn);
        exit(1);
    } else {
        std::cout << "Looks like we connected to database ok." << std::endl;
    }
    // -------------------------------------------- Candidate function not viable: no known conversion from 'const char *' to 'const char *const *' for 5th argument; take the address of the argument with &
    //                         √                √                 √               √              √                  √                √               √
    this->res = PQexecParams(this->conn, this->insertIntoCov, this->nParams, this->paramTypes, &this->paramValues, this->paramLengths, this->paramFormats, 1 );
    //this->res = PQexecParams(this->conn, this->insertInto, 8, this->arrayOfOid, arrayOfparamValues, arrayOfParamLengths, arrayOfParamFormats, 1 );
    if (PQresultStatus(this->res) != PGRES_COMMAND_OK) { //INSERTS usually return PGRES_COMMAND_OK when they do a successful insert.\
        PGRES_COMMAND_OK is the successful indicator status for commands, like INSERT INTO, that reurn no data.
        std::cout << "Insert Operation failed: " << PQerrorMessage(this->conn);
        this->rc=1;
    } else {
        std::cout << "Successful insert!" << std::endl;
    }
    PQclear(this->res);
    PQfinish(this->conn);
    delete tfloat;
    delete tdouble;
    delete tshort;
    delete [] arrayOfparamValues;
    delete [] arrayOfParamLengths;
    delete [] arrayOfParamFormats;
    delete [] arrayOfOid;
    return this->rc;
}

