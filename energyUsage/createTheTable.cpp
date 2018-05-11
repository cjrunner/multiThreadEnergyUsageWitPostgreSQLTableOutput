//
//  createTheTable.cpp
//  energyUsage
//
//  Created by Clifford Campo on 5/8/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//


#include <iostream>
#include <cstring>
#include <pqxx/pqxx>
#include "myPrototypes.hpp"
extern const char *createTable;
extern const char *createTempTable;
int createTheTable(pqxx::connection *ptrConnectionStringObject) {
    int rc=0;
    pqxx::work WorkObject(*ptrConnectionStringObject);  // Create a work object, W.
    try {
        WorkObject.exec(createTable);
        WorkObject.commit(); // It is by executing this instruction that the table gets created!
    } catch (const std::exception &e) {
        rc = 2;
    }
    return rc;
} //Used to drop a table if it exists with the second parameter pointing to the SQL
