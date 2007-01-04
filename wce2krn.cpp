/*
 *  wce2krn.cpp
 *  wce2krn
 *
 *  Created by Peter Van Kranenburg on 10/25/06.
 *  Copyright 2006 Peter van Kranenburg. All rights reserved.
 *
 */
 
#include <iostream>
#include <string>
#include "Song.h"
using namespace std;

#include "SongLine.h"

void usage() {
	cerr << "Usage: wce2krn [-k] [-s] [-l] [wcefile]" << endl;
	cerr << "Reads wce-file and generates *kern file" << endl;
	cerr << " -s: spit in lines." << endl;
	cerr << " -l: generate file(s) with only the lyrics." << endl;
	cerr << " -h: print this help message" << endl;
	cerr << "If no filename is given, or '-', standard input and output will be used." << endl;
	cerr << "In this case, '-s' doesn't have any effect and '-l' suppresses *kern output." << endl << endl;
}

int main (int argc, char * const argv[]) {
	
	bool kern = true;
	bool split = false;
	bool absolute = true;
	bool lyrics = false;
	
	string filename = "-";
	string arg = "";
	
	for (int i = 1; i<argc; i++ ) {
		arg = string(argv[i]);
		if ( arg == "-s" ) split = true;
		else if ( arg == "-l" ) lyrics = true;
		else if ( arg == "-h" ) { usage(); exit(0); }
		else filename = arg;
	}
	
	clog << endl;
	clog << "Processing " << filename << endl;
	
	string basename = filename;
	
	if ( basename != "-" ){
		string::size_type pos;	
		pos = basename.rfind(".wce");
		basename.erase(pos);
	}
	
	Song s(filename);
	
	if (basename == "-" && kern && lyrics) kern = false;
	if (basename == "-" && split ) split = false;
	if (kern) s.writeToDisk(basename, SongLine::KERN, split, absolute);
	if (lyrics) s.writeToDisk(basename, SongLine::TEXT, split, absolute);
	
	return 0;
}
