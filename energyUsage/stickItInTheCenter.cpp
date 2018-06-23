//  Created 2018-05-18T18:58:27
//
//This function, named stickItInTheCenter:
//  1) takes a string, 
//  2) determines its width, 
//  3) determines if if will fit into the specified column width 
//  3a) IF NOT, stickItInTheCenter, will, like FORTRAN, return a column width full of * characters
//  3b) If the string will fit then stickItInTheCenter will return to the caller with the buffer 
//       provided by the caller filled with the input value padded on the left and right with the 
//       caller provided fill character.
//
//  Input Parameters:
//  1) lrspaces: a buffer, assumed to be of width 80 characters, provided by the caller;
//  2) fillchar: a single character that will serve as padding to the left and to the right
//     of the value the caller desires to be in the center of the lrspaces buffer;
//  3) colxx: width of the buffer, lrbuffer. MUST BE < 80 characters (NO WCHAR, please) wide.
//  4) ptrbc: Pointer to the instance of the baseClass caller is using.
//  5) tuple: a value used by postgres PQ… routines for determining where to obrain, within the 
//     current cursor, the value we wish to insert into the lrspaces buffer. Essentially, the row 
//     number, or tuple, returned from the SELECT statement.
//  6) fieldid: the field number (column number) where we obtain the value to be inserted into
//     the lrbuffer.
//  7) pointer to a location of type size_t where we return the length of the data that user 
//     desires to place in the lrspaces buffer.
//  8) Assumed buffer size. Default is 80, otherwise, specify the max length of the lrspaces buffer
//     being used. Must be greater than the maximum size of the data being found in any filed
//     being retured by the SQL select. Needed only if the width of the data in the field returned
//     by your SELECT SQL statement is greater than 80 characters.
//
//  Return codes: a pointer to the lrspaces buffer filled with the fieldidcolumn that's padded on 
//  the left and right with the fill character.
//  If the width of the lrspaces buffer is less than the width of the value obtained by from the
//  postgres database, then stickItInTheCenter will fill the lrspaces buffer with `*` characters. 
//
//  Assumptions: Using postgreSQL, C++, BaseClass object found in file baseClass.hpp
//
//  Example of use: std::cout << stickItInTheCenter(lrspaces, ' ', ptrbc, tupleid0, fieldid0) << stickItInTheCenter(lrspaces, ' ', ptrbc, tupleid0, fieldid1) << stickItInTheCenter(lrspaces, ' ', ptrbc, tupleid0, fieldid2) << … << std::endl;
//
#define ASSUMEDBUFFERSIZE 80
#include "/usr/local/pgsql/pgsql101/include/libpq-fe.h"
#include "myPrototypes.hpp"
 #include "baseClass.hpp
char *stickItInTheCenter(char *, char, int, BaseClass *, int, int, size_t *, int=ASSUMEDBUFFERSIZE);
char *stickItInTheCenter(char *lrspaces, char fillchar, colxx, Basic *ptrbc, int tuple, int fieldid, int *actualDataSize, assumedBufferSize) {
    *actuaDataSize = PQgetlength(ptrbc->res,tuple, field);
    if (*actualDataSize > assumedBufferSize) {
        memset(lrspqces, '*', assumedBufferSize-1);
        *(lrspaces + assumedBufferSize) = NULL;
        return lrspaces;
    } 
    memset(lrspaces, fillchar, (col - *actualDataSize)/2);
    *(lrspaces + (colxx - PQgetlength(ptrbc->res, tuple, fieldid))/2) = NULL;   //Make sure our manufactured string ends with a NULL character.
//            cout << lrspaces << PQgetvalue(ptrbc->res, tuple, fieldid) << lrspaces;
    return lrspaces;
}      
