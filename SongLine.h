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
	SongLine(vector<string> lines, RationalTime upb, TimeSignature timesig, int duration, int dots, int octave, char pitchclass, bool initialtriplet, int keysig, int mtempo, int barnumber, bool meterinvisible, string filename, int phraseno, int numphrases, string recordno, string stropheno, string wcelineno); //if duration is 0, take duration from first note
	SongLine();
	SongLine(const SongLine& sl);
	SongLine& operator=(const SongLine& sl);
	
	enum Representation { KERN, RELLY, ABSLY, TEXT };
	
	void writeToStdout() const;
	
	vector<string> getWceLines() const { return wcelines; }
	//RationalTime getUpbeat() const { return upbeat; }
	
	int getNumberOfLines() const { return relLyTokens.size(); }
	
	bool checkMelisma() const; // ties ok? slurs ok? text ok (-- or _)?
	bool checkTies() const; // slur where a tie should be?
	bool checkTextPlacing() const;
	bool checkLengths() const;
	
	TimeSignature getInitialTimeSignature() const {return initialTimeSignature;}
	RationalTime getInitialUpbeat() const {return initialUpbeat;}
	int getInitialOctave() const {return initialOctave;}
	char getInitialLastPitchClass() const {return initialLastPitchClass;}
	int getInitialDuration() const {return initialDuration;}
	int getInitialDots() const {return initialDots;}
	int getInitialBarnumber() const {return initialBarnumber;}
	bool getInitialTripletStatus() const {return initialTripletStatus; }
	
	TimeSignature getFinalTimeSignature() const {return finalTimeSignature;} 
	RationalTime getFinalUpbeat() const {return finalUpbeat;}
	int getFinalOctave() const {return finalOctave;}
	char getFinalLastPitchClass() const {return finalLastPitchClass;}
	int getFinalDuration() const {return finalDuration;}
	int getFinalDots() const {return finalDots;}
	int getFinalBarnumber() const {return finalBarnumber;}
	int getKeySignature() const { return keySignature;}
	bool getFinalTripletStatus() const { return finalTripletStatus; }

	int getMidiTempo() const { return midiTempo; }
	bool getMeterInvisible() const { return meterInvisible; }
	int getPhraseNo() const { return phraseNo; }
	string getRecord() const { return record; }
	string getStrophe() const { return strophe; }

	void createAnnotations();
	vector<string> getAnnotations() { return annotations; }
	void printAnnotations() const;
	string getLocation() const;
	string getWCELineNumber() const { return WCELineNumber; }

	//NB no initialSlur and finalSlur status. Slurs should not be extended over line endings. -- might happen though

	vector<string> getLyLine(bool absolute, bool lines) const;
	vector<string> getLyBeginSignature(bool absolute, bool lines, bool weblily) const;
	vector<string> getLyEndSignature() const;
	
	vector<string> getKernLine() const;
	vector<string> getKernBeginSignature(bool lines) const;
	vector<string> getKernEndSignature() const;
	
	string getLyricsLine(int line) const; //line 0 is first text line
	
private:
	void translate();
	vector<vector<RelLyToken> > relLyTokens; //the input broken into tokens.
	vector<vector<string> > absLyTokens; //converted to abs ly
	vector<vector<string> > kernTokens; //converted to kern
	const vector<string> wcelines; //original wce lines (belonging to one song line)
	vector<string> absLyLine; //lilypond line with absolute pitch and explicit note durations
	//vector<Spine> KernLine; //kern representation of the line .
	vector<string> lyricsLines; //lyrics in normal text representation.
	bool translationMade; //true if absLyLine and KernLine are made.
	
	//for check
	vector<RelLyToken::SlurStatus> slurs_ann; //annotation for slurs
	vector<RelLyToken::TieStatus> ties_ann; // annotation for ties
	vector< vector<RelLyToken::TextStatus> > text_ann; // annotation for text
	vector<string> annotations;
	
	//some useful functions
	void breakWcelines(); //output goes into relLyTokens. Invoked in translate().
	int computeOctave(int curoct, char pitch, char lastPitch, int octcorrection) const;
	RationalTime rationalDuration(int duration, int dots, bool triplet) const;
	string toText(string tok, RelLyToken::TextStatus ts, Representation repr)  const;
	string upbeatToString(RationalTime t) const;
	bool inheritFirstLynoteDuration( string& lyline, int duration) const;
	
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
	const int initialDots;
	int finalDots;
	const int keySignature;
	const int midiTempo;
	const int initialBarnumber;
	int finalBarnumber;
	bool meterInvisible;
	bool initialTripletStatus;
	bool finalTripletStatus;
	string fileName;
	int phraseNo;
	int numPhrases;
	string record;
	string strophe;
	string WCELineNumber;
	
};

#endif
