//
//  polynomialFit.cpp
//  energyUsage
//
//  Created by Clifford Campo on 5/4/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//  Based upon /Users/cjc/C/baselineSource/polynomialFit-r1.c, but updated to make this more c++ like

#ifndef __POLYNOMIALFIT__
#define __POLYNOMIALFIT__
#include "polynomialFit.hpp" 
#endif
/*
#include "julianToGregorian.hpp"
*/
void polySet(gsl_matrix *value, double independentVariable, const int rowNumber, const int polynomialdegree, int columnNumber = 0);   //Internal function
int     findPolynomial(int, int , bool, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *);  //Internal function.
int     polynomialFit(int polynomialdegree, int reccntr, double *XVALin, double *YVALin,  double *returnJD, double *returnZD,  double *TSS, double *goodnessOfFit)  { // <==== New Way of Calling what used to be a test program
    bool    dofrac=false; // Get more accurate results if we do our computations without the integer part because more slots in floating point registers allocated to fractional part.
    double  dopct=0.02; // Specify 2% uncertainty as the default
    int     integerPartOfDay;
    double  *XVAL, *YVAL, *fracXVAL, *pcnt2val, *oW, *cC, *oCov;    /* declare our pointers to buffer areas */
    double  julian, yval;
    double  xMakesYmin; //Allocate space for these 1-dimensional arrays at compile time, rather than at run time, in which case we would have to do a malloc/calloc.
    //double  oCov[POLYNOMIALDEGREE+1][POLYNOMIALDEGREE+1];                //Allocate space for this 2-dimensional array at compile time, rather than at run time, in which case we would have to do a malloc/calloc.
    oW   = (double *)calloc((1+polynomialdegree),sizeof(double));
    cC   = (double *)calloc((1+polynomialdegree),sizeof(double));                        //
    oCov = (double *)calloc((1+polynomialdegree)*(1+polynomialdegree), sizeof(double));  // 2-dimensional array for holding the covariance matrix
    char    gregorian[ISO8601LEN];
    int    i=0, j, k=0, rc=0;
    j=0;
    k=k-k;
    XVAL    = (double *)calloc(reccntr, sizeof(double));
    YVAL    = (double *)calloc(reccntr, sizeof(double));
    fracXVAL= (double *)calloc(reccntr, sizeof(double));
    pcnt2val= (double *)calloc(reccntr, sizeof(double));
    for(i=0; i<reccntr; i++) {
        /* inrec should = 4 for each iteration */
        XVAL[i] = XVALin[i];
        YVAL[i] = YVALin[i];
  //      fracXVAL[i]= FPXVAL[i];
        fracXVAL[i] = XVALin[i] - (int)(size_t)XVALin[i];  //Get the just the fractional part of the independent variable, XVALin[i]
        pcnt2val[i] = YVAL[i] * dopct; //If pcnt error is specified in command line then use it.
        printf("%02d. XVAL: %lf;\t YVAL: %lf;\t fracXVAL: %lf;\t pcnt2val: %lf\n",i, XVAL[i], YVAL[i], fracXVAL[i], pcnt2val[i]);
    }
    integerPartOfDay = (int)(XVAL[reccntr/2]); //Get the integer part of the x-value in the middle of the array
    rc    = findPolynomial( polynomialdegree, reccntr, dofrac, XVAL, YVAL, fracXVAL, pcnt2val, oW, cC, oCov, &xMakesYmin, TSS, goodnessOfFit);
    julian    = (double)integerPartOfDay+xMakesYmin;  //Now combine the integer part of the day with the fractional part of x creating the time-of-day, expressed in local julian day format, when noon occurs.
    yval    = cC[0] + cC[1]*xMakesYmin + cC[2]*xMakesYmin*xMakesYmin;
    if(polynomialdegree == 3) {
        yval = yval + cC[3]*xMakesYmin*xMakesYmin*xMakesYmin;
    }
//    JulianToGregorian *jtg =new JulianToGregorian(julian); 
    /*
    rc    =julianToGregorian (&julian, false, gregorian); //OLD, c-style way of doing this function.
     */
    // Return to caller when noon occurs formatted as an ISO8601 character string.


    printf("Noon occurs on %16.8lf (%s) on date of interest, noon zenithdistance = %16.8lf\n", julian, gregorian, yval);
    free(oCov);
    free(oW);
    free(cC);
    free(pcnt2val);
    free(fracXVAL);
    free(YVAL);
    free(XVAL);
    return (rc);
}

