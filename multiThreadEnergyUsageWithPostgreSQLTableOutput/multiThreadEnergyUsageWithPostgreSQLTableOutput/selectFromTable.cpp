//
//  selectFromTable.cpp
//  c++Postgres
//
//  Created by Clifford Campo on 5/1/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//
/*
 SELECT Operation

*/
/*
int     polynomialFit(int polynomialdegree, int reccntr, double *XVALin, double *YVALin, double *FPXVAL, double *IPXVAL, double *returnJD, double *returnZD, char *returnISO8601, double *TSS, double *goodnessOfFit);   // <==== New Way of Calling what used to be a test program
 
 id  |    date    |   time   | microsofttime | m1kwh | m2kwh | avtempf | avtempk | m1m2kwh |       jd       | siteid
 ------+------------+----------+---------------+-------+-------+---------+---------+---------+----------------+--------
 2874 | 2018-05-04 | 05:01:00 |   43224.20903 | 42940 |  7734 |         |         |   11.51 |  2458243.20903 |
 2873 | 2018-05-03 | 06:05:00 |  43223.253472 | 42930 |  7733 | 62.2122 | 289.935 |    7.18 | 2458242.253472 |      9
 2872 | 2018-05-02 | 06:41:00 |  43222.278472 | 42924 |  7732 | 53.9762 | 285.359 |   12.32 | 2458241.278472 |      9
 2871 | 2018-05-01 | 07:18:00 |  43221.304167 | 42913 |  7731 | 52.4003 | 284.484 |   25.16 | 2458240.304167 |      9
 2870 | 2018-04-30 | 06:30:00 |  43220.270833 | 42889 |  7729 | 47.0272 | 281.498 |   23.81 | 2458239.270833 |      9
 2868 | 2018-04-29 | 07:19:00 |  43219.304861 | 42867 |  7728 | 57.8237 | 287.496 |   14.12 | 2458238.304861 |      9
 2867 | 2018-04-28 | 10:55:00 |  43218.454861 | 42856 |  7727 |  51.817 | 284.159 |   21.83 | 2458237.454861 |      9
 2865 | 2018-04-27 | 06:20:00 |  43217.263889 | 42833 |  7724 | 57.8552 | 287.514 |   13.57 | 2458236.263889 |      9
 2866 | 2018-04-26 | 07:20:00 |  43216.305556 | 42821 |  7723 |   58.89 |  288.09 |   17.11 | 2458235.305556 |      9
 2864 | 2018-04-25 | 06:05:00 |  43215.253472 | 42804 |  7722 | 57.0455 | 287.064 |   15.92 | 2458234.253472 |      9
 2863 | 2018-04-24 | 07:28:00 |  43214.311111 | 42790 |  7721 | 55.8987 | 286.427 |   15.42 | 2458233.311111 |      9
 2862 | 2018-04-23 | 06:34:00 |  43213.273611 | 42775 |  7720 | 55.3796 | 286.139 |   17.86 | 2458232.273611 |      9
 */
#include <stdio.h>
#include <iostream>
#include <fstream> 
#include <cstring>
#include <string>
#include <sys/select.h>
#include <sys/time.h>
//#include <unistd.h>`
#include <sstream>
#include "effectConnection.hpp"
#include "/usr/local/pgsql/pgsql101/include/libpq-fe.h"
#include "myPrototypes.hpp"
#include "AllocateWorkingVectors.hpp"
#include "baseClass.hpp"
#include "insertIBucketizedEnergyValuesIntoTable.hpp"
#include "BitFlags.hpp"
#include "setupForMultiFit.hpp"
#include "tblPolyFit.hpp"
#include "TblPolyFitWithCov.hpp"
// include "/Users/cjc/c++/energyUsage/multiThreadEnergyUsageWithPostgreSQLTableOutput/multiThreadEnergyUsageWithPostgreSQLTableOutput/setupForMultiFit.hpp"
#include "alignField.hpp"
#include "varyingType.hpp"

// #include "tblPolyFit.hpp"
std::ostringstream *current_Date_Time(BaseClass *);
int     polynomialFit(int, int, double *, double *,  double *, double *,  double *, double *);
using namespace std;

#ifdef METER //Provided by the compiler -D switch
#define WHICHMETER METER
#else
#define WHICHMETER m1m2kwh   //Set the Default
#endif
extern const char *insertIntoTable[];
extern const char *createTable[];     //From message.cpp
extern const char *readBackTable[];  //SQL SELECT for reading back the data that we previously inserted into table tbl_bucketized_by_tempf_ranges
extern const char *begin_Transaction;
extern const char *end_Transaction;
extern const char *fetchALLTransaction;
extern const char *close_Transaction;
extern const char *cases[]; //Array of strings specifying cases
extern const char *myTempFiles[]; //Array of the names of Temp Files for the m1m2kwh case, the m1kwh case, and the m2kwh case.
extern const char *dependentVariableColumn[];
extern const char *theCaseLabels[];
extern const char *copySQL;
extern const char *theMotherOfAllUnionSelectStatements[];
extern PGconn *mycon;
//
extern const char *motherOfAllSelectStatements[];
extern const char *copy_file[];
//extern const char *dropThenCreateTempTable;
extern const char *drop_tbl_temp_eu[];
extern const char *dropTable[];
extern const char *create_tbl_temp_eu[];
std::string currentDateTime(BaseClass *);
void exit_nicely(BaseClass *,  int, int) ;
void exit_nicely(BaseClass *bc, int rc, int lineNumber) {
    PQfinish(bc->conn); //Close this task's connection.
    cout << currentDateTime(bc) << "\t" << __FILE__ << "Line " << lineNumber << "\nexit_nicely is terminating this program with exit code: " << rc << cases[bc->debugFlags.mycase] << endl;
    bc->selectFromTable_RC = rc; //Duplicate the exit code as selectFromTable_RC this was the mother task will know why the child exited
    exit (rc);
}
std::ostringstream *current_Date_Time( BaseClass * );
std::ostringstream *current_Date_Time( BaseClass *bc ) {
    bc->now = time(0);
    bc->tstruct = *localtime(&bc->now);
    bc->date_Time << (YEAROFSTANDARDEPOCH + bc->tstruct.tm_year) << "-" << (1 + bc->tstruct.tm_mon) << "-" \
    << bc->tstruct.tm_mday << " " << bc->tstruct.tm_hour << ":" << bc->tstruct.tm_min << ":" \
    << bc->tstruct.tm_sec << " " << bc->tstruct.tm_zone;
    return &bc->date_Time;
}
int createTheTable(BaseClass *, const char *, bool = true);
int dropTheTable(BaseClass *, const char *);
int closePortal(BaseClass *, const char *, bool = true);
int endTransaction(BaseClass *, const char *, bool = true);
int executeSQL(BaseClass *, const char *, bool = true);
int beginTransaction(BaseClass *, const char *, bool = true);
int fetchAllTransaction(BaseClass *, const char *, bool = false);
int insertBucketizedEnergyValuesIntoTable( double, double, double, double, double, int, BaseClass *);
template<typename T>
std::string to_string(T t)
{
    std::ostringstream oss;
    oss << t;
    return oss.str();
}

