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
//#include <cmdargs.h>
using namespace std;

#include "SongLine.h"

void usage() {
	cerr << "Usage: wce2krn [-k] [-s] [-l] [wcefile]" << endl;
	cerr << " -k: generate *kern." << endl;
	cerr << " -s: spit in lines." << endl;
	cerr << " -l: generate file(s) with only the lyrics." << endl;
	cerr << " -h: print this help message" << endl;
	cerr << "If no filename is given, standard input and output will be used." << endl;
	cerr << "In this case, '-s' doesn't have any effect and '-k' cancels '-l'." << endl << endl;
}

int main (int argc, char * const argv[]) {
	
	/*
	// Commandline arguments
	CmdArgBool split_flag('s', "split", "generate an separate outputfile for each verse.");
	//CmdArgInt  wnumber('w', "window", "number", "size of the window when splitting.");
	//CmdArgInt  onumber('o', "offset", "number", "stepsize to next part when splitting.");
	//CmdArgBool  mflag('m', "mason", "make mason diagram. Will be written in png-format");
	CmdArgBool  text_flag('t', "text", "generate a with only the lyrics.");
	CmdArgStr  input("input-file",  "input file. A WichcraftEditor file.");
	//CmdArgStr  output("output-file",  "file to write featurevalues.");

	// Declare command object and its argument-iterator
	CmdLine  cmd(*argv, &split_flag, &text_flag, &input, NULL);
	CmdArgvIter arg_iter(--argc, ++argv);

	// Initialize arguments to appropriate default values.
	split_flag = 0;
	text_flag = 0;
  
	// Parse arguments
	cmd.parse(arg_iter);	
	*/
	
	bool kern = false;
	bool split = false;
	bool absolute = true;
	bool lyrics = false;
	
	string filename = "-";
	string arg = "";
	//string arg1 = "";
	//string arg2 = "";
	
	for (int i = 1; i<argc; i++ ) {
		arg = string(argv[i]);
		if ( arg == "-s" ) split = true;
		else if ( arg == "-l" ) lyrics = true;
		else if ( arg == "-k" ) kern = true;
		else if ( arg == "-h" ) { usage(); exit(0); }
		else filename = arg;
	}
	
	/*
	if ( argc == 4 ) {
		filename = string(argv[3]);
		arg1 = string(argv[1]);
		arg2 = string(argv[2]);		
	} else if ( argc == 3 ) {
		filename = string(argv[2]);
		arg1 = string(argv[1]);
	} else if ( argc == 2 ) {
		filename = string(argv[1]);
	} else {
		usage();
		exit(1);
	}
	
	if ( arg1 == "-s" || arg2 == "-s" ) split = true;
	if ( arg1 == "-l" || arg2 == "-l" ) lyrics = true;
	*/
	
	clog << endl;
	clog << "Processing " << filename << endl;
	
	
	//const std::string filename = std::string(input);
	string basename = filename;
	
	if ( basename != "-" ){
		string::size_type pos;	
		pos = basename.rfind(".wce");
		basename.erase(pos);
	}
	
	Song s(filename);
	
	if (basename == "-" && kern && lyrics) lyrics = false;
	if (basename == "-" && split ) split = false;
	if (kern) s.writeToDisk(basename, SongLine::KERN, split, absolute);
	if (lyrics) s.writeToDisk(basename, SongLine::TEXT, split, absolute);
	
	return 0;
}
