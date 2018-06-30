//
//  Child.hpp
//  ObjectInheritancePlayPen
//
//  Created by Clifford Campo on 6/30/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#ifndef Child_hpp
#define Child_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>
#include "Parent.hpp"
#include "GrandParent.hpp"
class Child : public Parent {
private:
    const char *objectId;
    int myGenerationNumber;
public:
    Child *thekid;
    Child(thisGeneration **, int=CHILDGENERATION);
    void output (void);

};
#endif /* Child_hpp */