void selectFromTable(register BaseClass *ptrbc, int myRC) {

    enum bucketColNames  {_avgtemp, _avgeu, _stddeveu, _mineu, _maxeu, _countu};  //This enum did not work when placed in BaseClass

    ptrbc->selectFromTable_RC = 0;
    mycon = PQconnectdb(ptrbc->connectionString);
    ptrbc->conn = mycon;  //Copy over the pointer
    if(PQstatus(ptrbc->conn) != CONNECTION_OK ) {
        cerr << "Connection to database failed because: " << PQerrorMessage(ptrbc->conn) << ". We're exiting with exit code of 1."<< endl;
        exit(1);
    } else {
        cout << "It looks like we connected to the database `" << PQdb(ptrbc->conn) << "`, on host `" << PQhost(ptrbc->conn)  << "` ok." << endl;
    }
    if (ptrbc->debugFlags.debug2 ) std::cout << dependentVariableColumn[ptrbc->debugFlags.intMyCase] << currentDateTime(ptrbc) << "\nThe mother of all select statements looks like:\n" << motherOfAllSelectStatements[ptrbc->debugFlags.intMyCase] << std::endl;
//============
//    const char * sqlStat0 = "SELECT ";
    //(8 +10)/2;
    
    ///8
//    const char *sqlStat2 = " AND avtempf <= ";
    //10;
    const char *sqlStat1 = " AS \"AvgIndependentVariable\", avg(%s) AS \"AvgDependentVariable\", stddev(%s) AS \"σOfDependentVariable\", min(%s) AS \"minDependentVariable\", max(%s) AS \"maxDependentVariable\", count(*) AS \"NumberOfDependentVariablesInThisBucket\" FROM tbl_energy_usage WHERE avtempf > ";

    ptrbc->len = strlen(motherOfAllSelectStatements[ptrbc->debugFlags.intMyCase]);
    ptrbc->sql = new char [ ptrbc->len ];
    ptrbc->len = sprintf(ptrbc->sql, motherOfAllSelectStatements[ptrbc->debugFlags.intMyCase], ptrbc->debugFlags.includeM2InAverages);  //At this point ptrbc->debugFlags.includeM2InAverages is either a 1 or a zero, depending upon the presences of the -c command line switch. If -c switch is NOT present then ptrbc->debugFlags.includeM2InAverages = 1, else its zero.

    ptrbc->len = strlen(sqlStat1);
    ptrbc->len += 4 * (11 + strlen( dependentVariableColumn[ptrbc->debugFlags.intMyCase]) ); //Add to the length the anticipated length of the case labels and other data. m1m2kwh is the longest it can be
    ptrbc->subCharBuffer = new char[  ptrbc->len ]; //Allocate space for our buffer
    ptrbc->len = sprintf(ptrbc->subCharBuffer,sqlStat1, dependentVariableColumn[ptrbc->debugFlags.intMyCase], \
                         dependentVariableColumn[ptrbc->debugFlags.intMyCase], \
                         dependentVariableColumn[ptrbc->debugFlags.intMyCase], \
                         dependentVariableColumn[ptrbc->debugFlags.intMyCase]);
//===============

 
/*
    if (ptrbc->debugFlags.tableCreated == true) {  //Have we created the table?
        try {
            ptrbc->rc[SELECTFROMTABLE] = dropTheTable(ptrbc, dropTable[ptrbc->debugFlags.intMyCase]);   //First drop the pre-existing table to get a fresh start.
        } catch (const std::exception &e) {
            std::cerr << "Failed to drop the table, tbl_bucketized_by_tempf_ranges. EXITING " << cases[ptrbc->debugFlags.intMyCase] << std::endl;
            exit(1);
       }
    }  else {
        try {
            if(createTheTable(ptrbc, createTable[ptrbc->debugFlags.intMyCase], true)) std::cerr << "Failed to create the table, tbl_bucketized_by_tempf_ranges. EXITING " << cases[ptrbc->debugFlags.intMyCase] << std::endl; //Go create table tbl_bucketized_by_tempf_ranges
        } catch (const std::exception &e) {
            std::cerr << "Failed to create the table, tbl_bucketized_by_tempf_ranges. EXITING " << cases[ptrbc->debugFlags.intMyCase] << std::endl;
            exit(2);
        }
        ptrbc->debugFlags.tableCreated = true;  //Indicate table created.
    }
 */
    
//     MOVED TO myPrototypes   enum colnames {_currentavtempf,  _previousavtempf,  _davtempf,  _currentavtempk,  _previousavtempk,   _previousdate,   _currentdate, _dday,  _previousm1kwh, _currentm1kwh, _m1Usage, _previousm2kwh,  _currentm2kwh, _m2Usage, _AvgDailyUsage};

    ptrbc->res = PQexec(ptrbc->conn,  createTable[ptrbc->debugFlags.intMyCase]);  //Drop the table, if it exists, then recreate the table specific for this case.
    if (PQresultStatus(ptrbc->res) != PGRES_COMMAND_OK) {
        std::cerr << "Line " << __LINE__ << ", File " << __FILE__ << ", case "  << cases[ptrbc->debugFlags.intMyCase] <<": Failed to execute the SQL statement: " << createTable[ptrbc->debugFlags.intMyCase] << ". Error cause:\n" << PQerrorMessage(ptrbc->conn) << std::endl;
        exit_nicely(ptrbc, BEGINTRANSACTIONFAILED, __LINE__);
    }
    PQclear(ptrbc->res);
    
    ptrbc->res = PQexec(ptrbc->conn,  begin_Transaction);
    if (PQresultStatus(ptrbc->res) != PGRES_COMMAND_OK) {
        std::cerr << "Line " << __LINE__ << ", File " << __FILE__ << ", case " << cases[ptrbc->debugFlags.intMyCase] <<": Failed to execute the SQL statement: " << begin_Transaction << ". Error cause:\n" << PQerrorMessage(ptrbc->conn) << std::endl;
        exit_nicely(ptrbc, 3, __LINE__);
    }
    //    PQclear(ptrbc->res);

//    rc = executeSQL(ptrC, ptrbc->res, ptrBigSQL);
    ptrbc->res = PQexec(ptrbc->conn,  motherOfAllSelectStatements[ptrbc->debugFlags.intMyCase]);
    if (PQresultStatus(ptrbc->res) != PGRES_COMMAND_OK) {
        std::cerr << "Line " << __LINE__ << " of file " << __FILE__ << ": Failed to execute the SQL statement: " << motherOfAllSelectStatements[ptrbc->debugFlags.intMyCase] << ". Error cause:\n" <<
        PQerrorMessage(ptrbc->conn) << cases[ptrbc->debugFlags.intMyCase] << std::endl;
        exit_nicely(ptrbc, EXECUTESQLFAILED, __LINE__);

    } else {
        PQclear(ptrbc->res);
    }
//    rc = fetchAllTransaction(ptrC,ptrbc->res, fetchALLTransaction);
    ptrbc->res = PQexec(ptrbc->conn,  "FETCH ALL in myportal" );
    if (PQresultStatus(ptrbc->res) != PGRES_TUPLES_OK) {
        std::cerr << "Failed to execute the SQL FETCH ALL: " << ptrbc->sql << ". Error cause:\n" <<
        PQerrorMessage(ptrbc->conn) << cases[ptrbc->debugFlags.intMyCase] << std::endl;
        cerr << "Line " << __LINE__ << "Of file " << __FILE__ << "Failed when attempting to execute this sql:\n" << motherOfAllSelectStatements[ptrbc->debugFlags.intMyCase] << "\nWe got a return status of: " << PQresultStatus(ptrbc->res) << cases[ptrbc->debugFlags.intMyCase] << endl;
        exit_nicely(ptrbc, FETCHALLFAILED, __LINE__);
    }

    ptrbc->numberOfReturnedRows = PQntuples(ptrbc->res);
    ptrbc->numberOfReturnedColumns = PQnfields(ptrbc->res);

//    Align *af = new Align(ASSUMEDBUFFERSIZE);
    const char *headerRow[] ={" currentavtempf | prevavtempf |  ∆avtempf  | currentavtempk | prevavtempk |   yesterday    |   today    | ∆day | cm1kwh | pm1kwh | m1Usage | cm2kwh | pm2kwh | m2Usage | AvgDailyUsage",
                                "----------------+-------------+------------+----------------+-------------+----------------+------------+------+------- +------- +---------+------- +- ------+---------+---------------+"}; \
    /*  Example of what's returned by this mother of all select statements:
          col00           col01         col02          col03          col04          col05          col06     col07  col08   col09    col10    col11   col12   col13        col14
          16w              13w          12wide        16wide          13wide         16wide        12wide      6W      8w      7w      9w       7w     7w       9w         15w
     currentavtempf | prevavtempf |  ∆avtempf  | currentavtempk | prevavtempk |   yesterday    |   today    | ∆day | m1kwh | m1kwh | m1Usage | m2kwh | m2kwh | m2Usage | AvgDailyUsage
    ----------------+-------------+------------+----------------+-------------+----------------+------------+------+-------+-------+---------+-------+-------+---------+---------------
         64.4       |       38.86 |     -25.54 |         291.15 |      276.96 |     2014-12-03 | 2014-12-01 |    2 | 12776 | 12720 |      56 |  2670 |  2665 |       5 |          30.5
        46.15       |       38.86 |      -7.29 |         281.01 |      276.96 |     2014-12-03 | 2014-12-02 |    1 | 12776 | 12739 |      37 |  2670 |  2667 |       3 |            40
        38.86       |       39.27 |       0.41 |         276.96 |      277.19 |     2014-12-04 | 2014-12-03 |    1 | 12813 | 12776 |      37 |  2673 |  2670 |       3 |            40
        39.27       |       34.82 |      -4.45 |         277.19 |      274.71 |     2014-12-05 | 2014-12-04 |    1 | 12852 | 12813 |      39 |  2675 |  2673 |       2 |            41
        34.82       |       40.55 |       5.73 |         274.71 |      277.90 |     2014-12-06 | 2014-12-05 |    1 | 12890 | 12852 |      38 |  2678 |  2675 |       3 |            41
             .          .            .                     .          .                .          .           .       .          .      .      .           .               .
             .          .            .                     .          .                .          .           .       .          .      .      .           .               .
             .          .            .                     .          .                .          .           .       .          .      .      .           .               .

*/

    Align *af = new Align(ASSUMEDBUFFERSIZE);
    af->alignSetup(   headerRow[1]);
enum colwidths {col00 = 16, col01=13, col02=12, col03=16, col04=13, col05=16, col06=12, col07=6,col08=8, col09=8, col10=9, col11=8, col12=8, col13=9, col14=15};
#define NEITHERFIRSTORLAST 2 //This means we will not include the first elemet or the last element because they are misleading and erroneous: \
      a) The first element lacks the previous day's reading;  \
      b) Likely, the last element does not have a full-day's worth of readings.
    AllocateWorkingVectors *awv = new AllocateWorkingVectors (ptrbc->numberOfReturnedRows - NEITHERFIRSTORLAST);   /* << =================== awv object ================ */ //Strange, yet powerful, things these templates!!
    if (ptrbc->debugFlags.debug2 ) std::cout  <<headerRow[0] << "\t" << theCaseLabels[ptrbc->debugFlags.intMyCase] << std::endl;
    if (ptrbc->debugFlags.debug2 ) std::cout  <<headerRow[1] << "\t" << theCaseLabels[ptrbc->debugFlags.intMyCase] << std::endl;

    const size_t bufferSize = SIZEOFONEROW  *  ptrbc->numberOfReturnedRows;
