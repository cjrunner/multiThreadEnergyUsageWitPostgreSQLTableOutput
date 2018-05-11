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
    const char *dependentVariableColumn;
    int    includeM2InAverages;
    int8_t debug1; //when set, output level 1 debug messages
    int8_t debug2; //when set, output level 2 debug messages
    int8_t debug3; //when set, output level 3 debug messages
} BitFlags;

#endif /* BitFlags_h */
