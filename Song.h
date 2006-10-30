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
#include<WCE_File.h>
#include<Spine.h>
#include<SongLine.h>
using namespace std;

#include "RationalTime.h"
#include "TimeSignature.h"

class Song {
public:
	Song(string inputfilename);

	vector<string> getAbsoluteLilypondFile() const;
	vector< vector<string> > getAbsoluteLilypondLineFiles() const;
 	vector<Spine> getKernFile() const;
	
	TimeSignature translateTimeSignature(string lyTimeSignature) const;
	RationalTime translateUpbeat(string lyUpbeat) const;
	
	void writeKern(string basename, bool lines = false, bool rhythm = false) const;
	void writeLy(string basename, bool lines = false, bool absoulte = false, bool rhythm = false) const;
	
private:
	WCE_File wcefile;
	vector<SongLine> songLines;
};

#endif