#define THISROWISNULL 1
//    int ptrbc->numberOfReturnedRows;

    ptrbc->copyBuffer = new char[bufferSize]; //Get sufficient space to store, in one buffer, all temperture and energy usage (eu) data.
    memset(ptrbc->copyBuffer, 0, bufferSize); //Start with a clean copyBuffer
    ptrbc->bufferOffset = 0;  //The constructor should initialize this.
    ptrbc->grc = 0;
    ptrbc->brc = 0;
    for (ptrbc->loopCounter = 0; ptrbc->loopCounter < ptrbc->numberOfReturnedRows; ptrbc->loopCounter++ ) {
        if (PQgetisnull(ptrbc->res, ptrbc->loopCounter, _AvgDailyUsage) == THISROWISNULL) { //PQgetisnull Returns 1 if row IS NULL. Returns 0 if row is NOT NULL
            ++ptrbc->brc; //increment the bad row count.
            continue; //Skip NULL values of today's avgtempf and the first record and the last record
        }
        try {
            if (ptrbc->debugFlags.debug2 ) {
                cout << af->alignCenter(0,  PQgetvalue(ptrbc->res, ptrbc->loopCounter, _currentavtempf) ) << "|";
                cout << af->alignCenter(1,  PQgetvalue(ptrbc->res, ptrbc->loopCounter, _previousavtempf) ) << "|";
                cout << af->alignCenter(2,  PQgetvalue(ptrbc->res, ptrbc->loopCounter, _davtempf) ) << "|" ;
                cout << af->alignCenter(3,  PQgetvalue(ptrbc->res, ptrbc->loopCounter, _currentavtempk) ) << "|" ;
                cout << af->alignCenter(4,  PQgetvalue(ptrbc->res, ptrbc->loopCounter, _previousavtempk) ) << "|" ;
                cout << af->alignCenter(5,  PQgetvalue(ptrbc->res, ptrbc->loopCounter, _previousdate) ) << "|" ;
                cout << af->alignCenter(6,  PQgetvalue(ptrbc->res, ptrbc->loopCounter, _currentdate) ) << "|" ;
                cout << af->alignCenter(7,  PQgetvalue(ptrbc->res, ptrbc->loopCounter, _dday) ) << "|" ;
                cout << af->alignCenter(8,  PQgetvalue(ptrbc->res, ptrbc->loopCounter, _currentm1kwh) ) << "|" ;
                cout << af->alignCenter(9,  PQgetvalue(ptrbc->res, ptrbc->loopCounter, _previousm1kwh) ) << "|" ;
                cout << af->alignCenter(10, PQgetvalue(ptrbc->res, ptrbc->loopCounter, _m1Usage) ) << "|" ;
                cout << af->alignCenter(11, PQgetvalue(ptrbc->res, ptrbc->loopCounter, _currentm2kwh) ) << "|" ;
                cout << af->alignCenter(12, PQgetvalue(ptrbc->res, ptrbc->loopCounter, _previousm2kwh) ) << "|" ;
                cout << af->alignCenter(13, PQgetvalue(ptrbc->res, ptrbc->loopCounter, _m2Usage) ) << "|" ;
                cout << af->alignCenter(14, PQgetvalue(ptrbc->res, ptrbc->loopCounter, _AvgDailyUsage) ) << "|";
                cout << cases[ptrbc->debugFlags.intMyCase] << endl;
            }
            switch (ptrbc->debugFlags.mycase) {
                case M1M2KWH:
                    ptrbc->eu = PQgetvalue(ptrbc->res, ptrbc->loopCounter, _AvgDailyUsage);
                    awv->energyUsageVector[ptrbc->grc] = atof(ptrbc->eu);
                    break;
                case M1KWH:
                    ptrbc->eu = PQgetvalue(ptrbc->res, ptrbc->loopCounter, _m1Usage);
                    awv->energyUsageVector[ptrbc->grc]  = atof(ptrbc->eu);
                    break;
                case M2KWH:
                    ptrbc->eu = PQgetvalue(ptrbc->res, ptrbc->loopCounter, _m2Usage);
                    awv->energyUsageVector[ptrbc->grc]  = atof(ptrbc->eu);
                    break;
                default:
                    cerr << "Line " << __LINE__ << "File " << __FILE__ << "It seems like we picked up an undefined value for ptrbc->debugFlags.mycase: " << ptrbc->debugFlags.mycase << "\t We're terminating." << cases[ptrbc->debugFlags.intMyCase] << endl;
                    exit (BADMYCASEVALUE);
            }
            ptrbc->temp = PQgetvalue(ptrbc->res, ptrbc->loopCounter, _currentavtempf);
            if(strlen(ptrbc->temp) < 1) {
                continue; // skip this row if nothing returned because COPY doesn't like nothing there
            } else {
                awv->temperatureVector[ptrbc->grc]  = atof(ptrbc->temp);
            }
            strcpy(ptrbc->copyBuffer + ptrbc->bufferOffset, ptrbc->temp );
            ptrbc->bufferOffset += strlen(ptrbc->temp);
            *(ptrbc->copyBuffer+ptrbc->bufferOffset) = ','; //insert a comma to separate the fields.
            ptrbc->bufferOffset++;     //Accout for the comma we just added
            *(ptrbc->copyBuffer + ptrbc->bufferOffset) = ' '; //insert a comma to separate the fields.
//            strcpy(ptrbc->copyBuffer+ptrbc->bufferOffset, ", ");
            ptrbc->bufferOffset++; //strlen(","); //account for the space character  we added, above.
            strcpy(ptrbc->copyBuffer + ptrbc->bufferOffset, ptrbc->eu);  //ptrbc->eu is different for the m1m2kwh case than for the m1kwh and m2kwh cases. Process differently
            ptrbc->bufferOffset += strlen(ptrbc->eu); //Account for the eu data we just added
            *(ptrbc->copyBuffer+ptrbc->bufferOffset) = '\n'; //insert a new line character so postgres's COPY will understand that this is the end of the row.
//        strcpy(ptrbc->copyBuffer+ptrbc->bufferOffset, "\n");
            ptrbc->bufferOffset++; //strlen("\n"); account for the '\n' character
            *(ptrbc->copyBuffer + ptrbc->bufferOffset) = '\0'; //Terminate our growing string. Do we really have to do this since we did a massive memset just after allocating this dynamic instruction?
            //But don't update the ptrbc->bufferOffset to account for the above NULL character. We'll just over-write it with the temp data if it's there.
            awv->weightVector[ptrbc->grc] = atof(PQgetvalue(ptrbc->res, ptrbc->loopCounter, _dday));
            ptrbc->grc++; //increment the good row count
        } catch (const std::exception &e) {
            std::cerr << "On row: " << ptrbc->grc << ", we encountered some kind of conversion error, like trying to convert a null to an int or a float. [["<< e.what() << "]] Skipping this row, however processing continues." << cases[ptrbc->debugFlags.intMyCase] << std::endl;
            ++ptrbc->brc; //increment the bad row count.
        }
        ++ptrbc->trc_t; //Increment the total row count, includes valid and invalid rows.
        
    }  //End of for loop
    strcat(ptrbc->copyBuffer , "\\." );   //The COPY command likes to see `\.` as the penultimate line.
    ptrbc->bufferOffset += 2;
    *(ptrbc->copyBuffer+ptrbc->bufferOffset) = '\n';   //The COPY command likes to see `\n` as the final line.
    ptrbc->bufferOffset++;
    *(ptrbc->copyBuffer+ptrbc->bufferOffset) = '\0';
    if (ptrbc->debugFlags.debug2) cout << "copyBuffer Looks Like: \n" << ptrbc->copyBuffer << endl;
    if (ptrbc->debugFlags.debug2) std::cout << "\nNumber of VALID Returned Rows: " << ptrbc->grc << ".\nNumber of INVALID Rows: " << ptrbc->brc << ".\nTotal Number of rows processed: " << ptrbc->grc + ptrbc->brc << ".\nNumber of unprocessed rows: " << ptrbc->numberOfReturnedRows-ptrbc->trc_t << "\n length of ptrbc->copyBuffer " << strlen(ptrbc->copyBuffer) << " bytes." << cases[ptrbc->debugFlags.intMyCase] << std::endl;
    
    if (ptrbc->debugFlags.debug2)  std::cout << "Successfully performed SELECT operation on all data.\nNow let's look at what we've put in the temperatureVector and the energyUsageVector." << cases[ptrbc->debugFlags.intMyCase] << std::endl;
    
    
    if (ptrbc->debugFlags.debug2 ) {
        for (ptrbc->k=0; ptrbc->k < ptrbc->numberOfReturnedRows - ptrbc->brc; ptrbc->k++) {
            
            std::cout << ptrbc->k << ". " << awv->temperatureVector[ptrbc->k] << ", " << awv->energyUsageVector[ptrbc->k] << cases[ptrbc->debugFlags.intMyCase] << std::endl;
        }
    }
    //Take a character --  as in ptrbc->debugFlags.mycase -- and turn \
    it into an integer, so arrays can use it as an index, by subtracting the character '0'
    ptrbc->k = (int)(ptrbc->debugFlags.mycase - '0');
    ptrbc->theTempFile = myTempFiles[ptrbc->k];
    /*  These data in ptrbc->theTempFile will be used by the copy operation, below */
    ptrbc->tmpfile = fopen(ptrbc->theTempFile, "w");
    for(ptrbc->row=0; ptrbc->row < strlen(ptrbc->copyBuffer); ptrbc->row++) {
        ptrbc->rc[SELECTFROMTABLE] = fputc(*(ptrbc->copyBuffer + ptrbc->row), ptrbc->tmpfile);
    }
    fclose(ptrbc->tmpfile);
    /*  SUDDENLY, EVEN the simple stuff doesn't work.
    ptrbc->myTempFile.open(ptrbc->theTempFile, ios_base::out);   //2018-06-26T05:45:48: Never had to do std::ios_base::out
    ptrbc->myTempFile  << ptrbc->copyBuffer  << endl;
    ptrbc->myTempFile.close();  //Close the output file
    */
    ptrbc->allocateArrays(ptrbc->grc);
    for(ptrbc->loopCounter = 0; ptrbc->loopCounter<ptrbc->grc; ptrbc->loopCounter++ )  {
        ptrbc->setArray(IV, ptrbc->loopCounter, awv->temperatureVector[ ptrbc->loopCounter] ); //Place the independent variable, Average Daily Temperature, into its designated array.
        ptrbc->setArray(ODVA, ptrbc->loopCounter, awv->energyUsageVector[ ptrbc->loopCounter] ); //Place the observed dependent variable (EnergyUsage as Read (Observed) from the electric meters into its array.

    }

    //WE will  now close the portal and end the transaction because I don't think we can do a create table on the ptrC connection with a transaction in progress.
    PQclear(ptrbc->res); // 2018-06-29T19:35:35 Do we want to do this here and now?
    ptrbc->res = PQexec(ptrbc->conn,  "CLOSE myportal; END;");
    if (PQresultStatus(ptrbc->res) != PGRES_COMMAND_OK) {
        std::cerr << "Line " << __LINE__ << " of file " << __FILE__ << ": Failed to execute the SQL statement: " << motherOfAllSelectStatements[ptrbc->debugFlags.intMyCase] << ". Error cause:\n" <<
        PQerrorMessage(ptrbc->conn) << cases[ptrbc->debugFlags.intMyCase] << std::endl;
        exit_nicely(ptrbc, EXECUTESQLFAILED, __LINE__);
        
    } else {
        PQclear(ptrbc->res);
    }
    
    /* This routine, closePortal, chooses to set ptrbc->conn to NULL upon entry. Why, I don't know. but the above code where we execute the "CLOSE myportal; END;" SQL statements together is my workaround.
    ptrbc->rc[SELECTFROMTABLE] = closePortal(ptrbc , "CLOSE myportal");  //Close myportal before ending the transaction. closePortal does a ¡PQclear! ;

    ptrbc->rc[SELECTFROMTABLE] = endTransaction(ptrbc, "END");  //End the transaction. endTransaction does a ¡PQclear!
*/
    //Create a temporary table named tbl_temp_eux; it is here where we will mirror on a temporary table the data that went into the temperature vector and the energy usage vector, above.
    if (!ptrbc->debugFlags.tempTableCreated) {  //Have we created the temporary table?
        //We will do the DROP Table and Create Table with one SQL statement
        ptrbc->res = PQexec(ptrbc->conn, drop_tbl_temp_eu[ptrbc->debugFlags.intMyCase]);
        if (PQresultStatus(ptrbc->res) != PGRES_COMMAND_OK ) {
            cerr << drop_tbl_temp_eu[ptrbc->debugFlags.intMyCase] << " command failed " << cases[ptrbc->debugFlags.intMyCase] << endl;
            exit_nicely(ptrbc, DROPTABLEFAILED, __LINE__);
        }
        PQclear(ptrbc->res);
        ptrbc->res = PQexec(ptrbc->conn, create_tbl_temp_eu[ptrbc->debugFlags.intMyCase] );
        if (PQresultStatus(ptrbc->res) != PGRES_COMMAND_OK ) {
            cerr << create_tbl_temp_eu[ptrbc->debugFlags.intMyCase] << " command failed " << cases[ptrbc->debugFlags.intMyCase] << endl;
            exit_nicely(ptrbc, CREATETABLEFAILED, __LINE__);
        }
        PQclear(ptrbc->res);
        ptrbc->debugFlags.tempTableCreated = true;  //Indicate table created.
    }
