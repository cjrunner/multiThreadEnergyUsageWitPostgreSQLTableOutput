//
//  TblPolyFitWithCov.hpp
//  multiThreadEnergyUsageWithPostgreSQLTableOutput
//
//  Created by Clifford Campo on 6/26/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#ifndef TblPolyFitWithCov_hpp
#define TblPolyFitWithCov_hpp

#include <stdio.h>

#endif /* TblPolyFitWithCov_hpp */
//
//  tblPolyFit.hpp
//  multiThreadEnergyUsage
//
//  Created by Clifford Campo on 6/15/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#ifndef TblPolyFitWithCovhpp
#define TblPolyFitWithCovhpp

#include <stdio.h>
#include <iostream>
#include "/usr/local/pgsql/pgsql101/include/libpq-fe.h"
#include "setupForMultiFit.hpp"  //Establish Addressability to parent object
//#include "baseClass.hpp"
#include "myPrototypes.hpp"

#define NUMBEROFPQEXECPARAMS 24
#define FLOAT4ARRAYOID 1021
#define FLOAT8ARRAYOID 1022
#define FLOAT4OID 700
#define FLOAT8OID 701
#define TEXTOID 25
#define INT2OID 21
#define PD POLYNOMIALDEGREE //Polynomial Degree
#define NUMBEROFCOEFF 1 + PD
#define NUMBEROFCOVARIANCEMATRIXENTRIES  ( (NUMBEROFCOEFF) * (NUMBEROFCOEFF) )
#define NMBEROFSHORTS 2
#define NUMBEROFCHI2  1
#define NUMBEROFCC    1
#define  TOTALNUMBEROFPARAMETERS    ((NUMBEROFCOEFF) + (NUMBEROFCOVARIANCEMATRIXENTRIES) + (NMBEROFSHORTS) + (NUMBEROFCHI2) + (NUMBEROFCC))
#define  TEXTRESULTS 0    //Used to set resultFormat in public space
#define  BINARYRESULTS 1 //Used to set resultFormat in public space
//class SetupForMultiFit;  //doing a forward declaration of SetupForMultifit instead of:\
#include "/Users/cjc/c++/energyUsage/multiThreadEnergyUsageWithPostgreSQLTableOutput/multiThreadEnergyUsageWithPostgreSQLTableOutput/setupForMultiFit.hpp


