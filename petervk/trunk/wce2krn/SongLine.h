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

#include <string>
#include <vector>
using namespace std;

#include "RationalTime.h"
#include "TimeSignature.h"
#include "RelLyToken.h"

class SongLine {
public:
	SongLine(vector<string> lines, RationalTime upb, TimeSignature timesig, int duration, bool dotted, int octave, char pitchclass, int keysig, int mtempo, int barnumber); //if duration is 0, take duration from first note
	SongLine();
	SongLine(const SongLine& sl);
	SongLine& operator=(const SongLine& sl);
	
	enum representation { KERN, RELLY, ABSLY };
	
	void writeToStdout() const;
	
	vector<string> getWceLines() const { return wcelines; }
	//RationalTime getUpbeat() const { return upbeat; }
	
	int getNumberOfLines() const { return relLyTokens.size(); }
	
	TimeSignature getInitialTimeSignature() const {return initialTimeSignature;}
	RationalTime getInitialUpbeat() const {return initialUpbeat;}
	int getInitialOctave() const {return initialOctave;}
	char getInitialLastPitchClass() const {return initialLastPitchClass;}
	int getInitialDuration() const {return initialDuration;}
	bool getInitialDotted() const {return initialDotted;}
	int getInitialBarnumber() const {return initialBarnumber;}
	
	TimeSignature getFinalTimeSignature() const {return finalTimeSignature;} 
	RationalTime getFinalUpbeat() const {return finalUpbeat;}
	int getFinalOctave() const {return finalOctave;}
	char getFinalLastPitchClass() const {return finalLastPitchClass;}
	int getFinalDuration() const {return finalDuration;}
	bool getFinalDotted() const {return finalDotted;}
	int getFinalBarnumber() const {return finalBarnumber;}
	int getKeySignature() const { return keySignature;}

	int getMidiTempo() const { return midiTempo; }

	//NB no initialSlur and finalSlur status. Slurs should not be extended over line endings.

	vector<string> getLyLine(bool absolute) const;
	vector<string> getLyBeginSignature(bool absolute) const;
	vector<string> getLyEndSignature() const;
	
	vector<string> getKernLine() const;
	vector<string> getKernBeginSignature() const;
	vector<string> getKernEndSignature() const;
	
private:
	void translate();
	vector<vector<RelLyToken> > relLyTokens; //the input broken into tokens.
	vector<vector<string> > absLyTokens; //converted to abs ly
	vector<vector<string> > kernTokens; //converted to kern
	const vector<string> wcelines; //original wce lines (belonging to one song line)
	vector<string> absLyLine; //lilypond line with absolute pitch and explicit note durations
	//vector<Spine> KernLine; //kern representation of the line .
	bool translationMade; //true if absLyLine and KernLine are made. 
	
	//some useful functions
	void breakWcelines(); //output goes into relLyTokens. Invoked in translate().
	int computeOctave(int curoct, char pitch, char lastPitch, int octcorrection) const;
	RationalTime rationalDuration(int duration, bool dotted, bool triplet) const;
	string toKernText(string tok, RelLyToken::TextStatus ts) const;
	
	const RationalTime initialUpbeat;
	RationalTime finalUpbeat;
	const TimeSignature initialTimeSignature;
	TimeSignature finalTimeSignature;
	const int initialOctave;
	int finalOctave;
	const char initialLastPitchClass;
	char finalLastPitchClass;
	const int initialDuration; //multiples of 2, or multiples of 3 (in case of triplets).
	int finalDuration;
	const bool initialDotted;
	bool finalDotted;
	const int keySignature;
	const int midiTempo;
	const int initialBarnumber;
	int finalBarnumber;
};

#endif