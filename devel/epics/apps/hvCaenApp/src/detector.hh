/*
 * detector.hh
 *
 *  Created on: Jun 24, 2013
 *      Author: nerses
 */

#ifndef DETECTOR_HH_
#define DETECTOR_HH_

#include "genericDB.hh"
#include "drvMPOD.hh"
#include "drvGlobalMPOD.hh"

#ifdef __cplusplus
extern "C" {


static GenericDB* db;
extern string prefix;

void detConfigureCrate(const char *det, const char* uri);
void detDbLoadRecords();

//void CAENx527ConfigureCreate(char *name, char *addr, char *username = NULL, char* password = NULL);
//void CAENx527DbLoadRecords();
//epicsShareFunc int epicsShareAPI dbLoadRecords(const char* filename, const char* substitutions);

}

#endif

//GenericDB* db;


#endif /* DETECTOR_HH_ */
