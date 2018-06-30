//
//  Child.cpp
//  ObjectInheritancePlayPen
//
//  Created by Clifford Campo on 6/30/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#include "Child.hpp"
#include "Parent.hpp"
#include "GrandParent.hpp"
Child::Child(thisGeneration **tg, int order) :  Parent::Parent(tg,  order-1)  {
    thisGeneration *me;

    me = *tg;
    this->objectId = (*tg + order)->myName;
    this->myGenerationNumber = (me + order)->myGenerationNumber;
}
