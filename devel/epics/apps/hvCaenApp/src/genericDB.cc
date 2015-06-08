/*
 * genericDB.cc
 *
 *  Created on: Jun 19, 2013
 *      Author: nerses@jlab.org
 */

#include "genericDB.hh"

#include <algorithm>
#include <iostream>
//using namespace std;

/*
 * Constructor and destructor
 */
//GenericDB::GenericDB(){
////	cr = new map<string, string>;
//}
//GenericDB::~GenericDB(){
////	delete cr;
////	cr = 0;
//}
const int GenericDB::MAX_HIERERCHY_LEVELS = 20;
/*
 * Parse URI and Open the connection to the DB.
 * Run the query and put acquired info into cr map.
 */
void GenericDB::addDetector(const char* detector, const char* URI){

	std::string uri;			// URI of current detector

	// Verify if the detector group is defined

	//int idet = det.capacity();
	int idet = det.size();
	det.push_back(string(detector));
	cout<<"idet = "<<idet<<" det[idet] = "<<det[idet]<<endl;
	std::transform(det[idet].begin(), det[idet].end(), det[idet].begin(), ::toupper);
	// Parse the URI
	uri.append(URI);
	string::size_type pos = uri.find("://");
	string protocol = uri.substr(0,pos);
	std::transform(protocol.begin(), protocol.end(), protocol.begin(), ::tolower);

	string resource = uri.substr(pos+3);
	// cout<<"uri = "<<uri<<" protocol = "<<protocol<<" resource = "<<resource<<endl;

	// Open appropriate DB
	if (protocol.compare("sqlite")==0) {
		cout<<"The DB protocol is SQLite - "<<protocol<<endl;
		QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
		//db.setHostName("acidalia");
		db.setDatabaseName(resource.c_str());
		//db.setUserName("mojito");
		//db.setPassword("J0a1m8");

		if(!db.open()) {
			cerr << "Can't open database: " << uri << endl;
			cerr << db.lastError().text().toStdString() << endl;
			exit(1);
		}
	} else if (protocol.compare("mysql")==0){
		cout<<"The DB protocol is MySQL -  "<<protocol<<endl;
	} else {
		cerr<<"Unsupported DB protocol - "<<protocol<<endl;
		exit(1); // FIXME::Might be a status should be risen and program should continue without this part
	}
	for (std::vector<string>::iterator it = det.begin(); it != det.end(); ++it) {
		QString q = "SELECT "
				"Crate.crateid  as crid,       Crate.name  as crname,"
				"Crate.area     as crarea,     Crate.rack  as crrack,"
				"Crate.location as crlocation, Crate.SN    as crSN,"
				"Crate.Function as crFunction, Crate.rocid as crrocid,"
				"Crate.host     as crhost,     Crate.IP as crIP "
				"FROM Channel JOIN "
				"Module ON Channel.moduleid = Module.moduleid,"
				" Crate ON  Module.crateid  =  Crate.crateid "
				"WHERE (Channel.col_name='hv_chanid' or Channel.col_name='lv_chanid' or Channel.col_name='bias_chanid')"
				" and system='%1' GROUP BY crid";
//		QString q = "SELECT "
//				"Crate.crateid  as crid "
//				"FROM Channel JOIN "
//				"Module ON Channel.moduleid = Module.moduleid,"
//				" Crate ON  Module.crateid  =  Crate.crateid "
//				"WHERE col_name='hv_chanid' and system='%1' GROUP BY crid";
		qWarning()<<q.arg((*it).c_str())<<endl;
		QSqlQuery query(q.arg((*it).c_str()));
		cout<<"The query executed"<<endl;

		cr.clear();	// Reset the map, so it will have only the mainframe info for current map
		while (query.next()) {

			string key;
//			if ( query.value(6).toString().toStdString().compare("HV") == 0 ) {
//				key = "hv";
////				cr[key] = "halldcaenhv3";
////				allCr[key] = "halldcaenhv3"; // All crates from all detectors go into this map
//			} else {
//				key = "lv";
////				cr[key] = "halldmpod2";
////				allCr[key] = "halldmpod2"; // All crates from all detectors go into this map
////				cr[key] = "129.57.194.19";
////				allCr[key] = "129.57.194.19"; // All crates from all detectors go into this map
//			}
			//			key += query.value(0).toString().toStdString();			// crid
						key = query.value(1).toString().toStdString();			// crate name
			cr[key]["HOST"] = query.value(8).toString().toStdString();		// host
			allCr[key]["HOST"] = query.value(8).toString().toStdString();	// host
			cr[key]["TYPE"] = query.value(6).toString().toStdString();		// Function
			allCr[key]["TYPE"] = query.value(6).toString().toStdString();	// Function
			qWarning()<<query.value(0).toString();
			cout<<"key = "<<key<<endl;
		}
	}
	return;
}

/*
 * Get the map of crates
 */
map<string, map<string, string> > GenericDB::getCrates(const char* det){
//	cout << "Getting Crates" << endl;
//	for( map<string,string>::iterator it=cr.begin(); it != cr.end(); it++ ) {
//		cout << "Key " << it->first << "  , Value is " << it->second << endl;
//	}
	return cr;
}
/*
 * Get the vector of detectors
 */
