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

//struct for pitch and octave
struct Pitchclass_Octave {
	Pitchclass_Octave() : pitchclass('c'), octave(4) {};
	Pitchclass_Octave(char p, int o) : pitchclass(p), octave(o) {};
	char pitchclass;
	int octave;
};

class RelLyToken {
public:
	
	enum Identity { NOTE, TIME_COMMAND, TIMES_COMMAND, TEXT, FREETEXT, GRACE, CHORD, BARLINE, CLEF_COMMAND, KEY_COMMAND, UNKNOWN };
	enum SlurStatus { NO_SLUR_INFO, START_SLUR, END_SLUR, ENDSTART_SLUR, IN_SLUR, NO_SLUR, START_NEW_SLUR }; //only START_SLUR and END_SLUR can be extracted from relative ly token!
	enum TieStatus { NO_TIE_INFO, START_TIE, CONTINUE_TIE, END_TIE, NO_TIE }; //only START_TIE can be extracted from relative ly token!
	//enum GlissandoStatus { START_GLISSANDO, END_GLISSANDO };
	enum Accidental { DOUBLE_FLAT, FLAT, NO_ACCIDENTAL, NATURAL, SHARP, DOUBLE_SHARP };
	enum BraceStatus { OPEN_BRACE, CLOSE_BRACE };
	enum TextStatus { SINGLE_WORD, BEGIN_WORD, END_WORD, IN_WORD, NO_WORD, BEGIN_WORD_CONT, SINGLE_WORD_CONT, END_WORD_CONT, IN_WORD_CONT, DONTKNOW };
	enum BarLineType { NOBARLINE, NORMALBAR, ENDBAR, DOUBLEBAR, BEGINREPEAT, ENDREPEAT, DOUBLEREPEAT, UNKNOWNBAR};
	enum GraceType { PLAINGRACE, APP, KVS, AFTER, NOGRACE }; //make sure only one of this value corresponds with NOT grace notes
	enum Ornament { TRILL, PRALL, PRALLPRALL, MORDENT, TURN, DOUBLESLASH}; //handle "+" as text. often not clear what it means.
	enum Articulation { STACCATO, STACCATISSIMO, ACCENT, TENUTO};

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

	void addOrnament(Ornament o) {ornaments.push_back(o);}; //
	void addArticulation(Articulation a) {articulations.push_back(a); };
	bool hasOrnament() const { return (ornaments.size()>0); };
	bool hasArticulation() const { return (articulations.size()>0); };
	vector<Ornament> getOrnaments() const { return ornaments; };
	vector<Articulation> getArticulations() const { return articulations; };
	bool isIn(Ornament o, vector<Ornament> orns) const;
	bool isIn(Articulation a, vector<Articulation> arts) const;
	string krnOrnaments() const;
	string krnArticulations() const;


	vector<RelLyToken> splitChord() const;
	
	Identity getIdentity() const;
	char getPitchClass() const;
	char getRefPitchClass() const;
	string::size_type getPosOfPitchClass() const;
	int getOctaveCorrection() const; // 0 if no octave correction; for chord return octavecorrection of first note
	int getDurationBase() const; // 0 if no durations is given
	int getDots() const; // return number of dots
	string getClefType() const; //return the clef type for a clef change.
	int computeOctave(int previous_octave, char previous_pitch, int octcorrection, char pitch) const;
	vector<RelLyToken> getNotes() const { return notes; }
	//Pitchclass_Octave getHighestOfChord(int initialoctave) const;
	int getIndexHighestOfChord() const;
	vector<Pitchclass_Octave> getAllOfChord(int initialoctave) const;
	//Pitchclass_Octave getRelative() const; //get the pitch and octave that is context for next note.
	bool getInterpretedPitch() const; // true if pitch is interpreted (crossed note head)
	bool getFicta() const; // true if accidental is editorial
	bool getNotDotted() const; // true if no duration is given
	bool getGlissandoEnd() const; // true if glissando ends on this note.
	bool getGlissandoBegin() const { return beginGlissando; } //true if a glissando starts at this note.
	void setGlissandoBegin(bool value=true) { beginGlissando = value; }
	string getLocation() const { return location; }
	SlurStatus getSlur() const;
	TieStatus getTie() const;
	Accidental getAccidental() const;
	BraceStatus getBraceStatus() const;
	void getBarLinePositions(string::size_type & pos1, string::size_type & pos2) const;
	BarLineType getBarLineType() const;
	GraceType getGraceType() const;
	bool containsOpeningBrace() const {return (token.find("{") != string::npos); }
	bool containsClosingBrace() const {return (token.find("}") != string::npos); }
	bool containsClosingBraceBeforeNote() const;
	bool containsClosingBraceAfterNote() const;
	bool isRest() const;
	void setSoftBreak() { softBreak = true;};
	void removeSoftBreak() { softBreak = false;};
	bool hasSoftBreak() const { return softBreak; };
	void setFermata() { fermata = true; };
	bool hasFermata() const { return fermata; };
	void setFreeText() { freeText=true; };
	bool hasFreeText() const { return freeText; };
	std::string getFreeText() const ;
	void repairBarline();
	
	TimeSignature getTimeSignature() const; //do only invoke if identity is TIME_COMMAND
	
	//pitchclass, tiestatus, accidental and braces are already known. If token is rest ('r' or 's'), octave en slur not taken into account.
	string createKernNote(int octave, int duration, int dots, bool triplet, SlurStatus slur, TieStatus tie, bool opensub, bool closesub, GraceType gt) const;
	string createKernSingleNote(int octave, int duration, int dots, bool triplet, SlurStatus slur, TieStatus tie, bool opensub, bool closesub, GraceType gt) const;
	string createKernChordNote(int octave, int duration, int dots, bool triplet, SlurStatus slur, TieStatus tie, bool opensub, bool closesub, GraceType gt) const;
	string createAbsLyNote(int octave, int duration, int dots, SlurStatus slur, TieStatus tie) const;

	string getKernBarLine(const int barnumber, const bool unnumbered=false) const;
	string getKernBarLineVisual() const;

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
	bool beginGlissando;
	bool fermata;
	bool freeText;
	vector<RelLyToken> notes; //contains the notes of a chord
	vector<Ornament> ornaments;
	vector<Articulation> articulations;
};

#endif
