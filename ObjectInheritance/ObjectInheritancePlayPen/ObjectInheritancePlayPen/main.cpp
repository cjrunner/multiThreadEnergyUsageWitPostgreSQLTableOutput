//
//  main.cpp
//  ObjectInheritancePlayPen
//
//  Created by Clifford Campo on 6/30/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#include <iostream>
#include "GrandParent.hpp"
#include "Parent.hpp"
#include "Child.hpp"
int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, *(argv + 0)!\n";
    thisGeneration gen[]= {{"Grand Parent",0}, {"Parent",1}, {"Child",2}};
    thisGeneration *tg;
    tg = gen;
    Child c(&tg, CHILDGENERATION);
    Child  *ctwo = new Child(&tg, CHILDGENERATION);
    std::cout << "Parent says to child named ctwo" << ctwo->Parent::theboss->forPosterity << std::endl;
    std::cout << "Grand Parent says to grandchild named ctwo" << ctwo->Parent::theboss->granps->forPosterity[1] << ". While saying to his son: " << ctwo->Parent::theboss->granps->forPosterity[0] << std::endl;
    return 0;
}
