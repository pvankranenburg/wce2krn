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
	cout << "usage: wce2krn [-s] [-l] wcefile" << endl;
	cout << " -s: spit in lines." << endl;
	cout << " -l: generate file(s) with only the lyrics." << endl << endl;
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
	
	
	bool split = false;
	bool absolute = true;
	bool lyrics = false;
	
	string filename;
	string arg1 = "";
	string arg2 = "";
	
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
	
	cout << endl;
	cout << "Processing " << filename << endl;
	
	
	//const std::string filename = std::string(input);
	string basename = filename;
	string::size_type pos;	
	pos = basename.rfind(".wce");
	basename.erase(pos);
	Song s(filename);
	
	s.writeToDisk(basename, SongLine::KERN, split, absolute);
	if (lyrics) s.writeToDisk(basename, SongLine::TEXT, split, absolute);
	
	return 0;
}
