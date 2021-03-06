/*
 *  Song.h
 *  wce2krn
 *
 *  Created by Peter Van Kranenburg on 10/27/06.
 *  Copyright 2006 Peter Van Kranenburg. All rights reserved.
 *
 */

#ifndef SONG_H
#define SONG_H

#include<vector>
#include<string>
#include"WCE_File.h"
#include"SongLine.h"
using namespace std;

#include "RationalTime.h"
#include "TimeSignature.h"

class Song {
public:
	Song(string inputfilename, bool weblilypond, string filename_titles);
	
	TimeSignature translateTimeSignature(string lyTimeSignature) const;
	RationalTime translateUpbeat(string lyUpbeat) const;
	int translateMidiTempo(string lyKey) const;
	string getLocation() const;
	void printContents() const;
	
	void writeToDisk(string basename_full,
					 SongLine::Representation repr,
					 bool lines, int ly_ver) const;
	
private:
	WCE_File wcefile;
	vector<SongLine> songLines;
	bool weblily;
};

#endif
