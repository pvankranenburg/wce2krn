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
	string::size_type pos;
	if ( pos = ts.find("\\time ") != string::npos ) // time gevonden
		ts.erase(0,6); //verwijder "\time "
	if ( pos = ts.find("\\time") != string::npos ) // time gevonden
		ts.erase(0,5); //verwijder "\time"
	pos = ts.find("/");
	numerator = convertToInt(ts.substr(0,pos));
	denominator = convertToInt(ts.substr(pos+1));
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
