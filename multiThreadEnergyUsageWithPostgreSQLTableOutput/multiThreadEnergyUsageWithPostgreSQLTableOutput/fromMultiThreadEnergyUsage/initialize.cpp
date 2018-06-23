#include "baseClass.hpp"
void initializeBaseClass (BaseClass *, BaseClass *);
void initializeBaseClass (BaseClass *bc, BaseClass *bcCopy) {
    bcCopy->clDatabaseName = bc->clDatabaseName;
    bcCopy->results_File = bc->results_File;
    bcCopy->debugFlags.useKelvin = bc->debugFlags.useKelvin;
    bcCopy->clUserID = bc->clUserID;
    bcCopy->debugFlags.mycase = bc->debugFlags.mycase;
    bcCopy->debugFlags.dependentVariableColumn = bc->debugFlags.dependentVariableColumn;
    bcCopy->debugFlags.includeM2InAverages = bc->debugFlags.includeM2InAverages;
    bcCopy->debugFlags.includeM1InAverages = bc->debugFlags.includeM1InAverages;
    bcCopy->clPW = bc->clPW;
    bcCopy->clHostID = bc->clHostID;
    bcCopy->clPortID = bc->clPortID;
    bcCopy->clSQL = bc->clSQL;
    bcCopy->debugFlags.debug1 = bc->debugFlags.debug1;
    bcCopy->debugFlags.debug2 = bc->debugFlags.debug2;
    bcCopy->debugFlags.debug3 = bc->debugFlags.debug3;
}

