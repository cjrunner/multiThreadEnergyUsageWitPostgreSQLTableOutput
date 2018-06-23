//
//  tblPolyFit.hpp
//  multiThreadEnergyUsage
//
//  Created by Clifford Campo on 6/15/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#ifndef tblPolyFit_hpp
#define tblPolyFit_hpp

#include <stdio.h>
#include <iostream>
#include "/usr/local/pgsql/pgsql101/include/libpq-fe.h"
#include "baseClass.hpp"
#include "setupForMultiFit.hpp"
#define  NUMBEROFPARAMETERS 8
#define  TEXTRESULTS 0    //Used to set resultFormat in public space
#define  BINARYRESULTS 1 //Used to set resultFormat in public space
typedef struct ll64 {
    union {
        double d64;
        uint64_t b64;
        uint8_t b0;
        uint8_t  arr[8]; //An Array of eight, eight-bit, bytes.
    } u64;
} ll64;
class TblPolyFit : SetupForMultiFit {
private:
    const char *connString = "dbname=LocalWeather port=5436 user=cjc password=apple";  //Connection String goes here.
    enum kinds {'m1m2kwh', 'm1kwh', 'm2kwh'}
public:
    SetupForMultiFit *smf;
    // Should look something like: INSERT INTO tbl_poly_fit (lt,pd,coeff,kind,correl,chi2) VALUES(now(), 3, '{51.0091, 1.40023, -0.0656151, 0.000525268}', 'm1kwh', -0.945637, 4.8905);
    //                                                               OID = FLOAT8OID (701) --------------------------------------+---+
    //                                                               OID = TEXTOID (25) ------------------------------------+    |   |
    //                                                               OID = FLOAT8OID (701) --------------+---+---+---+      |    |   |
    //                                                               OID = INT2OID  (21) ----------+     |   |   |   |      |    |   |
    //                                                                                             V     V   V   V   V      V    V   V
    const char *insertINTO = "INSERT INTO tbl_poly_fit (lt,pd,coeff,kind,correl,chi2) VALUES (now(), $1, '{$2, $3, $4, $5}', '$6', $7, $8);" //Where $1 … $4 are coefficients C0 … C3; \
    $5 is either m1kwh, or m2kwh, or m1m2kwh; $6 is the correlation coefficient; $7 is the χ-squared value.
    PGconn *conn;
    PGresult *res;
    int    nParams; // Number of Parameters. Also the lengths (number of elements) of the following four arrays. This value used by constructor for allocating the dynamic storage when instantiating the next four pointers.
    short  nbop1,
    double nbop2;
    double nbop3;
    double nbop4;
    double nbop5;
    char   *nbop6;
    double nbop7;
    double nbop8;
    
    const Oid *paramTypes; //Pointer to array of length = nParams and of type Oid (see /Users/cjc/C/postgres10/postgresql-10.1/postgresql-10.1/tmp_install/usr/local/pgsql/pgsql101/include/server/catalog/pg_type.h)
    const char * const *parmValues; //Pointer to array of length = nParams
    const int *paramLengths; //Pointer to array of length = nParams. Specifies the actual data lengths of binary-format parameters. Ignored for NULL parameters and text-format parameters.
    const int *paramFormats; //Pointer to array of integers specifying if the value is text (array entry is 0), or binary (array entry is 1). Values passed in binary format require knowledge of the internal representation expected by the backend. For example, integers must be passed in network byte order (BIG ENDIAN). Passing numeric values requires knowledge of the server storage format, as implemented in `src/backend/utils/adt/:::numeric_send()` and `src/backend/utils/adt/numeric.c::numeric_recv()`
    int  resultFormat; //Specify 0 to obtain results in text format; specify 1 to obtain results in binary format.
    TblPolyFit(SetupForMultiFit *, const char *="dbname=LocalWeather port=5436 user=cjc password=apple", int=NUMBEROFPARAMETERS, int=TEXTRESULTS); //This constructor takes the third (right-most) parameter and uses it to set the value of nParams and then dynamically allocate storage
    ~TblPolyFit();  //Needs to delete the four dynamic storage objects  allocated by the above constructor.
    int doInsertInto(const char *, );
};
#endif /* tblPolyFit_hpp */
