//
//  beginTransaction.cpp
//  energyUsageWithoutThe_PQXX_Bullshit
//
//  Created by Clifford Campo on 5/15/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//
#include "/usr/local/pgsql/pgsql101/include/libpq-fe.h"
#include <iostream>
#include "myPrototypes.hpp"
#include "baseClass.hpp"
extern PGconn *mycon;
int beginTransaction(BaseClass *const , const char *);
int beginTransaction(BaseClass *const ptrbc, const char *sql, bool doPQclear)  {
    int rc=WONDERFUL;
    if (ptrbc->conn != mycon) {
        ptrbc->conn = mycon;
    }
    ptrbc->res= PQexec(ptrbc->conn,  sql);
    if (PQresultStatus(ptrbc->res) != PGRES_COMMAND_OK) {
        std::cout << "Line " << __LINE__ << "File " << __FILE__ <<  ": Failed to execute the SQL statement: " << sql << ". Error cause:\n" << PQerrorMessage(ptrbc->conn) << std::endl;
        rc = BEGINTRANSACTIONFAILED;
    }
    if (doPQclear) PQclear(ptrbc->res); //Per prototype definition of function beginTransaction, doPQclear defaults to true.
    return rc;
}