vector<string> GenericDB::getDetectors(){
//	cout << "Getting Crates" << endl;
//	for( map<string,string>::iterator it=cr.begin(); it != cr.end(); it++ ) {
//		cout << "Key " << it->first << "  , Value is " << it->second << endl;
//	}
	return det;
}
/*
 * Get the List of Channels and store them in the HVCRATE Crate[]
 */
map<int, map<string, string> >  GenericDB::getChannels(){

	map<int, map<string, string> > channels;
	QSqlQuery query("SELECT "
			"Crate.crateid  as crid,       Crate.name  as crname,"
			"Crate.area     as crarea,     Crate.rack  as crrack,"
			"Crate.location as crlocation, Crate.SN    as crSN,"
			"Crate.Function as crFunction, Crate.rocid as crrocid, "

			"Module.moduleid as mid,   Module.crateid as mcrateid,"
			"Module.slot     as mslot, Module.type    as mtype, "
			"Module.SN       as mSN, "

			"Channel.chanid,  Channel.moduleid, Channel.name,"
			"Channel.channel, Channel.system,   Channel.col_name "

			"FROM Channel JOIN "
			"Module ON Channel.moduleid = Module.moduleid, "
			" Crate ON  Module.crateid  =  Crate.crateid "
			"WHERE col_name='hv_chanid' or col_name='lv_chanid' or col_name='bias_chanid'");
	qWarning()<<query.executedQuery();
//    qDebug() << query.lastError();
//    query.first();
//    0	Crate.crateid as crid
//    1	Crate.name as crname
//    2	Crate.area as crarea
//    3	Crate.rack as crrack
//    4	Crate.location as crlocation
//    5	Crate.SN as crSN
//    6	Crate.Function as crFunction
//    7	Crate.rocid as crrocid
//    8	Module.moduleid as mid
//    9	Module.crateid as mcrateid
//   10	Module.slot as mslot
//   11	Module.type as mtype
//   12	Module.SN as mSN
//   13	Channel.chanid
//   14	Channel.moduleid
//   15	Channel.name
//   16	Channel.channel
//   17	Channel.system
//   18	Channel.col_name
	int i=0;
	while (query.next()) {
		string::size_type pos = query.value(18).toString().toStdString().find("_");	//   18	Channel.col_name
		string type    = query.value(18).toString().toStdString().substr(0,pos);	// hv or lv from Channel.col_name
		string DETch   = query.value(15).toString().toStdString();					// 15 Channel.name
		string DET	   = query.value(17).toString().toStdString();					// 17 Channel.system
//		string PSNAME  = type + query.value( 0).toString().toStdString();			// 0 Crate.crateid as crid
		string PSNAME  = query.value( 1).toString().toStdString();			// 0 Crate.crateid as crid
		string SLOT    = query.value(10).toString().toStdString();					// 10 Module.slot   as mslot
		string CHANNUM = query.value(16).toString().toStdString();					// 16 Channel.channel
		string BOARD   = query.value(11).toString().toStdString();					// 11 Module.type
//		string name = type + ":" + getChannelName(query.value(13).toString().toStdString()); // 13 Channel.chanid
		string name = getChannelName(query.value(13).toString().toStdString()); 	// 13 Channel.chanid
		channels[i]["DETch"]  = name;
		channels[i]["PSNAME"]  = PSNAME;
		channels[i]["SLOT"]    = SLOT;
		channels[i]["CHANNUM"] = CHANNUM;
		channels[i]["BOARD"]   = BOARD;
		channels[i]["TYPE"]   = type;

//		cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__
//				<<   " type = "<<type
//				<<   " name = "<<name
//				<< " PSNAME = "<<PSNAME
//				<<   " SLOT = "<<SLOT
//				<<" CHANNUM = "<<CHANNUM
//				<<  " BOARD = "<<BOARD
//				<<endl;
		i++;
	}
	query.clear();
//	query.finish();

	return(channels);
}

string GenericDB::getChannelName(string channel_id){
	string name;

	QString q("SELECT parent_id,name FROM Detector_Hierarchy WHERE chanid = '%1'");
	QSqlQuery query(q.arg(channel_id.c_str()));
	query.next();
//	qWarning()<<"GenericDB::getChannelName() "<<q.arg(channel_id.c_str())<<" query.size() = "<<query.size()<<" query.value(0).isNull() = "<<query.value(0).isNull()<<endl;
	QString id;
	q = "SELECT parent_id,name FROM Detector_Hierarchy WHERE id = '%1'";
	int i = 0;
	while (!query.value(0).isNull() && i<MAX_HIERERCHY_LEVELS) {
		name.insert(0, ":");										// Prepend delimiter
		name.insert(0, query.value(1).toString().toStdString());	// Prepend upper hierarchy name
		id = query.value(0).toString();					// Get ID
		query.clear();
		query.exec(q.arg(id));							// Query the parent
		query.next();									// Just one row must be enough, since each item must have one parent.
		//qWarning()<<q.arg(id)<<" query.size() = "<<query.size()<<" query.value(0).isNull() = "<<query.value(0).isNull()<<" i = "<<i<<endl;
		i++;
	}
	name.insert(0, ":");										// Prepend delimiter
	name.insert(0, query.value(1).toString().toStdString());	// Prepend upper hierarchy name
	name.erase(name.find_last_of(':'));
	query.clear();
	return name;
}
