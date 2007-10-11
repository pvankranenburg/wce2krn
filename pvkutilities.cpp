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