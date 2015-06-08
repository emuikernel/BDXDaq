/*
 * genericDB.hh
 *
 *  Created on: Jun 19, 2013
 *      Author: nerses
 */
//#ifndef GENERICDB_HH_
//#define GENERICDB_HH_

//#include "HVCAENx527.h"
#include <string>
#include <vector>
#include <map>
#include <QtSql/QtSql>

using namespace std;

class GenericDB {
private:
	static const int MAX_HIERERCHY_LEVELS; // A hard limit to query the hierarchy levels in the Detector_Hierarchy table
	vector<string> det;			// Detector name
	map<string, map<string, string> > cr;		// Map containing the crate names of current detector
	map<string, map<string, string> > allCr;	// Map containing the crate names of all detectors (this seems unused).
	string getChannelName(string channel_id);
public:
	 GenericDB(){}		// Create the object
	~GenericDB(){}		// Delete the object
	void addDetector(const char* detector, const char* URI);		// Parse URI and Open the connection to the DB
	map<string, map<string, string> > getCrates(const char* detector = "");	// Get the List of Crates  and store them in the HVCRATE Crate[]
	map<int, map<string, string> >  getChannels();	// Get the List of Channels and store them in the HVCRATE Crate[]
	vector<string>  getDetectors();	// Get the List of detectors

};

//#endif /* GENERICDB_HH_ */