int    findPolynomial(int polynomialdegree, int nmbr, bool frac, double *XV, double * YV, double *fracXV, double *pcnt2, double *ow, double *oc, double *ocov, double *minx, double *tss, double *goodnessOfFit) {
    int    i, n;
    double  *xi, *yi, *ei, chisq, xiSquared, sigmaSquared, localMinX, xWork, sqrWork, TSS, R2;
    gsl_matrix    *independentVariableMatrix, *cov;
    gsl_vector    *dependentVariableVector, *w, *polynomialCoefficientsVector;
    n=nmbr;    /* Get the number of expected input independentVariableMatrix, Y, and stddev values. */
    independentVariableMatrix = gsl_matrix_alloc(n,polynomialdegree+1);    //Allocate workspace for the X data values.
    dependentVariableVector = gsl_vector_alloc(n);            //Allocate workspace for the y data values. There is one y data value for each of the above independentVariableMatrix data value.
    w = gsl_vector_alloc(n);            //Allocate workspace for the standard deviation values. There is one standard deviation value associated with each y data value
    polynomialCoefficientsVector = gsl_vector_alloc(polynomialdegree+1);           // Allocate workspace for computing the polynomial coefficients; the greater the degree of your polynomial, the more coefficients you will have
    cov=gsl_matrix_alloc(polynomialdegree+1, polynomialdegree+1);   //Allocate workspace for computing the covariance matrix.
    xi = XV;

    if(frac) {
        xi = fracXV;    //fracXV is an array containing just the fractional part (hours, minutes and seconds) of the Julian Date of interest.
    } else {
        xi = XV;   //XV is an array containing the integer (days) and fractional (hours, minutes, and seconds) parts of the Julian Date.
    }
    xiSquared = *xi; //get value of xi
    xiSquared = xiSquared*xiSquared; // Now square value of xi
    
    yi = YV;
    ei = pcnt2;
    sigmaSquared = *ei;  //Get the uncertainty value
    sigmaSquared = sigmaSquared*sigmaSquared;
    for(size_t rowNumber = 0; rowNumber < n; rowNumber++) { //Start of loop operation.
        xWork = xi[rowNumber];
//   total Number of columns -----------------------------+
//   Matricies' row number we're working on --+           |
//   Independendt Variable -----------+       |           |
//   Pointer to matrix+               |       |           |
//                    V               V       V           V
        polySet(independentVariableMatrix, xWork, rowNumber, polynomialdegree);
//        gsl_matrix_set(independentVariableMatrix, loopCounter, 0, 1.0);    //prepare for the zeroth coefficient, the coefficient associated with the x^0 term of the polynomial. Note, I believe the 1.0 is the same as x^0.
//        gsl_matrix_set(independentVariableMatrix, loopCounter, 1,  xWork);    //prepare for the first coefficient, the coefficient associated with x^1 term of the polynomial.
//        gsl_matrix_set(independentVariableMatrix, loopCounter, 2, (xWork*xWork)); //prepare for the second coefficient, the coefficient associated with the x^2 term.
//        if(polynomialdegree==3) {// Are we working with a cubic equation?
//            gsl_matrix_set(independentVariableMatrix, loopCounter, 3, (xWork*xWork*xWork)); // If we had wanted to fit our data to a cubic equation it would look like this. Later on we may try to fit our data to a cubic equation.
//       }
        gsl_vector_set(dependentVariableVector, rowNumber, YV[rowNumber]); // set our vector to the y-values of our input data
        gsl_vector_set(w, rowNumber, (1.0/(pcnt2[ rowNumber] * pcnt2[rowNumber]) ) ); /* set the weight vector, w, to one-over-sigma-squared */
    } //End of loop operation.
    {
        gsl_multifit_linear_workspace *work = gsl_multifit_linear_alloc(n, polynomialdegree+1);
        //    gsl_multifit_wlinear(independentVariableMatrix, w, y, c, cov, &chisq, work); /* do our curve fitting here */
        gsl_multifit_linear(independentVariableMatrix,  dependentVariableVector, polynomialCoefficientsVector, cov, &chisq, work); /* do our curve fitting here */
        TSS = gsl_stats_tss(yi, 1, n);    //Compute total sum of squares.
        R2 = (1-chisq)/TSS;  //Compute R2, or R-Squared, or Goodness-of-Fit
        if (tss != NULL) {
            *tss = TSS;
        }
        if (goodnessOfFit != NULL) {
            *goodnessOfFit = R2;
        }
        gsl_multifit_linear_free(work);
    }
/*
#define C(i) (gsl_vector_get(c, (i)))
#define COV(i,j) (gsl_matrix_get(cov, (i), (j)))
*/
    //{
    if(polynomialdegree==2) {
        printf("# best fit quadratic: Y = %g +%g X + %g X^2\n", *(polynomialCoefficientsVector+0), *(polynomialCoefficientsVector+1), *(polynomialCoefficientsVector+2) );
    } else {
        printf("# best fit cubic: Y = %g +%g X + %g X^2 + %g X^3\n", *(polynomialCoefficientsVector+0), *(polynomialCoefficientsVector+1), *(polynomialCoefficientsVector+2), *(polynomialCoefficientsVector+3) );
    }
    printf("## total sum squared of y values: %f;\t R-squared of fit: %f \n", TSS, R2);
    printf("\n[\n");
    for(int row = 0; row <= polynomialdegree; row++) {
        printf("[");
        for (int col = 0; col <= polynomialdegree; col++) {
            /* NOTE: ocov[rowNumber,colNumber] is equivalent to: *(&ocov[0][0] + ((MaximumValueOf_colNumber) * rowNumber) + colNumber)  */
            printf("%+.5e  ", gsl_matrix_get(cov, row, col) );
        }
        printf("]\n");
    }
    printf("]\n");
    printf("# chisq = %g\n", chisq);
    // }
    /*
    if(polynomialdegree==2) {
        localMinX  = (-C(1)/(2*C(2)));
    } else {
        sqrWork = sqrt(2*2*C(2)*C(2) - 4*C(1)*3*C(3));
        localMinX = ((-2*C(2) - sqrWork)/(2*3*C(3)));  // Try first of two possible answers
        if (localMinX < 0 ||  localMinX >= 1) { // Two possible answers with a cubic equation because the derivative of a cubic is a quadratic equation whic, according to good 'ol quadratic formula has two possible answers!
            localMinX = ((-2*C(2) + sqrWork)/(2*3*C(3)));
        }
    }
    
    *minx = localMinX;
    printf("The value of x which makes y minimum is %lf.\n", localMinX);
    oc[0] = C(0);
    oc[1] = C(1);
    oc[2] = C(2);
    if(polynomialdegree==3) {
        oc[3] = C(3);
    }
    */
    gsl_matrix_free(independentVariableMatrix);
    gsl_vector_free(dependentVariableVector);
    gsl_vector_free(w);
    gsl_vector_free(polynomialCoefficientsVector);
    gsl_matrix_free(cov);
    return 0;
}

