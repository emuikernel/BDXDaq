/*
 * detector.c
 *
 *  Created on: Jun 24, 2013
 *      Author: nerses@jlab.org
 */

#include <string>
#include <iostream>
#include <sstream>
using namespace std;
#include "detector.hh"

#include <stdlib.h>
#include <string.h>
#include <epicsExport.h>
#include <iocsh.h>

string prefix;

extern "C" {
#include "HVCAENx527.h"


/* ------------------------------------------------------------ */

/* Information needed by iocsh */
static const iocshArg     detUsePrefixArg0 = {"Prefix",   iocshArgString};
static const iocshArg    *detUsePrefixArgs[] = {
		&detUsePrefixArg0,
};
static const iocshFuncDef detUsePrefixFuncDef = {"detUsePrefix", 1, detUsePrefixArgs};

/* Wrapper called by iocsh, selects the argument types that function needs */
static void detUsePrefixCallFunc(const iocshArgBuf *args) {
	if (args[0].sval == NULL) {
		prefix = "";
	} else {
		prefix = args[0].sval;
	}
}

/* Registration routine, runs at startup */
static void detUsePrefixRegister(void) {
	iocshRegister(&detUsePrefixFuncDef, detUsePrefixCallFunc);
}

/* ------------------------------------------------------------ */

void detConfigureCrate(const char *det, const char* uri){

	db->addDetector(det, uri);

	map<string, map<string, string> >  cr = db->getCrates(det);

	for (std::map<string, map<string, string> >::iterator it=cr.begin();  it != cr.end(); ++it) {
		string n = (string)(*it).first;
		string tmp = prefix;
		tmp += n;
		char* name = (char*)tmp.c_str();
		map<string, string> prop = (*it).second;
//		for (std::map<string, string>::iterator it2=prop.begin();  it2 != prop.end(); ++it2) {
		char* host = (char*)prop["HOST"].c_str();
		string type = prop["TYPE"];
		cout<<__FILE__<<"::"<<__FUNCTION__<<"::"<<__LINE__
				<<" name = "<<name<<" host = "<<host<<" n = "<<n<<" type = "<<type<<endl;
		if (type.compare(0,2,"HV")  ==0) CAENx527ConfigureCreate(name, host, NULL, NULL);
		if (type.compare(0,7,"BIAS/LV")  ==0) drvMPODConfig(host, name, 10);
//		if (name[0] == 'h') CAENx527ConfigureCreate(name, host, NULL, NULL);
//		if (name[0] == 'l' || name[0] == 'b') drvMPODConfig(host, name, 10);
	}

	return;
}

/* ------------------------------------------------------------ */

/* Information needed by iocsh */
static const iocshArg     detConfigureCrateArg0 = {"Detector", iocshArgString};
static const iocshArg     detConfigureCrateArg1 = {"URI",      iocshArgString};
static const iocshArg    *detConfigureCrateArgs[] = {
		&detConfigureCrateArg0,
		&detConfigureCrateArg1
};
static const iocshFuncDef detConfigureCrateFuncDef = {"detConfigureCrate", 2, detConfigureCrateArgs};

/* Wrapper called by iocsh, selects the argument types that function needs */
static void detConfigureCrateCallFunc(const iocshArgBuf *args) {
	detConfigureCrate( args[0].sval, args[1].sval );
}

/* Registration routine, runs at startup */
static void detConfigureCrateRegister(void) {
	iocshRegister(&detConfigureCrateFuncDef, detConfigureCrateCallFunc);
}

/* Will dynamically load DBes according to the boards discovered on the Crate */
void detDbLoadRecords(){

	if (prefix.size()==0) prefix="";
	CAENx527DbLoadRecords(prefix.c_str());
	cout<<"CAENx527DbLoadRecords() was called"<<endl;
	dbLoadRecordsMPOD(prefix.c_str());
	cout<<"dbLoadRecordsMPOD() was called"<<endl;

	map<int, map<string, string> > channels = db->getChannels();
//	cout<<"channels.size() = "<<channels.size()<<endl;
	vector<string> dets = db->getDetectors();
	for (map<int, map<string, string> >::iterator it = channels.begin(); it != channels.end(); ++it) {

		string pars;	// parameters to be sent dbLoadRecords()
		string DETch;	// DETch from map (basic part of PV name, w/out prefix and suffix)
		string type;	// type = {hv, lv bias}

		// fill the pars
		map<string, string> items = it->second;
		for (map<string, string>::iterator items_it = items.begin(); items_it != items.end(); ++items_it) {

			PDEBUG(1) cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__
					<<" items_it->first = "<<items_it->first
					<<" items_it->second = "<<items_it->second<<endl;
			if (pars.size()>0) pars += ","; // The first entry doesn't need ","
			pars += items_it->first;
			pars += "=";
			pars += items_it->second;
			if (items_it->first.compare("DETch") == 0) DETch = items_it->second;
			if (items_it->first.compare("TYPE")  == 0) type  = items_it->second;
		}
		// insert the prefix
		int idx;
		idx = pars.find("PSNAME="); pars.insert(idx+7, prefix);
		idx = pars.find("DETch=");  pars.insert(idx+6, prefix);
		// NOTE: idx is DETch= place it is going to be used
		PDEBUG(1) cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" pars(+prefix) = "<<pars<<endl;

		// If a variable name does not match any detectors from configured list then skip!
		string chname = pars.substr(idx, pars.find(",",idx)-idx);
		string chpattern;
		// This block is to skip the detectors not in the dets vector.
		bool isDetInTheList = false;
		for(unsigned int iv = 0; iv<dets.size(); ++iv) {
			chpattern  = "DETch=";
			if (prefix.size()>0) chpattern += prefix;
			chpattern += dets.at(iv);
			chpattern += ":";
			if(chname.find(chpattern)!=std::string::npos) isDetInTheList =  true;
//			cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" chname = "<<chname<<" chpattern = "<<chpattern<<" isDetInTheList = "<<isDetInTheList<<endl;
		}
		if (!isDetInTheList) continue;

		// find the channel type hv/lv/bias and act accordingly
		int ibegin = DETch.find(":", 0);
//		PDEBUG(1)
		cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" it->first = "<<it->first<<" DETch = "<<DETch<<" pars = "<<pars<<" ibegin = "<<ibegin<<" DETch(ibegin,) = "<<DETch.substr(ibegin)<<endl;
		if (type.compare(0,2,"hv") == 0) {
			dbLoadRecords("db/DetCh.db",     pars.c_str());
//			PDEBUG(1)
			cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<": Load the db 'db/DetCh.db' with args = "<<pars.c_str()<<endl;
			if (DETch.compare(0,4,"FDC:") == 0) { // special interlocks for FDC
				dbLoadRecords("db/DetChItLk_FDC.db", pars.c_str());
			} else {
				dbLoadRecords("db/DetChItLk.db", pars.c_str());
			}
		} else if (type.compare(0,2,"lv") == 0 || type.compare(0,4,"bias") == 0) { // load lv channels DBes
			/*
			PORT=lv55,HOST=halldmpod1,COMMUNITY=guru,SCAN=2 second,CH=715
			+ CH       <==> DETch
			+ COMMUNITY == "guru"
			+ HOST      == hostname
			+ PORT     <==> PSNAME
			+ SCAN      == "2 second"
			- ADDR     <==> SLOT
			- MBB       == {"ZR", "ON", ... , "FF"}
			mpv_8008l.db with arguments PORT=lv55,HOST=halldmpod1,COMMUNITY=guru,SCAN=2 second,CH=100
			ehs_f_205pf.db with arguments PORT=lv55,HOST=halldmpod1,COMMUNITY=guru,SCAN=2 second,CH=700
			DETch=hv:CDC:A:1,BOARD=A1550P,CHANNUM=0,PSNAME=hv35,SLOT=1
			*/

			string board;
			int slot;
			int ch;
			int idx1, idx2;

			idx1 = pars.find("BOARD=")+6;
			idx2 = pars.find(",", idx1);
			if (idx2<0) idx2=pars.size();
//			cout<<"BOARD idx1,2 = "<<idx1<<" "<<idx2<<" pars = "<<pars;
			board = pars.substr(idx1, idx2-idx1);
//			cout<<" BOARD = "<<board<<endl;

			idx1 = pars.find("SLOT=")+5;
			idx2 = pars.find(",", idx1);
			if (idx2<0) idx2=pars.size();
//			cout<<"SLOT idx1,2 = "<<idx1<<" "<<idx2<<" pars = "<<pars;
			slot = atoi(pars.substr(idx1, idx2-idx1).c_str());
//			cout<<" SLOT = "<<slot<<endl;

			idx1 = pars.find("CHANNUM=")+8;
			idx2 = pars.find(",", idx1);
			if (idx2<0) idx2=pars.size();
//			cout<<"CHANNUM idx1,2 = "<<idx1<<" "<<idx2<<" pars = "<<pars;
			ch   = atoi(pars.substr(idx1, idx2-idx1).c_str());
//			cout<<" CHANNUM = "<<ch<<endl;

			stringstream s;
			s << slot*100 + ch;
			pars += ",CH=" + s.str();

			// replace PSNAME => PORT
			idx1 = pars.find("PSNAME=");
			pars.replace(idx1, 7, "PORT=");
			// replace bias => lv for PSNAME argument (i.e. for hardware names)
//			idx2 = pars.find("bias", idx1-1);
//			cout<<"replace bias => lv idx1,2 = "<<idx1<<" "<<idx2<<" pars = "<<pars<<endl;
//			if (idx2>=0) pars.replace(idx2, 4, "");
//			idx2 = pars.find("lv", idx1-1);
//			cout<<"replace bias => lv idx1,2 = "<<idx1<<" "<<idx2<<" pars = "<<pars<<endl;
//			if (idx2>=0) pars.replace(idx2, 2, "");

			PDEBUG(1) cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" it->first.c_str() = "<<it->first<<" pars = "<<pars<<endl;
			if (board.compare("MPV8008L") == 0) dbLoadRecords("db/DetMpv_8008l.db",   pars.c_str());
			if (board.compare("MPV8030L") == 0) dbLoadRecords("db/DetMpv_8030l.db",   pars.c_str());
			if (board.compare("MPV8030") == 0) dbLoadRecords("db/DetMpv_8030.db",   pars.c_str());
			if (board.compare("ISEG")     == 0) dbLoadRecords("db/DetEhs_f_205pf.db", pars.c_str());
		}
	}
	return;
}

//static const iocshArg     detDbLoadRecordsArg0 = {"Prefix", iocshArgString};
//static const iocshArg    *detDbLoadRecordsArgs[] = { &detDbLoadRecordsArg0 };
static const iocshFuncDef detDbLoadRecordsFuncDef = {"detDbLoadRecords", 0, NULL}; //detDbLoadRecordsArgs};

/* Wrapper called by iocsh, selects the argument types that function needs */
static void detDbLoadRecordsCallFunc(const iocshArgBuf *args) {
  detDbLoadRecords();
}

/* Registration routine, runs at startup */
static void detDbLoadRecordsRegister(void) {
    iocshRegister(&detDbLoadRecordsFuncDef, detDbLoadRecordsCallFunc);
}

/*
 * Call the constructor of GenericDB at startup
 */
static int GenericDBInit() {
	if (db == NULL) db = new GenericDB();
	return 0;
}
static int done = GenericDBInit();

epicsExportRegistrar(detUsePrefixRegister);
epicsExportRegistrar(detConfigureCrateRegister);
epicsExportRegistrar(detDbLoadRecordsRegister);

}; // The end of extern "C"

