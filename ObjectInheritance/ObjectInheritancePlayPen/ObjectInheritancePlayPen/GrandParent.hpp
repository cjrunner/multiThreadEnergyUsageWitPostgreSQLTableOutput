//
//  GrandParent.hpp
//  ObjectInheritancePlayPen
//
//  Created by Clifford Campo on 6/30/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#ifndef GrandParent_hpp
#define GrandParent_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>

typedef   struct {
    const char *myName;
    int myGenerationNumber;
} thisGeneration;
#define CHILDGENERATION 2
#define PARENTGENERATION CHILDGENERATION - 1
#define GRANDPARENTGENERATION PARENTGENERATION -1
class GrandParent {
private:
    const char *objectId;
    int myGenerationNumber;
public:
    GrandParent(thisGeneration **, int=GRANDPARENTGENERATION);
    std::string forPosterity[2];
    GrandParent *granps;
};
#endif /* GrandParent_hpp */
