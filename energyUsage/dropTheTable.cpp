//
//  dropTable.cpp
//  energyUsage
//
//  Created by Clifford Campo on 5/8/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//
#include <iostream>
#include <cstring>
#include <pqxx/pqxx>
extern const char *dropTable;
int dropTheTable(pqxx::connection *ptrC) { 
    int rc=0;
    pqxx::work W(*ptrC);  // Create a work object, W.
    try {
        W.exec(dropTable);
        W.commit();
    } catch (const std::exception &e) {
        rc = 4;
    }
    return rc;
} //Used to drop a table if it exists with the second parameter pointing to the SQL
