//
//  AllocateWorkingMemory.hpp
//  energyUsage
//
//  Created by Clifford Campo on 5/3/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#ifndef AllocateWorkingVectors_hpp
#define AllocateWorkingVectors_hpp

#include <iostream>
class AllocateWorkingVectors {
private:
    size_t numberOfRows;
public:
    double *temperatureVector;
    double *energyUsageVector;
    double *weightVector; //Occasionally we'll find a value other than one.
    AllocateWorkingVectors(size_t);
    ~AllocateWorkingVectors();
};
AllocateWorkingVectors::AllocateWorkingVectors(size_t nor) {
    this->numberOfRows = nor;
    this->temperatureVector = new double[nor];
    this->energyUsageVector = new double[nor];
    this->weightVector = new double[nor];
    std::cout << "The TemperatureVector is of type: " << typeid(this->temperatureVector).name() << ", The energyUsageVector is of type " << typeid(this->energyUsageVector).name() << ", and the weightVector is of type " << typeid(this->weightVector).name() << std::endl; //WTF!
    
}
AllocateWorkingVectors::~AllocateWorkingVectors () {
    std::cout << "1. Deleting temperatureVector which is " << sizeof(this->temperatureVector) << " bytes long." << std::endl;
    delete this->temperatureVector;
    std::cout << "2. Deleting energyUsageVector which is " << sizeof(this->energyUsageVector) << " bytes long." << std::endl;
    delete this->energyUsageVector;
    std::cout << "3. Deleting weightVector which is " << sizeof(this->weightVector) << " bytes long." << std::endl;
    delete this->weightVector;
    
}
#endif /* AllocateWorkingMemory_h */
