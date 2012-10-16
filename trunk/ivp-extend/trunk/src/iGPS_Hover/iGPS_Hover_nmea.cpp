/*
 * iGPS_Hover_nmea.cpp
 *
 *  Created on: Oct 13, 2012
 *      Author: josh
 */


#include "simple_gps.h"
#include "MBUtils.h"

using namespace std;
using namespace boost::posix_time;

void SIMPLE_GPS::parseGPRMC(string sNMEAString) {
//	cout << "rmc: " << sNMEAString << endl;
	bool bGood = true;

	string sTmp;

	// field 1 - UTC time
	sTmp = MOOSChomp(sNMEAString, ",");
	if ( sTmp.size() == 0 ) return;
	string sTime = sTmp;
//	double dfTime = atof( sTmp.c_str() );

	// field 2 - fix status
	sTmp = MOOSChomp(sNMEAString, ",");
	if ( sTmp == "A" ) m_Comms.Notify("GPS_LOCK", "true");
	else m_Comms.Notify("GPS_LOCK", "false");

	// field 3 - latitude
	sTmp = MOOSChomp(sNMEAString, ",");

	// field 4 - N/S
	sTmp = MOOSChomp(sNMEAString, ",");

	// field 5 - longitude
	sTmp = MOOSChomp(sNMEAString, ",");

	// field 6 - E/W
	sTmp = MOOSChomp(sNMEAString, ",");

	// field 7 - speed
	sTmp = MOOSChomp(sNMEAString, ",");
	double dfSpeed = atof( sTmp.c_str() );
	m_Comms.Notify("GPS_SPEED", dfSpeed);

	// field 8 - course
	sTmp = MOOSChomp(sNMEAString, ",");
	double dfCourse = atof( sTmp.c_str() );
	m_Comms.Notify("GPS_HEADING", dfCourse);

	// field 9 - date
	sTmp = MOOSChomp(sNMEAString, ",");
	if ( sTmp.size() != 6 ) return;
//	double dfDate = atof( sTmp.c_str() );
	string sDate = sTmp;

	// confirm posix date construction
	string modified_date = "20" + sDate.substr(4,2) + sDate.substr(2,2) + sDate.substr(0,2);
	string composite = modified_date + "T" + sTime;
	try {
		ptime t( from_iso_string(composite) );
		m_Comms.Notify("GPS_PTIME", to_simple_string(t));
		double seconds = t.time_of_day().total_milliseconds()/1000.0;
		m_Comms.Notify("GPS_TIME_SECONDS", seconds);
	} catch ( ... ) {
		cout << "Exception constructing ptime" << endl;
	}
}

void SIMPLE_GPS::parseGPGGA(string sNMEAString) {
//	cout << "gga: " << sNMEAString << endl;
	bool bGood = true;

	string sTmp;

	// field 1 - UTC time
	sTmp = MOOSChomp(sNMEAString, ",");

	// field 2 - latitude
	sTmp = MOOSChomp(sNMEAString, ",");
	if ( sTmp.size() == 0 ) return;
	double dfLat = atof( sTmp.c_str() );

	// field 3 - N/S
	sTmp = MOOSChomp(sNMEAString, ",");
	if ( sTmp == "S" ) dfLat *= -1.0;

	// field 4 - longitude
	sTmp = MOOSChomp(sNMEAString, ",");
	if ( sTmp.size() == 0 ) return;
	double dfLong = atof( sTmp.c_str() );

	// field 5 - E/W
	sTmp = MOOSChomp(sNMEAString, ",");
	if ( sTmp == "W" ) dfLong *= -1.0;

	// perform lat/lon conversion
	double dfLatDecDeg = m_Geodesy.DMS2DecDeg(dfLat);
	double dfLongDecDeg = m_Geodesy.DMS2DecDeg(dfLong);
	m_Comms.Notify("GPS_LATITUDE", dfLatDecDeg);
	m_Comms.Notify("GPS_LONGITUDE", dfLongDecDeg);
	double dfXLocal, dfYLocal;
	if ( m_Geodesy.LatLong2LocalUTM(dfLatDecDeg, dfLongDecDeg, dfYLocal, dfXLocal) ) {
		m_Comms.Notify("GPS_X", dfXLocal);
		m_Comms.Notify("GPS_Y", dfYLocal);
	}

	// field 6 - position fix status
	sTmp = MOOSChomp(sNMEAString, ",");
	int dfFix = atoi( sTmp.c_str() );
	switch ( dfFix ) {
	case 0:
		m_Comms.Notify("GPS_FIX_STATUS", "invalid");
		break;
	case 1:
		m_Comms.Notify("GPS_FIX_STATUS", "standard");
		break;
	case 2:
		m_Comms.Notify("GPS_FIX_STATUS", "differential");
		break;
	}

	// field 7 - num satellites
	sTmp = MOOSChomp(sNMEAString, ",");
	int dfNumSV= atoi( sTmp.c_str() );
	m_Comms.Notify("GPS_NUM_SV", dfNumSV);

	// field 8 - HDOP
	sTmp = MOOSChomp(sNMEAString, ",");
	double dfHDOP = atof( sTmp.c_str() );
	m_Comms.Notify("GPS_HDOP", dfHDOP);
}