// BEGINNING OF COPY OPERATION
// No Longer Used    const char *errmsg=NULL;
    ptrbc->errmsgCOPY="";
    if (ptrbc->debugFlags.debug2 ) cout << "COPY SQL looks like: " << copy_file[ptrbc->debugFlags.intMyCase] << endl;
    ptrbc->resC = PQexec(ptrbc->conn,  copy_file[ptrbc->debugFlags.intMyCase] ); //Execute the SQL statement: COPY tbl_temp_eu (temp, eu) STDIN;
//    PQclear(ptrbc->resC);
    ptrbc->resultsPQputCopyData = PQputCopyData(ptrbc->conn, ptrbc->copyBuffer, (int)strlen(ptrbc->copyBuffer));
    ptrbc->resultsPQputCopyEnd = PQputCopyEnd(ptrbc->conn, ptrbc->errmsgCOPY);
//    int whichIsIt = PQresultStatus(ptrbc->res);
    if (ptrbc->debugFlags.debug2 )  cout << " resultsPQputCopyData: " << ptrbc->resultsPQputCopyData << "; resultsPQputCopyEnd: " << ptrbc->resultsPQputCopyEnd << cases[ptrbc->debugFlags.intMyCase] << endl;
    
    if (strlen(ptrbc->errmsgCOPY)) {
        cerr << __LINE__ << " of file " << __FILE__ << ": COPY operation failed with error message of-->" << ptrbc->errmsgCOPY << "<--" <<cases[ptrbc->debugFlags.intMyCase] << endl;
    } else {
        if (ptrbc->debugFlags.debug2 ) cout << "COPY operation seems to have worked!" << cases[ptrbc->debugFlags.intMyCase] << endl;

    }
    PQclear(ptrbc->resC);
