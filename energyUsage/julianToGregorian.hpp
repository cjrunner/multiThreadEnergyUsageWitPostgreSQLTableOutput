//
//  julianToGregorian.hpp
//  energyUsage
//
//  Created by Clifford Campo on 5/4/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#ifndef __JULIANTOGREGORIAN__
#define __JULIANTOGREGORIAN__
#include <iostream>
typedef struct {
    char Y[4];
    char sep1[1];
    char M[2];
    char sep2[1];
    char D[2];
    char sep3[1];
    char H[2];
    char sep4[1];
    char m[2];
    char sep5[1];
    char S[2];
    char sep6[1];
    char ss[3];
}cb;
class JulianToGregorian {

//int    J, p, q, r, s, t, u, v, Y, M, D;    //These are the work variables from the example, below.
private:
    int    intJ;
    int    p;
    int    q;
    int    r;
    int    s;
    int    t;
    int    u;
    int    v;
    int    Y;
    int    M;
    int    D;
    int    hour;
    int    minute;
    int    seconds;
    double work;
    double Frac;
    double Jcopy;

    char   ResultBuffer[sizeof(cb)];
public:
    char *ptrResultingGregorian;
    ~JulianToGregorian();
    JulianToGregorian(double, char=' ');

};  //Ebn of class
JulianToGregorian::~JulianToGregorian () {
    std::cout << "Entered the destructor for the JulianToGregorian class whose instance is at address %p" << this << std::endl;
}
JulianToGregorian::JulianToGregorian(double J, char separator) {
    this->ptrResultingGregorian = this->ResultBuffer;
    this->Frac = J - (int)J;  //Separate the fractional part of the julian day
    this->Jcopy = J;
    this->intJ   = (int)J;
    this->p = this->intJ + 68569;
    this->q = 4 * this->p/146097;
    this->r = this->p - (146097 * this->q + 3)/4;
    this->s = 4000*(this->r + 1)/1461001;
    this->t = this->r - 1461*this->s/4 + 31;
    this->u = 80 * this->t/2447;
    this->v = this->u/11;
    
    this->Y = 100*(this->q-49)+this->s+this->v;    //Extract Years
    this->M = this->u + 2 - 12*this->v;            //Extract Months
    this->D = this->t - 2447*this->u/80;            //Extract Days
    this->hour = (int)(this->Frac*24.0);                // Get the hours
    this->work = this->Frac*24.0-(int)(this->Frac*24.0);      //Get what remains after we extract the hours
    this->minute = (int)(this->work*60.0);                //Extract the Minutes
    this->seconds = (60*(this->work*60.0 - (int)(this->work*60.0))); //Extract the Seconds & subseconds
    sprintf(this->ResultBuffer, "%04d-%02d-%02d %02d:%02d:%02d", this->Y, this->M, this->D, this->hour,  this->minute, this->seconds );
}
#endif /* julianToGregorian_hpp */