void SIMPLE_GPS::parseLine(string msg) {

	if (!DoNMEACheckSum(msg)) {
		cout << "checksum failed on: " << msg << endl;
		for ( int i=0; i<msg.size(); i++ ) {
			cout << hex << (int) msg[i] << " ";
		}
		cout << endl;
		return;
	}

	string cmd = MOOSChomp(msg, ",");
//	cout << "command = " << cmd << endl;
	if (cmd == "$GPRMC")
		parseGPRMC(msg);
	else if ( cmd == "$GPGGA" )
		parseGPGGA( msg );

}

//void SIMPLE_GPS::parseGPRMC( string msg ) {
//	// for example:
//	//	$GPRMC,174929,A,4221.5066,N,07105.2446,W,000.1,000.0,050412,015.6,W*70
//	// lat and lon are in degrees and minutes
//	// 4221.5066 = 42 degrees, 21.5066 minutes
//	vector<string> subs = tokenizeString( msg, ",");
//
////	cout << "parsing " << msg << endl;
//
//	// only proceed if we have lock
//	if ( subs[2] != "A" ) {
//		m_Comms.Notify("GPS_LOCK", "false");
//	} else {
//		m_Comms.Notify("GPS_LOCK", "true");
//
////		string date_string = subs[9];
////		string time_string = subs[1];
////		string modified_date = "20" + date_string.substr(4,2) +
////				date_string.substr(2,2) +
////				date_string.substr(0,2);
////		string composite = modified_date+"T"+time_string;
////		ptime t(from_iso_string(composite));
////
////		m_Comms.Notify("GPS_PTIME", to_simple_string(t));
////
////		double seconds = t.time_of_day().total_milliseconds()/1000.0;
////		m_Comms.Notify("GPS_TIME_SECONDS", seconds);
//
//		string lat_string = subs[3];
//		string lon_string = subs[5];
////		double temp_lat = atof(subs[3].c_str());
////		double temp_lon = atof(subs[5].c_str());
////		double lat_degrees = floor(temp_lat/100.0);
////		double lon_degrees = floor(temp_lon/100.0);
////		double lat_minutes = temp_lat - lat_degrees*100.0;
////		double lon_minutes = temp_lon - lon_degrees*100.0;
////		m_lat = lat_degrees + 60*lat_minutes;
////		m_lon = lon_degrees + 60*lon_minutes;
//
//		m_lat = 0;
//		m_lon = 0;
//		m_lat += atof(lat_string.substr(0,2).c_str());
//		m_lat += atof(lat_string.substr(2,lat_string.size()-2).c_str())/60;
//		m_lon += atof(lon_string.substr(0,3).c_str());
//		m_lon += atof(lon_string.substr(3,lon_string.size()-3).c_str())/60;
//		if ( subs[4] == "S" )
//			m_lat*=-1;
//		if ( subs[6] == "W" )
//			m_lon*=-1;
//
//		m_Comms.Notify("GPS_LATITUDE", m_lat);
//		m_Comms.Notify("GPS_LONGITUDE", m_lon);
//
//		m_speed =  atof(subs[7].c_str());
////		m_course = atof(subs[8].c_str());
//
//		m_Comms.Notify("GPS_SPEED", m_speed);
////		m_Comms.Notify("GPS_COURSE", m_course);
//
//		double latError = m_lat - m_lat_origin;
//		double lonError = m_lon - m_lon_origin;
//		double rlat = m_lat * M_PI/180;
//		double latErrorRad = latError * M_PI/180;
//		double lonErrorRad = lonError * M_PI/180;
//
//		double a = 6378137; //equatorial radius in m
//		double b = 6356752.3; //polar radius in m
//		double localEarthRadius = sqrt( ( pow(a,4) * pow( cos( rlat ), 2 ) +
//			pow( b, 4 ) * pow( sin( rlat ), 2 ) ) / ( pow( a * cos( rlat ), 2 ) +
//			pow( b * sin( rlat ), 2 ) ) );
//
//		double latErrorMeters = latErrorRad * localEarthRadius;
//		double lonErrorMeters = lonErrorRad * localEarthRadius;
//
//		m_Comms.Notify("GPS_X", lonErrorMeters);
//		m_Comms.Notify("GPS_Y", latErrorMeters);
//
////		m_Comms.Notify("GPS_MAGNETIC_VARIATION", atof(subs[10].c_str()));
//	}
//}
