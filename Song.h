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
	Song(string inputfilename);
	
	TimeSignature translateTimeSignature(string lyTimeSignature) const;
	RationalTime translateUpbeat(string lyUpbeat) const;
	int translateKeySignature(string lyKey) const;
	
	void writeToDisk(string basename_full,
					 SongLine::representation repr = SongLine::KERN,
					 bool lines = false,
					 bool absoulte = false) const;
	
private:
	WCE_File wcefile;
	vector<SongLine> songLines;
};

#endif