void polySet(gsl_matrix *value, double independentVariable, const int rowValue, const int polynomialdegree, int columnValue) {
//Upon first entering this recursive function, polySet, thisRecursionValue should be set to zero.
//We should recurse on this function until thisRecursionValue equals the polynomialdegree parameter.
//Note that value points to the beginning of the gsl_matrix.
    double previousColumnValue;
    double thisColumnValue;
    value->data[rowValue + 0] = 0;
    value->data[rowValue + 1] = independentVariable;
    columnValue=2; //Let's work on the next column.
    while (columnValue > 1 && columnValue < 1 + polynomialdegree) {
// See § 8.4 of Chapter Chapter 8, titled Vectors and Matrices, of GNU Scientific Library Reference Manual, third edition,
// pgs 101-103, version 1.12, for discussion of how to hop-scotch around these matrices.
        value->data[rowValue + columnValue] = value->data[rowValue + 1] * value->data[rowValue + columnValue-1];
        previousColumnValue = value->data[rowValue + columnValue - 1]; //Pick up the value found in the column to the left
        thisColumnValue     = value->data[rowValue + columnValue + 0];
        ++columnValue; //Let's work on the next column.
    }
    // -- Value being placed in matrix --------------------+
    //  Column Where We Put Value --------+                |
    // -Row Where We Put Value+           |                |
    // -Start of Matrix +     |           |                |
    //                  V     V           V                V
    //  gsl_matrix_set(value, rowValue, columnValue,  independentVariable );
    

    }

