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
int dropTheTable(pqxx::connection *ptrConnectionStringObject) {
    int rc=0;
    pqxx::work WorkObject(*ptrConnectionStringObject);  // Create a work object, W.
    try {
        WorkObject.exec(dropTable);
        WorkObject.commit();
    } catch (const std::exception &e) {
        rc = 4;
    }
    return rc;
} //Used to drop a table if it exists with the second parameter pointing to the SQL
