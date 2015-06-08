/*
 * genericDB_test.cc
 *
 *  Created on: Jun 19, 2013
 *      Author: nerses@jlab.org
 */

#include "genericDB.hh"
int main(){
	GenericDB db;

	db.addDetector("CDC", "SQLite://tt.db");
	db.addDetector("FDC", "SQLite://tt.db");
	db.getCrates();
	return(0);
}




