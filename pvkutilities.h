/*
 *  pvkutilities.h
 *  wce2krn
 *
 *  Created by Peter Van Kranenburg on 10/28/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef PVKUTILITIES_H
#define PVKUTILITIES_H

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
using namespace std;

class BadConversion : public std::runtime_error {
public:
	BadConversion(const std::string& s)	: std::runtime_error(s) { }
};

inline int convertToInt(const std::string& s) {
	std::istringstream i(s);
	int x;
	if (!(i >> x))
	  throw BadConversion("convertToInt(\"" + s + "\")");
	return x;
}

inline string convertToString(const int i) {
	std::ostringstream o;
	if (!(o << i))
	  throw BadConversion("convertToString(...)");
	return o.str();
}

string& pvktrim(string& s, string drop = "\t ");

string& columnize(string& s, int length, char fill = ' ');

string& removeTextFromNote(string& external_token);

#endif
