//
//  insertIBucketizeEnergyValuesIntoTable.cpp
//  energyUsage
//
//  Created by Clifford Campo on 5/5/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//


//
//  insertIntoTable.cpp
//  c++Postgres
//
//  Created by Clifford Campo on 5/1/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//
#include <iostream>

#include <pqxx/pqxx>


#include "insertIBucketizedEnergyValuesIntoTable.hpp"
#include "myPrototypes.hpp"


#include "makeConnectionString.hpp"

using namespace std;
using namespace pqxx;
extern const char *dropTable;
extern const char *createTable;
extern const char *insertIntoTable;
int insertBucketizedEnergyValuesIntoTable(const char *connStr, \
                                          BitFlags *mcs, \
                                          double avgtemp, \
                                          double avgeu, \
                                          double stddeveu, \
                                          double mineu, \
                                          double maxeu, \
                                          int counteu, \
                                          int8_t disconnectFlag, \
                                          int8_t processingFlag) {
    //    int rc=0;
    char *sql; //Need to do const char *sql rather than just char *sql to make the C++11 compiler happy.
    char sqlbuffer[200];
    sql = sqlbuffer;

    pqxx::connection *ptrForInsert = new pqxx::connection (connStr);  /* << =================================== */ //W A R N I N G: new, unlike calloc, does not pre-initialize acquired memory to a known state.
    if (ptrForInsert->is_open()) {
        if (mcs->debug2) std::cout << "We successfully opened a second connection to database " << ptrForInsert->dbname() << " for doing inserts. " << std::endl;
        ptrForInsert->activate();
    } else {
        std::cerr << "We failed to another connection to database for doing simultaneous inserts into another table " << std::endl;
    }

    sprintf (sqlbuffer, insertIntoTable, avgtemp, avgeu, stddeveu, mineu, maxeu, counteu); //CREATE the insert into SQL statement
    if ( mcs->debug3  ) std::cout << "Insert looks like: " << sqlbuffer << std::endl;
    work WI(*ptrForInsert);  //Create a work object, WI.
    /* Execute the INSERT INTO SQL statement we just constructed with above sprintf statement */

    WI.exec( sqlbuffer );
    WI.commit();
    
    if (mcs->debug3) {
        if (mcs->debug3) std::cout << "Record inserted successfully" << endl;
    }

//    WI.~transaction(); //End the insert transaction by destroying the work object, WI.
    ptrForInsert->disconnect();
    return 0;
}