//END OF COPY OPERATION

    ptrbc->rc[SELECTFROMTABLE] = beginTransaction(ptrbc, begin_Transaction);  //Start up a new transaction where we will
    if (ptrbc->rc[SELECTFROMTABLE]) exit_nicely(ptrbc, ptrbc->rc[SELECTFROMTABLE], __LINE__);

    ptrbc->rc[SELECTFROMTABLE] = executeSQL(ptrbc, theMotherOfAllUnionSelectStatements[ptrbc->debugFlags.intMyCase] );   //
    if (ptrbc->rc[SELECTFROMTABLE]) exit_nicely(ptrbc, ptrbc->rc[SELECTFROMTABLE], __LINE__);
    ptrbc->rc[SELECTFROMTABLE] = fetchAllTransaction(ptrbc, fetchALLTransaction);
    if (ptrbc->rc[SELECTFROMTABLE]) { //Note that PGRES_TUPLES_OK happens when PQresultStatus returns a 2.
        cerr << "Failed when attempting to do a FETCH ALL execute this sql:\n" << theMotherOfAllUnionSelectStatements[ptrbc->debugFlags.intMyCase] << "\nWe got a return status of: " << PQresultStatus(ptrbc->res) <<  cases[ptrbc->debugFlags.intMyCase] <<  endl;
        exit_nicely(ptrbc, ptrbc->rc[SELECTFROMTABLE], __LINE__);
    } else {
        ptrbc->numberOfReturnedRows = PQntuples(ptrbc->res);
        ptrbc->numberOfReturnedColumns = PQnfields(ptrbc->res);
        if (ptrbc->debugFlags.debug2 ) cout << "TheMotherOfAllUnionSelectStatements returned " << ptrbc->numberOfReturnedRows << " rows and " <<    ptrbc->numberOfReturnedColumns << " columns." << cases[ptrbc->debugFlags.intMyCase] <<  endl;
    }

    if (ptrbc->debugFlags.debug2 ) cout << "Now bucketize the data. Each bucket has a temperature range of " << BUCKETSIZE << "ºF " << " and we will get the average energy usage, in kwh, for that temperature range buffer along with the energy usage standard deviation. " << cases[ptrbc->debugFlags.intMyCase] << endl;
    ptrbc->sqlbuffer= new char[SIZEOFBUFFERCONTAININGMOTHEROFALLSELECTSTATEMNTS];   /* << =================================== Dynamically acquire sufficient storage for constructing, piece-by-piece, the Mother Of All Select Statements. */

    
    const char *headerEU[2] = {"  avgtemp | avg eu |      stddev       |  min  |  max  | count\n", \
                                 "----------+--------+-------------------+-------+-------+-------\n"};
    if (ptrbc->debugFlags.debug2 ) cout << *(headerEU +0) << *(headerEU + 1) << cases[ptrbc->debugFlags.intMyCase] << std::endl; //Output the headers.//
    
    Align *a_ff = new Align(ASSUMEDBUFFERSIZE);
    a_ff->alignSetup( headerEU[1]);
    
    ptrbc->loopCounter = 0; //Reset this.
    ptrbc->worthyOfPolynomialFitProcessing=0;
    ptrbc->countOfGoodEntries = 0;
    ptrbc->countOfBadEntries = 0;
    ptrbc->buckettemp = MINTEMPERATURE; //Start out at the minimum temperature.
    while ( (ptrbc->buckettemp <  MAXTEMPERATURE) && (ptrbc->loopCounter < ptrbc->numberOfReturnedRows) ) {  //Beginning of while statement.
        // See http://libpqxx.readthedocs.io/en/latest/a00004.html for explanation of how the next two lines work

        if ( ( PQgetisnull(ptrbc->res, ptrbc->loopCounter, _countu) ) ) { //Weed-out any bad records that contain poisonous fields.
            
            ++ptrbc->loopCounter;
            continue; //Skip this record if any critical values come back as NULL. Go back to top of while loop.
        }
        //            for (result::const_iterator column = r.rbegin(); column != r.rend(); ++column) { //Beginning of for loop

        try {
            if (ptrbc->debugFlags.debug2 ) {

                std::cout << a_ff->alignCenter(0,  PQgetvalue(ptrbc->res, ptrbc->loopCounter, _avgtemp) ) << "|";
                std::cout << a_ff->alignCenter(1,  PQgetvalue(ptrbc->res, ptrbc->loopCounter, _avgeu) ) << "|";
                std::cout << a_ff->alignCenter(2,  PQgetvalue(ptrbc->res, ptrbc->loopCounter, _stddeveu) ) << "|";
                std::cout << a_ff->alignCenter(3,  PQgetvalue(ptrbc->res, ptrbc->loopCounter, _mineu) ) << "|";
                std::cout << a_ff->alignCenter(4,  PQgetvalue(ptrbc->res, ptrbc->loopCounter, _maxeu) ) << "|";
                std::cout << a_ff->alignCenter(5,  PQgetvalue(ptrbc->res, ptrbc->loopCounter, _countu) ) << "|" <<  cases[ptrbc->debugFlags.intMyCase] << endl;
            }
            if( (ptrbc->thisCount =atoi( PQgetvalue(ptrbc->res, ptrbc->loopCounter, _countu) ) ) > 1 ) {
                ptrbc->values[_avgtemp] = atof(PQgetvalue(ptrbc->res, ptrbc->loopCounter, _avgtemp) );
                ptrbc->values[_avgeu]= atof(PQgetvalue(ptrbc->res, ptrbc->loopCounter, _avgeu));
                ptrbc->values[_stddeveu] =  atof( PQgetvalue(ptrbc->res, ptrbc->loopCounter, _stddeveu) );
                ptrbc->values[_mineu] = atof(PQgetvalue(ptrbc->res, ptrbc->loopCounter, _mineu));
                ptrbc->values[_maxeu] = atof(PQgetvalue(ptrbc->res, ptrbc->loopCounter, _maxeu) );
                ptrbc->thisCount = atoi(PQgetvalue(ptrbc->res, ptrbc->loopCounter, _countu)); 
                ++ptrbc->worthyOfPolynomialFitProcessing;
                ++ptrbc->countOfGoodEntries;
                //INSERT the values we extracted into table tbl_bucketized_by_tempf_ranges. NOW THAT WE HAVE COPY WORKIN, PERHAPS WE CAN USE COPY RATHER THAN INSERT INTO … BLAH, BLAH …
                ptrbc->rc[SELECTFROMTABLE] = insertBucketizedEnergyValuesIntoTable( ptrbc->values[_avgtemp], ptrbc->values[_avgeu], ptrbc->values[_stddeveu],  ptrbc->values[_mineu], ptrbc->values[_maxeu], ptrbc->thisCount, ptrbc);
                if (ptrbc->rc[SELECTFROMTABLE]) {
                    cerr << "Operation to INSERT INTO tbl_bucketized_by_tempf_ranges  F A I L E D!" << cases[ptrbc->debugFlags.intMyCase] << std::endl;
                    exit_nicely(ptrbc, ptrbc->rc[SELECTFROMTABLE], __LINE__);
                }
            } else { //Come here if fewer than 2 counts
                ++ptrbc->loopCounter;
                continue; //skip this record because it's no good, not enough data for a standard deviation. Go on to do the next record.
            }
        } catch (const std::exception &e) {
            ++ptrbc->countOfBadEntries;
            std::cerr << __FILE__ "2. bad values try/catch detected errors for entry number " << ptrbc->debugFlags.countOfBadEntries + ptrbc->countOfGoodEntries << cases[ptrbc->debugFlags.intMyCase] << std::endl;
        }  //end of try/catch block
        
        ptrbc->buckettemp += BUCKETSIZE;  //Go for the next bucket.
        ++ptrbc->loopCounter;
    }  //End of while loop
    ptrbc->rc[SELECTFROMTABLE] = closePortal(ptrbc , close_Transaction);  //Close myportal before ending the transaction
    ptrbc->rc[SELECTFROMTABLE] = endTransaction(ptrbc , end_Transaction);  //Close myportal before ending the transaction
