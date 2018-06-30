//
//  Parent.cpp
//  ObjectInheritancePlayPen
//
//  Created by Clifford Campo on 6/30/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#include "Parent.hpp"
#include "Child.hpp"
#include "GrandParent.hpp"
Parent::Parent(thisGeneration **tg, int order) : GrandParent(tg, order-1) {
    thisGeneration *me;
    
    me = *tg;
    this->objectId = (*tg + order)->myName;
    this->myGenerationNumber = (me + order)->myGenerationNumber;
    forPosterity = "Do your home work so you won't be a jackass!";
}
