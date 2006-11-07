/*
 *  RelLyToken.h
 *  wce2krn
 *
 *  Created by Peter Van Kranenburg on 10/31/06.
 *  Copyright 2006 Peter Van Kranenburg. All rights reserved.
 *
 */

#ifndef RELLYTOKEN_H
#define RELLYTOKEN_H

#include<string>
using namespace std;

#include "Timesignature.h"

class RelLyToken {
public:
	RelLyToken(string t);
	RelLyToken();
	RelLyToken(const RelLyToken& r);
	RelLyToken& RelLyToken::operator=(const RelLyToken& r);
	
	enum Identity { NOTE, TIME_COMMAND, TIMES_COMMAND, TEXT, UNKNOWN };
	enum SlurStatus { NO_SLUR_INFO, START_SLUR, END_SLUR, IN_SLUR, NO_SLUR }; //only START_SLUR and END_SLUR can be extracted from relative ly token!
	enum TieStatus { NO_TIE_INFO, START_TIE, CONTINUE_TIE, END_TIE, NO_TIE }; //only START_TIE can be extracted from relative ly token!
	enum Accidental { DOUBLE_FLAT, FLAT, NO_ACCIDENTAL, NATURAL, SHARP, DOUBLE_SHARP };
	enum BraceStatus { OPEN_BRACE, CLOSE_BRACE };
	enum TextStatus { SINGLE_WORD, BEGIN_WORD, END_WORD, IN_WORD };

	string RelLyToken::getToken() const { return token; }
	
	Identity getIdentity() const;
	char getPitchClass() const;
	int getOctaveCorrection() const; // 0 if no octave correction
	int getDurationBase() const; // 0 if no durations is given
	bool getDotted() const; // false if no duration is given
	bool getNotDotted() const; // true if no duration is given
	SlurStatus getSlur() const;
	TieStatus getTie() const;
	Accidental getAccidental() const;
	BraceStatus getBraceStatus() const;
	bool containsOpeningBrace() const {return (token.find("{") != string::npos); }
	bool containsClosingBrace() const {return (token.find("}") != string::npos); }
	bool containsClosingBraceBeforeNote() const;
	bool containsClosingBraceAfterNote() const;
		
	TimeSignature getTimeSignature() const; //do only invoke if idenity is TIME_COMMAND
	
	//pitchclass, tiestatus, accidental en braces are already known. If token is rest ('r' or 's'), octave en slur not taken into account.
	string createKernNote(int octave, int duration, bool dotted, bool triplet, SlurStatus slur, TieStatus tie) const;
	string createAbsLyNote(int octave, int duration, bool dotted, SlurStatus slur, TieStatus tie) const;
						  
private:
	const string token;
};

#endif