// =================================================================
    
    if (ptrbc->worthyOfPolynomialFitProcessing > POLYNOMIALDEGREE) {


        try {  //Attempt to read back the data we just inserted into table tbl_bucketized_by_tempf_ranges
            ptrbc->rc[SELECTFROMTABLE] = beginTransaction(ptrbc, begin_Transaction);
            if (ptrbc->rc[SELECTFROMTABLE]) exit_nicely(ptrbc, ptrbc->rc[SELECTFROMTABLE], __LINE__);
            ptrbc->rc[SELECTFROMTABLE] = executeSQL(ptrbc, readBackTable[ptrbc->debugFlags.intMyCase]);  //We might want to order by temp.
            if (ptrbc->rc[SELECTFROMTABLE]) exit_nicely(ptrbc, ptrbc->rc[SELECTFROMTABLE], __LINE__);
            ptrbc->rc[SELECTFROMTABLE] = fetchAllTransaction(ptrbc, fetchALLTransaction);

            if (ptrbc->rc[SELECTFROMTABLE]) {
                if (ptrbc->debugFlags.debug2 ) cout << "FETCH ALL for SELECT statement\n" << readBackTable[ptrbc->debugFlags.intMyCase] << "\nfailed with error: " << PQerrorMessage(ptrbc->conn) << cases[ptrbc->debugFlags.intMyCase] << endl;
            } else {
                ptrbc->numberOfReturnedRows = PQntuples(ptrbc->res);
                ptrbc->numberOfReturnedColumns = PQnfields(ptrbc->res);
            }

            if (ptrbc->numberOfReturnedRows) {
                if (ptrbc->numberOfReturnedRows != ptrbc->worthyOfPolynomialFitProcessing) {
                    std::cerr << "For some unknown reason, the numberOfReturnedRows: " << ptrbc->numberOfReturnedRows << " AND values worthyOfPolynomialFitProcessing: " << ptrbc->worthyOfPolynomialFitProcessing << " have different values. " << cases[ptrbc->debugFlags.intMyCase] << std::endl;
                }
                SetupForMultiFit  *smf = new SetupForMultiFit(ptrbc, POLYNOMIALDEGREE, ptrbc->worthyOfPolynomialFitProcessing, ptrbc->ptrValues);   /* << =================================== */ //First, establish an instance of the SetupForMultiFit object.
                ptrbc->ptrThisIsReallySetupForMultiFit = smf;   //Since BaseClass has difficulty identifying the \
                subordinate SetupForMultiFit class, we'll put the just acquired pointer to what we know is of \
                type SetupForMultiFit into BaseClass of type void *!! I hate playing hop-scotch with the \
                compiler.
                for  (ptrbc->loopCounter = 0, ptrbc->rowBeingProcessed = 0; ptrbc->loopCounter < ptrbc->worthyOfPolynomialFitProcessing; ++ptrbc->loopCounter, ++ptrbc->rowBeingProcessed)
                { //Begin loading the gsl matrices and vectors with values.
                    ptrbc->values[_avgtemp] =atof(PQgetvalue(ptrbc->res, ptrbc->rowBeingProcessed, _avgtemp) ) ;   //Considered the independent Variable
                    ptrbc->values[_avgeu]= atof(PQgetvalue(ptrbc->res, ptrbc->rowBeingProcessed, _avgeu));        //Considered the dependent Variable
                    ptrbc->values[_stddeveu] = atof( PQgetvalue(ptrbc->res, ptrbc->rowBeingProcessed, _stddeveu) ); //Considered the weight.
                    ptrbc->values[_mineu] = atof(PQgetvalue(ptrbc->res, ptrbc->rowBeingProcessed, _mineu));
                    ptrbc->values[_maxeu] =atof(PQgetvalue(ptrbc->res, ptrbc->rowBeingProcessed, _maxeu) );
                    ptrbc->thisCount = atoi(PQgetvalue(ptrbc->res, ptrbc->rowBeingProcessed, _countu));
                    smf->captureIndependentVariableValues( ptrbc->rowBeingProcessed, ptrbc->values );    //Now, put into the object the data that will go into the independent variable matrix (_avgtemp), the dependent variable vector (_avgeu), and the weighting
                    smf->setIntoVector(smf->dependentVariable, ptrbc->rowBeingProcessed, *(ptrbc->values + _avgeu) ); //Put the depenedent variable into its vector.
                    smf->setIntoVector(smf->weights, ptrbc->rowBeingProcessed, 1/( *(ptrbc->values + _stddeveu) * *(ptrbc->values + _stddeveu)  ) ); //Put the weight (stddev) into it's vector. See top 0f page 451 of version 1.12 of GNU Scientific Library Reference Manual (3rd Edition) for the `1/( *(values + _stddeveu) * *(values + _stddeveu)  )` piece of the processing

// NO LONGER NEEDED                    ++rowBeingProcessed;
                } //End of for loop
   //             PQclear(ptrbc->res);  //Get this messag: "WARNING:  there is no transaction in progress" because we closed the myportal cursor and ended the transaction
                ptrbc->rc[SELECTFROMTABLE] = closePortal(ptrbc , end_Transaction);  //Close myportal
//  KILL THE WARNING MESSAGE                ptrbc->rc[SELECTFROMTABLE] = endTransaction(ptrbc, end_Transaction);  //Get another `WARNING:  there is no transaction in progress`

                ptrbc->rc[SELECTFROMTABLE] = smf->doMultiFit( ); //Actually go out and do the gsl_multifit
                ptrbc->rc[SELECTFROMTABLE] = smf->outputPolynomial( "EnergyUsage(T) = ",  cases[ptrbc->debugFlags.intMyCase]);
                ptrbc->floatCovarianceMatrix = new float[NUMBEROFCOVARIANCEMATRIXENTRIES];
                //   UNUSED    VaryingType<float> *tfloat =new VaryingType<float>();  //This tfloat object, for type double, should serve us for making input parameters p2, p3, p4, p5, p7, and p8 in network byte order.
                
                smf->ptrCovarianceMatrixArray = ptrbc->floatCovarianceMatrix; //
                smf->outputCovarianceMatrixAndReturnChi2( "cov = [", cases[ptrbc->debugFlags.intMyCase] );
                std::string strD = to_string<double>(smf->computeTrace(smf->covarienceMatrix->data, 1+PD));

                const std::string tOCM("☜ Trace of Covariance Matrix");
                const std::string newLine("\n");
                smf->stringStreamForOutput << smf->computeCorrelationBetweenIndependentAndDependentVariables( )  << "☜ correlation coefficient" << cases[ptrbc->debugFlags.intMyCase] << newLine;
                smf->stringStreamForOutput << strD  << tOCM << cases[ptrbc->debugFlags.intMyCase]  << newLine;
                smf->stringStreamForOutput << smf->computeCorrelationBetweenIndependentAndDependentVariables( ) << "☜ correlation coefficient" << cases[ptrbc->debugFlags.intMyCase] << "\n";
                ptrbc->squareRootOfSumOfTheSquaresOfTheVarience = smf->computeGoodnessOfResults( );
                ptrbc->averageVariencePerValidReading = ptrbc->squareRootOfSumOfTheSquaresOfTheVarience/ptrbc->grc;
                
                smf->stringStreamForOutput << ptrbc->squareRootOfSumOfTheSquaresOfTheVarience << "☜ Square Root Of Sum Of TheSquares Of The Varience; \n" << ptrbc->averageVariencePerValidReading << "☜ Average Varience Per Valid Reading\n" << ptrbc->averageVariencePerValidReading*100 << "☜ \% Average Varience Per Valid Reading\n"<< ptrbc->grc << "☜ Number of valid meter readings (# observed dependent variables)" <<  cases[ptrbc->debugFlags.intMyCase] << newLine;
/* Right about here is where we want to call a TBD class to insert into the newly (as of 2018-06-14) created table, tbl_poly_fit:
Column |           Type           |                         Modifiers                         | Storage  |                                                    Description
-------+--------------------------+-----------------------------------------------------------+----------+--------------------------------------------------------------------------------------------------------------------
id     | integer                  | not null default nextval('tbl_poly_fit_id_seq'::regclass) | plain    | Prime Key as a monotonically increasing integer;
lt     | timestamp with time zone |                                                           | plain    | In ISO 8601 format: YYYY-MM-DD HH:mm:ss±tz;
pd     | smallint                 | not null default 3                                        | plain    | Polynomial Degree. Default is 3, meaning a 3rd-order polynomial;
coeff  | double precision[]       | not null                                                  | extended | Polynomial's coefficients go here as a single dimentional array. The number of coefficients is 1 tbl_poly_fit.pd;
kind   | meter                    | not null                                                  | plain    | An enumerated type with values of  m1m2kwh, m1kwh, m2kwh, each festooned with single quotes;
correl | double precision         |                                                           | plain    | Correlation coefficient of the polynomial. May be ±;
chi2   | double precision         |                                                           | plain    | Polynomial's chi-squared value.
Indexes: "tbl_poly_fit_pkey" PRIMARY KEY, btree (id)
*/
            } else {
                std::cerr << "Although the SELECT seems to have worked, we got " << ptrbc->numberOfReturnedRows << " rows back from the table which we thought we just inserted data into!" << cases[ptrbc->debugFlags.intMyCase] << std::endl;
            }
        } catch (const std::exception &e) {
            std::cerr << "Some how we failed to get any data back from the table which we thought we just inserted data into!" << cases[ptrbc->debugFlags.intMyCase] << std::endl;
        }
 
        
        //        rc = polynomialFit(POLYNOMIALDEGREE, grc, awv->temperatureVector, awv->energyUsageVector,  NULL, NULL,  &TSS, &goodnessOfFit);
    } else {
        std::cerr << "Insufficient number of VALID datapoints to do MultiFit processing for desired polynomial of degree " << POLYNOMIALDEGREE << cases[ptrbc->debugFlags.intMyCase] << std::endl;
    }
