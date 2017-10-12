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
#include<vector>
using namespace std;

#include "TimeSignature.h"
#include "pvkutilities.h"

class RelLyToken {
public:
	
	enum Identity { NOTE, TIME_COMMAND, TIMES_COMMAND, TEXT, GRACE, CHORD, BARLINE, STOPBAR, UNKNOWN };
	enum SlurStatus { NO_SLUR_INFO, START_SLUR, END_SLUR, IN_SLUR, NO_SLUR }; //only START_SLUR and END_SLUR can be extracted from relative ly token!
	enum TieStatus { NO_TIE_INFO, START_TIE, CONTINUE_TIE, END_TIE, NO_TIE }; //only START_TIE can be extracted from relative ly token!
	//enum GlissandoStatus { START_GLISSANDO, END_GLISSANDO };
	enum Accidental { DOUBLE_FLAT, FLAT, NO_ACCIDENTAL, NATURAL, SHARP, DOUBLE_SHARP };
	enum BraceStatus { OPEN_BRACE, CLOSE_BRACE };
	enum TextStatus { SINGLE_WORD, BEGIN_WORD, END_WORD, IN_WORD, NO_WORD, BEGIN_WORD_CONT, SINGLE_WORD_CONT, END_WORD_CONT, IN_WORD_CONT, DONTKNOW };
	enum BarLineType { NOBARLINE, NORMALBAR, ENDBAR, DOUBLEBAR, BEGINREPEAT, ENDREPEAT, DOUBLEREPEAT};

	RelLyToken(string t, string loc, int lineno, int linepos, RelLyToken::Identity token_id, bool softbreak, bool is_music = true);
	RelLyToken();
	RelLyToken(const RelLyToken& r);
	RelLyToken& operator=(const RelLyToken& r);

	static string printSlurStatus(SlurStatus ss);
	static string printTieStatus(TieStatus ts);
	static string printTextStatus(TextStatus ts);
	static string printIdentity(Identity i);

	string getToken() const { return token; }
	
	//string getWCEPosition() const { return WCEPosition; }
	string getWCEPosition() const { return convertToString(WCE_LineNumber) + ":" + convertToString(WCE_Pos); }

	int getWCE_LineNumber() const { return WCE_LineNumber; }
	int getWCE_Pos() const { return WCE_Pos; }
	
	void addTie(); //adds a tie to the note.
	void addClosingBrace(); //adds a closing brace to the note.
	void addSlurBegin(); //adds a slur begin ( to the note
	void addSlurEnd(); //adds a slur begin ( to the note

	vector<RelLyToken> splitChord() const;
	
	Identity getIdentity() const;
	char getPitchClass() const;
	int getOctaveCorrection() const; // 0 if no octave correction
	int getDurationBase() const; // 0 if no durations is given
	int getDots() const; // return number of dots
	bool getInterpretedPitch() const; // true if pitch is interpreted (crossed note head)
	bool getNotDotted() const; // true if no duration is given
	bool getGlissandoEnd() const; // true if glissando ends on this note.
	string getLocation() const { return location; }
	SlurStatus getSlur() const;
	TieStatus getTie() const;
	Accidental getAccidental() const;
	BraceStatus getBraceStatus() const;
	BarLineType getBarLineType() const;
	bool containsOpeningBrace() const {return (token.find("{") != string::npos); }
	bool containsClosingBrace() const {return (token.find("}") != string::npos); }
	bool containsClosingBraceBeforeNote() const;
	bool containsClosingBraceAfterNote() const;
	bool isRest() const;
	void setSoftBreak() { softBreak = true;};
	bool hasSoftBreak() const { return softBreak; };
	
	TimeSignature getTimeSignature() const; //do only invoke if identity is TIME_COMMAND
	
	//pitchclass, tiestatus, accidental en braces are already known. If token is rest ('r' or 's'), octave en slur not taken into account.
	string createKernNote(int octave, int duration, int dots, bool triplet, SlurStatus slur, TieStatus tie, bool opensub, bool closesub) const;
	string createKernSingleNote(int octave, int duration, int dots, bool triplet, SlurStatus slur, TieStatus tie, bool opensub, bool closesub) const;
	string createKernChordNote(int octave, int duration, int dots, bool triplet, SlurStatus slur, TieStatus tie, bool opensub, bool closesub) const;
	string createAbsLyNote(int octave, int duration, int dots, SlurStatus slur, TieStatus tie) const;

	string getKernBarLine() const;

private:
	//Identity computeIdentity(bool is_music) const;
	
	string token;
	Identity id;
	const string location;
	//const string WCEPosition; //store the position of the token in the ly input field.
	                          //format: line number:
	const int WCE_LineNumber;
	const int WCE_Pos;
	bool softBreak;
};

#endif
