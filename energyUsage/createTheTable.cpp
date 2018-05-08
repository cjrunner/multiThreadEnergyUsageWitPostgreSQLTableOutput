//
//  createTheTable.cpp
//  energyUsage
//
//  Created by Clifford Campo on 5/8/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//
#include "myPrototypes.hpp"
#include <iostream>
#include <cstring>
#include <pqxx/pqxx>
extern const char *createTable;
extern const char *createTempTable;
int createTheTable(pqxx::connection *ptrC) {
    int rc=0;
    pqxx::work Work(*ptrC);  // Create a work object, W.
    try {
        Work.exec(createTable);
        Work.commit(); // It is by executing this instruction that the table gets created!
    } catch (const std::exception &e) {
        rc = 2;
    }
    return rc;
} //Used to drop a table if it exists with the second parameter pointing to the SQL
