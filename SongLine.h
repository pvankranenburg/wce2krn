/*
 *  LilyLine.h
 *  wce2krn
 *
 *  Created by Peter Van Kranenburg on 10/27/06.
 *  Copyright 2006 Peter van Kranenburg. All rights reserved.
 *
 */

#ifndef SONGLINE_H
#define SONGLINE_H

#include "RationalTime.h"
#include "TimeSignature.h"
#include "Spine.h";
#include<string>
#include<vector>
using namespace std;

class SongLine {
public:
	SongLine(vector<string> lines, RationalTime upb, TimeSignature timesig, int duration, int octave); //if duration is 0, take duration from first note
	SongLine();
	SongLine(const SongLine& sl);
	SongLine& operator=(const SongLine& sl);
	
	void writeToStdout() const;
	
	vector<string> getWceLines() const { return wcelines; }
	RationalTime getUpbeat() const { return upbeat; }
	
	TimeSignature getInitialTimeSignature() const {return initialTimeSignature;}
	int getInitialOctave() const {return initialOctave;}
	int getInitialDuration() const {return initialDuration;}
	
	TimeSignature getFinalTimeSignature() const {return finalTimeSignature;} 
	int getFinalOctave() const {return finalOctave;}
	int getFinalDuration() const {return finalDuration;}

	vector<string> getLyLine(bool absolute) const;
	vector<string> getLyBeginSignature(bool absolute) const;
	vector<string> getLyEndSignature() const;
	
	vector<Spine> getKernLine() const;
	vector<Spine> getKernBeginSignature() const;
	vector<Spine> getKernEndSignature() const;
	
private:
	const vector<string> wcelines; //original wce lines (belonging to one song line)
	vector<string> absLyLine; //lilypond line with absolute pitch and explicit note durations
	vector<Spine> KernLine; //kern representation of the line .
	 
	const RationalTime upbeat;
	const TimeSignature initialTimeSignature;
	TimeSignature finalTimeSignature;
	const int initialOctave;
	int finalOctave;
	const int initialDuration;
	int finalDuration;
};

#endif