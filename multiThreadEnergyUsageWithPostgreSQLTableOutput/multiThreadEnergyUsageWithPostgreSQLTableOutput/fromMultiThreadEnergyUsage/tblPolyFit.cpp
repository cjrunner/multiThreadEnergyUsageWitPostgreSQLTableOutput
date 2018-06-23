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
#include "baseClass.hpp"
#include "setupForMultiFit.hpp"
#include "tblPolyFit.hpp"
#include "varyingType.hpp"
TblPolyFit::TblPolyFit(SetupForMultiFit *smf, const char *cs, int nv, int rsltFmt) {
    this->smf = smf;
    connString = cs;
    nParams = nv;
    parmValues = new const char*[nv];
    paramTypes = new const Oid*[nv];
    paramLengths = new const int *[nv];
    paramFormats = new const int *[nv];
}
TblPolyFit::~TblPolyFit() {
    delete [] parmValues;
    delete [] paramTypes;
    delete [] paramLengths;
    delete [] paramFormats;
    while 
}

//χ-squared value ---------------------------------------------------------------------------------------------------+
//Correlation Coefficient --------------------------------------------------------------------------------+          |
 //Meter Descriptor ----------------------------------------------------------------------------+         |          |
 //coefficient C3 -------------------------------------------------------------------+          |         |          |
 //coefficient C2 --------------------------------------------------------+          |          |         |          |
 //coefficient C1 ---------------------------------------------+          |          |          |         |          |
 //coefficient C0 -----------------------------------+         |          |          |          |         |          |
 //short int showing polynomial degree ---+          |         |          |          |          |         |          |
 //    Base Class ----------+             |          |         |          |          |          |         |          |
//                          V             V          V         V          V          V          V         V          V
int TblPolyFit::doInsertInto(BaseClass *bc, int *p1, double *p2, double *p3, double *p4, double *p5, char *p6, double *p7, double *p8) {
    VaryingType<short> *tp1 =new VaryingType();
    tp1->in64.d64 = *p1;
    tp1->VaryingType<short>::toNetworkByteOrder();
    this->nbop1 = tp1->out64.d64;
    VaryingType<double> *tp2_8 =new VaryingType();
    tp2_8->in64.d64 = *p2;
    tp2_8->VaryingType<double>::toNetworkByteOrder();
    this->nbop2 = tp2_8->out64.d64;
// --------------------------------------------
    tp2_8->in64.d64 = *p3;
    tp2_8->VaryingType<double>::toNetworkByteOrder();
    this->nbop3 = tp2_8->out64.d64;
// --------------------------------------------
    tp2_8->in64.d64 = *p4;
    tp2_8->VaryingType<double>::toNetworkByteOrder();
    this->nbop4 = tp2_8->out64.d64;
// --------------------------------------------
    tp2_8->in64.d64 = *p5;
    tp2_8->VaryingType<double>::toNetworkByteOrder();
    this->nbop5 = tp2_8->out64.d64;
// --------------------------------------------
    tp2_8->in64.d64 = *p7;
    tp2_8->VaryingType<double>::toNetworkByteOrder();
    this->nbop7 = tp2_8->out64.d64;
// --------------------------------------------
    tp2_8->in64.d64 = *p8;
    tp2_8->VaryingType<double>::toNetworkByteOrder();
    this->nbop8 = tp2_8->out64.d64;
// --------------------------------------------
    Oid *arrayOfOid = new Oid[8] = {INT2OID, FLOAT8OID, FLOAT8OID, FLOAT8OID, FLOAT8OID, TEXTOID, FLOAT8OID, FLOAT8OID};
    this->paramTypes = arrayOfOid;
    const char * const *arrayOfParmValues = new const char * const *[8] = {this->nbop1, this->nbop2, this->nbop3, this->nbop4, this->nbop5, this->nbop6, this->nbop7, this->nbop8};
    this->paramValues = arrayOfParmValues;
    const int *arrayOfParamLengths = new const int *[8] = {sizeof(short), sizeof(double), sizeof(double), sizeof(double), sizeof(double),NULL, sizeof(double), sizeof(double)};
    this->paramLengths = arrayOfParamLengths;
    const int *arrayOfParamFormats = new const int *[8] = {1, 1, 1, 1, 1, 0, 1, 1};
    this->paramFormats = arrayOfParamFormats;
}
void blPolyFit::toNetworkByteOrder(ll64 *in, ll64 *out) {
    for (int i=0; i<=7; i++) {
        out->u64.arr[7-i] = in->u64.arr[i]; //This works!
    }
}
