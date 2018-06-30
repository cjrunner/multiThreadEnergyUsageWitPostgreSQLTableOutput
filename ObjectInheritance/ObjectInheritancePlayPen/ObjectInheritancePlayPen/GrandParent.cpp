//
//  GrandParent.cpp
//  ObjectInheritancePlayPen
//
//  Created by Clifford Campo on 6/30/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#include "GrandParent.hpp"
#include "Parent.hpp"
#include "Child.hpp"
GrandParent::GrandParent(thisGeneration **tg, int order)  {
    thisGeneration *me;
    
    me = *tg;
    this->objectId = (*tg + order)->myName;
    this->myGenerationNumber = (me + order)->myGenerationNumber;
    this->forPosterity[0] = "Love you, son.";
    this->forPosterity[1] = "Don't forget to say your prayers before you go to bed!";
    this->granps = this;
}
