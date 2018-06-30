//
//  Parent.hpp
//  ObjectInheritancePlayPen
//
//  Created by Clifford Campo on 6/30/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#ifndef Parent_hpp
#define Parent_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>
#include "GrandParent.hpp"
class Parent : public GrandParent  {
private:
    const char *objectId;
    int myGenerationNumber;
public:
    Parent(thisGeneration **,  int )  ;
    Parent *theboss;
    std::string forPosterity;
};
#endif /* Parent_hpp */
