/*
 *  TimeSignature.cpp
 *  wce2krn
 *
 *  Created by Peter Van Kranenburg on 10/27/06.
 *  Copyright 2006 Peter van Kranenburg. All rights reserved.
 *
 */

#include "TimeSignature.h"
#include "pvkutilities.h"
#include<string>
#include<sstream>
using namespace std;

TimeSignature::TimeSignature() {
	numerator = 0;
	denominator = 1;
}

TimeSignature::TimeSignature(string ts) {
	string::size_type pos = string::npos;
	/*
	if ( pos = ts.find("\\time ") != string::npos ) // time gevonden
		ts.erase(0,pos+6); //verwijder "\time "
	if ( pos = ts.find("\\time") != string::npos ) // time gevonden
		ts.erase(0,pos+5); //verwijder "\time"
	pos = ts.find("/");
	numerator = convertToInt(ts.substr(0,pos));
	denominator = convertToInt(ts.substr(pos+1));
	*/
	//cout << ts << endl;
	if ( ( pos = ts.find("\\time") ) != string::npos ) // \time gevonden
		ts.erase(0,pos+5); //verwijder \time
	if ( ( pos = ts.find("\\itime") ) != string::npos ) // \itime gevonden
		ts.erase(0,pos+6); //verwijder \time
	pvktrim(ts);
	//cout << pos << endl;
	//cout << ts << endl;
	pos = ts.find("/");
	string nstr = ts.substr(0,pos);
	string dstr = ts.substr(pos+1);
	numerator = convertToInt(pvktrim(nstr));
	denominator = convertToInt(pvktrim(dstr));
}

RationalTime TimeSignature::getRationalTime() const {
	return RationalTime(numerator, denominator);
}

int TimeSignature::getNumerator() const {
	return numerator;
}

int TimeSignature::getDenominator() const {
	return denominator;
}

string TimeSignature::getKernTimeSignature() const {
	std::stringstream ss;
	std::string res;
	ss << "*M" << numerator << "/" << denominator;
	ss >> res;
	return res;
}
