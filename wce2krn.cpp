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
#include "wce2krn.h"
#include "Song.h"
using namespace std;

#include "SongLine.h"

void print_usage() {
	cout << "Usage: wce2krn [-k] [-r] [-s] [-l] [-v] [-h] [wcefile]" << endl;
	cout << "Reads wce-file and generates *kern file" << endl;
	cout << " -k: generate kernfile." << endl;
	cout << " -r: generate relative lilypond file." << endl;
	cout << " -w: generate lilypond for presentation on the web." << endl;
	//cout << " -a: generate absolute lilypond file (not implemented)!" << endl;
	cout << " -l: generate file(s) with only the lyrics." << endl;
	cout << " -s: spit in lines." << endl;
	cout << " -p: print the contents of the music to stdout with annotations" << endl;
	cout << " -e: suppress log messages." << endl;
	cout << " -v: print version number and exit." << endl;
	cout << " -h: print this help message and exit." << endl;
	cout << "If no filename is given, or '-', standard input and output will be used." << endl;
	cout << "In this case, '-s' doesn't have any effect." << endl << endl;
}

void print_version() {
	cout << "wce2krn version " << version << " (" << releasedate << ")" << endl;
}


int main (int argc, char * const argv[]) {
	
	bool kern = false;
	bool split = false;
	bool lyrics = false;
	bool absly = false;
	bool relly = false;
	bool weblily = false;
	bool suppresslog = false;
	bool printContents = false;
	
	string filename = "-";
	string arg = "";
	
	for (int i = 1; i<argc; i++ ) {
		arg = string(argv[i]);
		if ( arg == "-s" ) split = true;
		else if ( arg == "-k" ) kern = true;
		else if ( arg == "-l" ) lyrics = true;
		else if ( arg == "-r" ) relly = true;
		else if ( arg == "-a" ) absly = true;
		else if ( arg == "-w" ) weblily = true;
		else if ( arg == "-p" ) printContents = true;
		else if ( arg == "-e" ) suppresslog = true;
		else if ( arg == "-h" ) { print_usage(); exit(0); }
		else if ( arg == "-v" ) { print_version(); exit(0); }
		
		else filename = arg;
	}
	
	ofstream out;
	streambuf* clogbuf;
	if(suppresslog) {
		out.open("/dev/null");
		clogbuf = clog.rdbuf();
		clog.rdbuf(out.rdbuf());
	}
	
	clog << endl;
	clog << "===============================================================================" << endl;
	clog << "Processing " << filename << endl;
	
	string basename = filename;
	
	if ( basename != "-" ){
		string::size_type pos;	
		pos = basename.rfind(".wce");
		basename.erase(pos);
	}
	
	Song s(filename, weblily);
	if (printContents) s.printContents();
			
	if (basename == "-" && kern && lyrics) kern = false;
	if (basename == "-" && split ) split = false;
	if (kern) s.writeToDisk(basename, SongLine::KERN, split);
	if (lyrics) s.writeToDisk(basename, SongLine::TEXT, split);
	if (relly) s.writeToDisk(basename, SongLine::RELLY, split);
	//if (absly) s.writeToDisk(basename, SongLine::ABSLY, split);
	clog << "===============================================================================" << endl;

	if(suppresslog) {
		out.close();
		clog.rdbuf(clogbuf);
	}

	return 0;
}
