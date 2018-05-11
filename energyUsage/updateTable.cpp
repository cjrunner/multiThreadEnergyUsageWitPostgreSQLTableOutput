//
//  updateTable.cpp
//  energyUsage
//
//  Created by Clifford Campo on 5/6/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//
//
//  updateTable.cpp
//  c++Postgres
//
//  Created by Clifford Campo on 5/2/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#include "updateTable.hpp"
//#include <iostream>
//#include <pqxx/pqxx>
//#include "myPrototypes.hpp"
//using namespace std;
//using namespace pqxx;

// trim from right-end of string back to the first non-blank character (in place)
// static inline void rtrim(std::string &);
static inline void rtrim(std::string &s) { //Prototype in "myPrototypes.hpp"
    // λ function -----------------------------+
    //                                         |
    //                                         V
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

int updateTable(pqxx::connection *ptrC, const char *const mySQL, const char *connectString, int8_t disconnectFlag) {
    
    try {
        if (ptrC->is_open()) {
            std::cout << "Opened database successfully: " << ptrC->dbname() <<std::endl;
        } else {
            std::cerr << "Can't open database" <<std::endl;
            return 1;
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    /* Create a transactional object. */
    pqxx::work W(*ptrC);
    /* Create  SQL UPDATE statement */
    const char *sqlU = "UPDATE COMPANY set SALARY=25000.00 where ID=2";
    /* Execute SQL query */
    W.exec( sqlU );
    W.commit();
    std::cout << "Records updated successfully" <<std::endl;
    
    /* Create SQL SELECT statement */
    const char *const sqlS = "SELECT * from COMPANY ORDER BY ID";
    
    /* Create a non-transactional object. */
    pqxx::nontransaction N(*ptrC);
    
    /* Execute SQL query */
    pqxx::result R( N.exec( sqlS ));
    
    /* List down all the records */
    
    for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c) {
        std::string tempStr = c[3].as<std::string>(); //Make a copy of this string;
        rtrim(tempStr);
        //            std::std::cout << "ID: " << c[0].as<int>() << ", Name: " << c[1].as<string>() << ", Age: " << c[2].as<int>() << ", Address: " << c[3].as<string>()  << ", Salary: " << c[4].as<float>() << std::endl;
        std::cout << "ID: " << c[0].as<int>() << ", Name: " << c[1].as<std::string>() << ", Age: " << c[2].as<int>() << ", Address: " << tempStr  << ", Salary: " << c[4].as<float>() << std::endl;
    }
    std::cout << "UPDATE Operation performed successfully" <<std::endl;
    ptrC->disconnect ();
    
    
    return 0;
}
