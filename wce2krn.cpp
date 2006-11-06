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

int main (int argc, char * const argv[]) {

	//string filename(argv[1]);
	//string filename("/Users/pvk/Documents/data/OGLproefcorpus/OGL21810.wce");
	string filename("/Users/pvk/Documents/data/OGLproefcorpus/OGL19103.wce");
	//string filename("/Users/pvk/Desktop/Untitled.wce");
	//string filename("/Users/pvk/Desktop/hierinuwhuis.wce");
	
	//test conversie TimeSignature
	/*TimeSignature ts("\\time 2325/432");
	//cout <<	ts.getNumerator() << "/" << ts.getDenominator() << endl;
	return 0; */
	
	//test inititalisatie SongLine
	/*RationalTime ub(1,4);
	TimeSignature ts("33/12");
	vector<string> strs;
	strs.push_back("eerste string");
	strs.push_back("tweede string");
	SongLine sl1(strs, ub, ts, 8, 4);
	SongLine sl;
	sl = SongLine(strs, ub, ts, 8, 4);
	cout << sl.getUpbeat().getNumerator() << "/" << sl.getUpbeat().getDenominator() << endl;
	cout << sl.getInitialTimeSignature().getNumerator() << "/" << sl.getInitialTimeSignature().getDenominator() << endl;
	sl.writeToStdout();
	return 0; */
	
	string basename = filename;
	string::size_type pos;	
	pos = basename.rfind(".wce");
	basename.erase(pos);
	Song s(filename);

	s.writeKern(basename);
	//s.writeKern(basename, true, false);
	//s.writeLy(basename, false, true, false);
	//s.writeLy(basename, true, true, false);
	
	return 0;
}
