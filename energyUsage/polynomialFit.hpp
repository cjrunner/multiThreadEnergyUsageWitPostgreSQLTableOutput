//
//  polynomialFit.hpp
//  energyUsage
//
//  Created by Clifford Campo on 5/4/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

// #ifndef __POLYNOMIALFIT__
// #define __POLYNOMIALFIT__

#include <iostream>
#include <cstdlib>
#include <cstdbool>
//#include "/usr/local/include/gsl/gsl_multifit.h"
//#include "/usr/local/include/gsl/gsl_blas.h"
//#include "/usr/local/include/gsl/gsl_statistics_double.h"
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_statistics_double.h>
#define ISO8601LEN 20
/*
int     polynomialFit(int polynomialdegree, int reccntr, double *XVALin, double *YVALin,  double *returnJD, double *returnZD, double *TSS, double *goodnessOfFit);
*/
int     polynomialFit(int, int, double *, double *,  double *, double *, double *, double *);
// #endif // __POLYNOMIALFIT__
