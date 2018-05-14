//
//  BitFlags.hpp
//  energyUsage
//
//  Created by Clifford Campo on 5/10/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#ifndef BitFlags_hpp
#define BitFlags_hpp
typedef struct BitFlags {
    size_t countOfGoodEntries;
    size_t countOfBadEntries;
    const char *resultsFile;
    char *dependentVariableColumn;
    int    includeM2InAverages;
    size_t indexIntoMotherOfAllSelectStatements;  //takes on the values of 0, 1, or 2
    int8_t debug1; //when set, output level 1 debug messages (main.cpp)
    int8_t debug2; //when set, output level 2 debug messages (selectFromTable. cpp)
    int8_t debug3; //when set, output level 3 debug messages (InsertIBucketizedEnergyValuesIntoTable.cpp)
} BitFlags;

#endif /* BitFlags_h */
