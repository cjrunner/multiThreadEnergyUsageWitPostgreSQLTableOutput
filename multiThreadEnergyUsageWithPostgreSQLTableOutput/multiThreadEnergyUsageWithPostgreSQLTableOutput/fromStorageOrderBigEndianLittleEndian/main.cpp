//
//  main.cpp
//  storageOrderBigEndianLittleEndian
//
//  Created by Clifford Campo on 6/15/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//
/* T E S T            D A T A            L O O K S            L I K E            T H I S
 193 2018-06-15T22:22:00 EDT Line 160 of File /Users/cjc/c++/c++Postgres/c++Postgres/main.cpp, compiled on Tue Jun 12 07:44:11 2018
 194 Beginning execution of program:
 195 /Users/cjc/Library/Developer/Xcode/DerivedData/Build/Products/Debug/multiThreadEnergyUsage
 196 First to Join: Time to execute the M1M2KWHCASE 1591082 µsec.
 197 2018-06-15T22:22:01 EDT EnergyUsage(T) = 53.0739 + 1.34349*T^1 + -0.0663476*T^2 + 0.000558023*T^3   Case m1m2kwh
 198 2018-06-15T22:22:01 EDT cov = [38.5243, -3.41614, 0.078887, -0.000530546]       Case m1m2kwh
 199 2018-06-15T22:22:01 EDT cov = [-3.41614, 0.322409, -0.00767952, 5.26829e-05]        Case m1m2kwh
 200 2018-06-15T22:22:01 EDT cov = [0.078887, -0.00767952, 0.000187953, -1.31794e-06]        Case m1m2kwh
 201 2018-06-15T22:22:01 EDT cov = [-0.000530546, 5.26829e-05, -1.31794e-06, 9.42607e-09]        Case m1m2kwh
 202 4.12928☜ χ-squared  Case m1m2kwh
 203 38.8469☜ Trace of Covariance Matrix Case m1m2kwh
 204 -0.893097☜ correlation coefficient  Case m1m2kwh
 205 11.6146☜ Square Root Of Sum Of TheSquares Of The Varience;
 206 0.00940454☜ Average Varience Per Valid Reading
 207 0.940454☜ % Average Varience Per Valid Reading
 208 1235☜ Number of valid meter readings (# observed dependent variables)   Case m1m2kwh
 
 258 Second to Join: Time to execute the M2KWHCASE 1709932 µsec.
 259 2018-06-16T22:22:02 EDT EnergyUsage(T) = 1.31837 + 0.00649231*T^1 + -0.00181809*T^2 + 3.79257e-05*T^3   Case m2kwh
 260 2018-06-16T22:22:02 EDT cov = [1.33831, -0.130199, 0.00344484, -2.6128e-05]     Case m2kwh
 261 2018-06-16T22:22:02 EDT cov = [-0.130199, 0.0134453, -0.000370019, 2.88963e-06]     Case m2kwh
 262 2018-06-16T22:22:02 EDT cov = [0.00344484, -0.000370019, 1.0604e-05, -8.58803e-08]      Case m2kwh
 263 2018-06-16T22:22:02 EDT cov = [-2.6128e-05, 2.88963e-06, -8.58803e-08, 7.24148e-10]     Case m2kwh
 264 3.77747☜ χ-squared  Case m2kwh
 265 1.35177☜ Trace of Covariance Matrix Case m2kwh
 266 0.865064☜ correlation coefficient   Case m2kwh
 267 36.9541☜ Square Root Of Sum Of TheSquares Of The Varience;
 268 0.0298981☜ Average Varience Per Valid Reading
 269 2.98981☜ % Average Varience Per Valid Reading
 270 1236☜ Number of valid meter readings (# observed dependent variables)   Case m2kwh
 
 272 Last to Join: Time to execute the M1KWHCASE 1709922 µsec.
 273 2018-06-16T22:22:02 EDT EnergyUsage(T) = 50.9907 + 1.4022*T^1 + -0.0656676*T^2 + 0.000525647*T^3    Case m1kwh
 274 2018-06-16T22:22:02 EDT cov = [34.579, -2.95868, 0.0641061, -0.000407643]       Case m1kwh
 275 2018-06-16T22:22:02 EDT cov = [-2.95868, 0.270453, -0.00602666, 3.89874e-05]        Case m1kwh
 276 2018-06-16T22:22:02 EDT cov = [0.0641061, -0.00602666, 0.000137219, -9.03305e-07]       Case m1kwh
 277 2018-06-16T22:22:02 EDT cov = [-0.000407643, 3.89874e-05, -9.03305e-07, 6.03784e-09]        Case m1kwh
 278 4.87812☜ χ-squared  Case m1kwh
 279 34.8496☜ Trace of Covariance Matrix Case m1kwh
 280 -0.945606☜ correlation coefficient  Case m1kwh
 281 10.2274☜ Square Root Of Sum Of TheSquares Of The Varience;
 282 0.00827456☜ Average Varience Per Valid Reading
 283 0.827456☜ % Average Varience Per Valid Reading
 284 1236☜ Number of valid meter readings (# observed dependent variables)   Case m1kwh
T E S T            D A T A      */
#include <iostream>
#include <iomanip>
#include <math.h>
#include "varyingType.hpp"
#include "tblPolyFit.hpp"
#define MYTYPE double
int main(int argc, const char **argv) {
//    const char *cs="dbname=LocalWeather port=5436 user=cjc password=apple";
    int rc=0;
    short po = atoi(*(argv +8) );    // 3;
    double c0 = atof(*(argv + 1)) ;  // 50.9907;
    double c1 = atof(*(argv + 2)) ;  //1.4022;
    double c2 = atof(*(argv + 3)) ;  //-0.0656676;
    double c3 = atof(*(argv + 4)) ;  //0.000525647;
    short kind = atoi(*(argv + 5)) ; //1; //kind=0 => m1m2kwh; kind=1 => m1kwh; kind=2 => m2kwh;
    double cc = atof(*(argv + 6)) ;  //-0.945606;
    double chi2 = atof(*(argv + 7)); // 4.87812;
    float array[NUMBEROFCOVARIANCEMATRIXENTRIES];
#ifdef DODEBUG
    std::cout << "DODEBUG FLAG is set, must be in debug mode. " << std::endl;
#endif
#ifdef DOTABLETBLPOLYFITWITHCOV
    const char *const whichTable = "tbl_poly_fit_with_cov";
    std::cout << "We will be working with table " << whichTable  << ". There are " << NUMBEROFCOVARIANCEMATRIXENTRIES << " entries in the covariance matrix. "  << std::endl;
    TblPolyFit *ptrTPF = new TblPolyFit(3, 8+NUMBEROFCOVARIANCEMATRIXENTRIES , 0 );
#else
    const char *const whichTable = "tbl_poly_fit";
    std::cout << "We will be working with table " <<  whichTable << " which ***does not*** accomodate the covariance matrix." << std::endl;
    TblPolyFit *ptrTPF = new TblPolyFit(3, 8 , 0 );
#endif
//    const char *connString = "dbname=LocalWeather port=5436 user=cjc password=apple";

    // insert code here...
    //long doubles are 16 bytes long
    //doubles are 8 bytes long
    //floats are 4 bytes long
    //shorts are 2 bytes long


//int doInsertInto(const char *, int *, double *, double *, double *, double *, smallint *, double *, double *)
//χ-squared value ------------------------------------------------------------------+
//Correlation Coefficient -----------------------------------------------------+    |
//Meter Descriptor --kind=0=>m1m2kwh; kind=1=>m1kwh; kind=2=>m2kwh---------+   |    |
//Ptr to Covariance Matrix ------------------------------------------+     |   |    |
//coefficient C3 ----------------------------------------------+     |     |   |    |
//coefficient C2 -----------------------------------------+    |     |     |   |    |
//coefficient C1 -------------------------------------+   |    |     |     |   |    |
//coefficient C0 ---------------------------------+   |   |    |     |     |   |    |
//short int showing polynomial degree -------+    |   |   |    |     |     |   |    |
//Ptr to SetupForMultiFit, NULL(test mode+   |    |   |   |    |     |     |   |    |
    //                                    V   V    V   V   V    V     V     V   V    V
rc=ptrTPF->TblPolyFit::doInsertInto(NULL, &po, &c0, &c1, &c2, &c3, &*array, &kind, &cc, &chi2);

    if(rc) {
        std::cout << "Some kind of problem in TblPolyFit::doInsertInto, got a return code of " << rc << std::endl;
    } else {
        std::cout << "Successful call to TblPolyFit::doInsertInto, got a return code of " << rc << std::endl;
    }
    delete ptrTPF;
    return 0;
}
//int doInsertIntoCov(SetupForMultiFit *, short *, double *, double *, double *, double *, float *, short *, double *, double *);


