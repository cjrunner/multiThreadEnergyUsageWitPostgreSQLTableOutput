//
//  julianToGregorian.cpp
//  energyUsage
//
//  Created by Clifford Campo on 5/4/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#include "julianToGregorian.hpp"
#ifndef    __ISITDAYLIGHTSAVINGSTIME__
#define __ISITDAYLIGHTSAVINGSTIME__ 1
#include    "isItDayLightSavingsTime.h"
#endif
int    JulianToGregorian(register gregorian    *ptrGregorian) {
    ptrGregorian->Frac=ptrGregorian->J-(int)ptrGregorian->J;  //Separate the fractional part of the julian day
    //int    J, p, q, r, s, t, u, v, Y, M, D;    //These are the work variables from the example, below.
    ptrGregorian->p = ptrGregorian->J + 68569;
    ptrGregorian->q = 4*ptrGregorian->p/146097;
    ptrGregorian->r = ptrGregorian->p - (146097*ptrGregorian->q + 3)/4;
    ptrGregorian->s = 4000*(ptrGregorian->r+1)/1461001;
    ptrGregorian->t = ptrGregorian->r - 1461*ptrGregorian->s/4 + 31;
    ptrGregorian->u = 80*ptrGregorian->t/2447;
    ptrGregorian->v = ptrGregorian->u/11  ;
    
    ptrGregorian->Y = 100*(ptrGregorian->q-49)+ptrGregorian->s+ptrGregorian->v;    //Extract Years
    ptrGregorian->M = ptrGregorian->u + 2 - 12*ptrGregorian->v;            //Extract Months
    ptrGregorian->D = ptrGregorian->t - 2447*ptrGregorian->u/80;            //Extract Days
    ptrGregorian->hour = (int)(ptrGregorian->Frac*24.0);                // Get the hours
    ptrGregorian->work = ptrGregorian->Frac*24.0-(int)(ptrGregorian->Frac*24.0);      //Get what remains after we extract the hours
    ptrGregorian->minute = (int)(ptrGregorian->work*60.0);                //Extract the Minutes
    ptrGregorian->second = (60*(ptrGregorian->work*60.0 - (int)(ptrGregorian->work*60.0))); //Extract the Seconds & subseconds
    return(0);
}


/*  ----------------------------------------------------------------------
 The algorithm for converting from
 a Julian date to a Gregorian date is the following.  Let J be the
 Julian day number (since this algorithm uses integer arithmetic,
 this will be the Julian day number which begins at noon on the
 calendar day), and let Y, M, and D denote the corresponding calendar
 date.
 
 Then compute the following quantities, using integer arithmetic:
 
 p = J + 68569
 q = 4*p/146097
 r = p - (146097*q + 3)/4
 s = 4000*(r+1)/1461001
 t = r - 1461*s/4 + 31
 u = 80*t/2447
 v = u/11  .
 
 Then
 Y = 100*(q-49)+s+v
 M = u + 2 - 12*v
 D = t - 2447*u/80 .
 
 For example, using the data you supplied,
 
 J = 2451964
 p = 2520533
 q = 69
 r = 359
 s = 0
 t = 390
 u = 12
 v = 1
 
 D = 23
 M = 2
 Y = 2001  .
 
 It appears that you gave the integer part of the Julian date of
 midnight for Feb. 24, 2001, which would be 2451964.5. This algorithm
 gives the calendar date on which the Julian date 2451964 begins, which
 is at noon on Feb. 23, 2001.
 
 Just for completeness, although you didn't ask for it, here is the
 Fliegel-Van Flandern algorithm to convert from a Gregorian calendar
 date to the Julian date:
 
 Given Y,M,D, the year, month, and date, define (using integer
 arithmetic)
 
 M1 = (M-14)/12
 Y1 = Y + 4800 .
 
 Then the Julian date J is
 
 J = 1461*(Y1+M1)/4 + 367*(M-2-12*M1)/12 - (3*((Y1+M1+100)/100))/4
 + D - 32075  .
 
 For example, for Y = 2001, M = 2, and D = 24,
 
 M1 = -1
 Y1 = 6801 ,
 
 and
 
 J = 2483700 + 367 - 51 + 24 -32075
 
 = 2451965 .
 
 These algorithms are available in the Explanatory Supplement to the
 Astronomical Almanac, 2nd edition, P. Seidelmann (ed.), or in
 Fliegel and Van Flandern's original paper,
 
 Fliegel, H. F., and Van Flandern, T. C., "A Machine Algorithm for
 Processing Calendar Dates," Communications of the Association of
 Computing Machines, vol. 11 (1968), p. 657.
 
 If you have any questions, please write back and I will try to
 explain further.
 
 - Doctor Fenton, The Math Forum
 http://mathforum.org/dr.math/
 --------------------------------------------------------------------------- */