//Put into tbl_poly_fit the polynomial coefficients, the meter id, the correlation coeffincient, and the χ-squared value resulting from the above computations. We will not, at this time, store the covarience matricies 16 elements into tbl_poly_fit because of ignorence on my part of usefulness of these data from the covarience matrix.
 //  UNUSED   const short polynomialDegree=POLYNOMIALDEGREE;
 //  UNUSED   const short myCase = ptrbc->debugFlags.intMyCase;
    // insert code here...
    //long doubles are 16 bytes long
    //doubles are 8 bytes long
    //floats are 4 bytes long
    //shorts are 2 bytes long

 //   TblPolyFitWithCov *ptrTPF = new TblPolyFitWithCov;
    SetupForMultiFit *const sumf = (SetupForMultiFit *)ptrbc->ptrThisIsReallySetupForMultiFit;
    TblPolyFitWithCov *pfWC = new TblPolyFitWithCov( ptrbc  );
    for (ptrbc->row=0; ptrbc->row < 1+POLYNOMIALDEGREE; ptrbc->row++) {  //Get the coefficients
         *(pfWC->copyOfCoefficients +ptrbc->row) = sumf->getCoefficient(ptrbc->row); //One coefficient at a time.
    }
    for (ptrbc->row=0; ptrbc->row < 1+POLYNOMIALDEGREE; ptrbc->row++) {
        for (ptrbc->column=0; ptrbc->column < 1+POLYNOMIALDEGREE; ptrbc->column++) {
            *(pfWC->ptrToCovarianceMatrix+ ptrbc->row * sizeof(float) + ptrbc->column) = *(sumf->ptrCovarianceMatrixArray + sumf->row * sizeof(float) + sumf->col); //Move the covariance data over to the TblPolyFitWithCov Class
        }
    }
    pfWC->correlationCoefficient = sumf->getCorrelationCoefficient();
    pfWC->chi2 = sumf->getChi2();
    //int doInsertInto(const char *, int *, double *, double *, double *, double *, smallint *, double *, double *)
