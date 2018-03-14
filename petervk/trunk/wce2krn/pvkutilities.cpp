/*
 *  pvkutilities.cpp
 *  wce2krn
 *
 *  Created by Peter Van Kranenburg on 11/3/06.
 *  Copyright 2006 Peter van Kranenburg. All rights reserved.
 *
 */

#include "pvkutilities.h"

string& pvktrim(string& s, string drop) {
	s.erase(s.find_last_not_of(drop)+1);
    s.erase(0,s.find_first_not_of(drop));
	return s;
}

string& columnize(string& s, int length, char fill) {
	while ( s.size() < length ) s = s + fill;
	return s;
}

string& removeTextFromNote(string& external_token) {
	string::size_type pos1, pos2;
	//locate the texts (if any)
	while( (pos1 = external_token.find("^\"")) != string::npos || (pos1 = external_token.find("_\"")) != string::npos ) {
		//find end of text
		pos2 = external_token.find_first_of('\"', pos1+2); //skip the ^" or _"
		external_token.erase(pos1, pos2-pos1+1);
	}
	return external_token;
}