class TblPolyFitWithCov : SetupForMultiFit { //Note that SetupForMultiFit gets called before TblPolyFit.
private:
    BaseClass *bc;
    const short pdegree=POLYNOMIALDEGREE;
    const char *connString = "dbname=LocalWeather port=5436 user=cjc";  //Connection String goes here.
    //   enum kinds {_m1m2kwh, _m1kwh, _m2kwh};
    // Should look something like: INSERT INTO tbl_poly_fit (lt,pd,coeff,kind,correl,chi2) VALUES(now(), 3, '{51.0091, 1.40023, -0.0656151, 0.000525268}', 'm1kwh', -0.945637, 4.8905);
    //                                                                  OID = FLOAT8OID (701) -----------------------------------+---+
    //                                                                  OID = INT2OID (21) ---------------------------------+    |   |
    //                                                                  OID = FLOAT8OID (701) --------------+---+---+---+   |    |   |
    //                                                                  OID = INT2OID  (21) ----------+     |   |   |   |   |    |   |
    //                                                                                                V     V   V   V   V   V    V   V
    //  const char *insertINTO = "INSERT INTO tbl_poly_fit (lt,pd,coeff,kind,correl,chi2) VALUES (now(), $1,  {$2, $3, $4, $5},$6,  $7, $8);";
    const char *insertINTO = "INSERT INTO tbl_poly_fit (lt,pd,coeff[1], coeff[2], coeff[3], coeff[4], kind,correl, chi2) VALUES (now(), $1,  $2, $3, $4, $5,  $6, $7, $8);";
    // NB: kind = 0 => m1m2kwh, kind = 1 => m1kwh, kind = 2 => m2kwh
    int  rc=0;  //Set the return code, assuming success;
    /*
     Column |           Type           |                             Modifiers
     --------+--------------------------+--------------------------------------------------------------------
     id     | integer                  | not null default nextval('tbl_poly_fit_with_cov_id_seq'::regclass)
     lt     | timestamp with time zone |
     pd     | smallint                 | not null
     coeff  | double precision[]       |
     cov    | real[]                   |
     kind   | smallint                 | 0 => m1m2kwh, 1 => m1kwh, 2 => m2kwh.
     correl | double precision         |
     chi2   | double precision         |
     Indexes:
     "tbl_poly_fit_with_cov_pkey" PRIMARY KEY, btree (id)
     */
    /*  The following worked from the psql command line.
     INSERT INTO  tbl_poly_fit_with_cov (lt, pd, coeff, cov, kind, correl, chi2) values(now(), 3, ARRAY[50.9847,1.40283,-0.0656843,0.000525766], ARRAY[[-0.000407403,3.8962e-05,-9.02632e-07,6.033e-09],[0.0640727,-0.00602311,0.000137125,-9.02632e-07], [-2.95741,0.270319,-0.00602311,3.8962e-05], [34.567,-2.95741,0.0640727,-0.000407403]], 1, 0.945595, 6.87839);
     
     
     */
    const char *insertIntoCov =  "INSERT INTO tbl_poly_fit_with_cov (lt,pd, coeff, cov,  kind, correl, chi2) VALUES(now(), $1, \
    ARRAY[$2,$3,$4,$5],  \
    ARRAY[[$6,$7,$8,$9], [$10, $11, $12, $13], [$14, $15, $16, $17], [$18, $19, $20, $21]], \
    $22, $23, $24) " ; //Arrays of doubles (aka, float8s) have an object id of 1022 per  /Users/cjc/C/postgres10/postgresql-10.1/postgresql-10.1/
public:
    double copyOfCoefficients[1+POLYNOMIALDEGREE];
    float  copyOfCovarianceMatrix[NUMBEROFCOVARIANCEMATRIXENTRIES];
    float intermediateValue[NUMBEROFCOVARIANCEMATRIXENTRIES];
    double correlationCoefficient;
    double chi2;
    union {
        float *ptrToCovarianceMatrix;
        float *ptrCov;  //Points to dynamically acquired covariance matrix. It is the job of this class's constructor to acquire the space that will hold this covarance matrix of type
    };
    float *ptrCovNbo; //Same as above but each element of array is in network byte order
    double *ptrCoeffNbo; //Points to array of Coefficients in NBO order ($2, $3, $4, $5) Object ID 1022
    short  nbop1; //contains the polynomial order, as a short int, in Network Byte Order
    double nbop2;    //contains the c0 coefficient, as a double, in Network Byte Order
    double nbop3;   //contains the c1 coefficient, as a double, in Network Byte Order
    double nbop4;   //contains the c2 coefficient, as a double, in Network Byte Order
    double nbop5;   //contains the c3 coefficient, as a double, in Network Byte Order
    short  nbop6;  //points to meter definition
    double nbop7;   //contains correlation coefficient, as a double, in Network Byte Order
    double nbop8;   //contains χ-squared value, as a double, in Network Byte Order
    const Oid *paramTypes; //Pointer to array of length = nParams and of type Oid (see /Users/cjc/C/postgres10/postgresql-10.1/postgresql-10.1/tmp_install/usr/local/pgsql/pgsql101/include/server/catalog/pg_type.h)
    const char  *paramValues; //Pointer to array of length = nParams
    const int *paramLengths; //Pointer to array of length = nParams. Specifies the actual data lengths of binary-format parameters. Ignored for NULL parameters and text-format parameters.
    const int *paramFormats; //Pointer to array of integers specifying if the value is text (array entry is 0), or binary (array entry is 1). Values passed in binary format require knowledge of the internal representation expected by the backend. For example, integers must be passed in network byte order (BIG ENDIAN). Passing numeric values requires knowledge of the server storage format, as implemented in `src/backend/utils/adt/:::numeric_send()` and `src/backend/utils/adt/numeric.c::numeric_recv()`
    PGconn *conn;
    PGresult *res;
    int intLoopCounter = 0;
    int nParams; // Number of Parameters. Also the lengths (number of elements) of the following four arrays. This value used by constructor for allocating the dynamic storage when instantiating the next four pointers.
    //   SetupForMultiFit *smf;
    int resultFormat; //When 0 then will obtain rsults in text format. When 1, then will obtain results in binary format.
    
    /*
     TblPolyFit(short=PD, int=TOTALNUMBEROFPARAMETERS, int=TEXTRESULTS, bool=true); //This constructor takes the third (right-most) parameter and uses it to set the value of nParams and then dynamically allocate storageTblPolyFit(SetupForMultiFit *,  int=TOTALNUMBEROFPARAMETERS, int=TEXTRESULTS); //This constructor takes the third (right-most) parameter and uses it to set the value of nParams and then dynamically allocate storage
     int doInsertInto(const char *, short *, double *, double *, double *, double *, short *, double *, double *);
     
     */
    TblPolyFitWithCov(BaseClass *, int=NUMBEROFPQEXECPARAMS, int=BINARYRESULTS );
    //         +-------- pointer to array containing the covariance matrix. Only valid if bool=true
    //         V
    TblPolyFitWithCov(short, int=TOTALNUMBEROFPARAMETERS, int=TEXTRESULTS, bool=true); //This constructor takes the third (right-most) parameter and uses it to set the value of nParams and then dynamically allocate storageTblPolyFit(SetupForMultiFit *,  int=TOTALNUMBEROFPARAMETERS, int=TEXTRESULTS); //This constructor takes the third (right-most) parameter and uses it to set the value of nParams and then dynamically allocate storage
    //                +-------- pointer to array containing the covariance matrix.
    //                V
    int doInsertInto(float *, short *, double *, double *, double *, double *, float *, short *, double *, double *);   //When doing insert into tbl_poly_fit_with_cov (WITH covariance matrix)
    int doInsertInto(short *, double *, double *, double *, double *, float *, short *, double *, double *);   //When doing insert into tbl_poly_fit (WITHOUT covariance matrix)
    int doInsertInto(const char *);
    
    ~TblPolyFitWithCov();  //Needs to delete the four dynamic storage objects  allocated by the above constructor.
    
    
    int doInsertIntoCov(void);
};
#endif /* TblPolyFitWithCovhpp */