//χ-squared value --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
//Correlation Coefficient ------------------------------------------------------------------------------------------------------------------------------------------------------------+                       |
//Meter Descriptor --kind=0=>m1m2kwh; kind=1=>m1kwh; kind=2=>m2kwh---------------------------------------------------------------------------------------------------+                |                       |
//coefficient C3 --------------------------------------------------------------------------------------------------------------------------------------+             |                |                       |
//coefficient C2 ------------------------------------------------------------------------------------------------------------------+                   |             |                |                       |
//coefficient C1 ----------------------------------------------------------------------------------------------+                   |                   |             |                |                       |
//coefficient C0 ---------------------------------------------------------------------------+                  |                   |                   |             |                |                       |
//short int showing polynomial degree ----------------------------------+                   |                  |                   |                   |             |                |                       |
//Base Class or ConnString --------------------+                        |                   |                  |                   |                   |             |                |                       |
    //                                          V                        V                   V                  V                   V                   V             V                V                       V
    //    int doInsertInto(short *, double *, double *, double *, double *, float *, short *, double *, double *);
    ptrbc->rc[SELECTFROMTABLE] = pfWC->doInsertInto(ptrbc->lookAtMyConnectionString );
    if(ptrbc->rc[SELECTFROMTABLE]) {
        std::cout << "Some kind of problem in TblPolyFit::doInsertInto, got a return code of " << ptrbc->rc[SELECTFROMTABLE] << std::endl;
    } else {
        std::cout << "Successful call to TblPolyFit::doInsertInto, got a return code of " << ptrbc->rc[SELECTFROMTABLE] << std::endl;
    }
    delete [] ptrbc->floatCovarianceMatrix;

    delete pfWC;
    if (ptrbc->sql != nullptr) delete [] ptrbc->sql;
    if (ptrbc->copyBuffer != nullptr) delete [] ptrbc->copyBuffer;
    if (ptrbc->subCharBuffer != nullptr) delete [] (ptrbc->subCharBuffer);   //c++'s Equivalent to C's free when we allocate storage with a calloc/malloc. Note the use of [] prior to the (subCharacterBuffer);
    if (ptrbc->sqlbuffer != nullptr) delete[](ptrbc->sqlbuffer);       //Delete another dynamically acquired character buffer.
    if (a_ff != nullptr)  {
        a_ff->~Align();
//        delete a_ff;
    }
    if ( af != nullptr) {
         af->~Align();
//        delete af;
    }
//    UPDATE tbl_energy_usage set avtempf = (SELECT avg(temperature) FROM tbl_l WHERE siteid=5 AND lt BETWEEN '2018-05-06 08:17:00' AND '2018-05-07 07:00:00' ), avtempk=SELECT (5*(avg(temperature)-32)/9+273.15 ) FROM tbl_l WHERE  siteid=5 AND lt BETWEEN '2018-05-06 08:17:00' AND '2018-05-07 07:00:00' ), siteid=5 WHERE id=2877;
    PQfinish(ptrbc->conn);  //Since this routine did the connection, this  routine must do the PQfinish to disconnect from the database.
}
