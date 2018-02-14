/*
 *  RelLyToken.cpp
 *  wce2krn
 *
 *  Created by Peter Van Kranenburg on 10/31/06.
 *  Copyright 2006P Peter Van Kranenburg. All rights reserved.
 *
 */

#include "RelLyToken.h"
#include "pvkutilities.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cctype>
#include <cassert>
using namespace std;

//used to split chord notes
#undef yyFlexLexer
#define yyFlexLexer ChordNoteFlexLexer
#include <FlexLexer.h>

RelLyToken::RelLyToken(string t, string loc, int lineno, int linepos, RelLyToken::Identity token_id, bool hassoftbreak, bool is_music) : token(t), id(token_id), location(loc), WCE_LineNumber(lineno), WCE_Pos(linepos), softBreak(hassoftbreak) {
	if (getIdentity() == CHORD) {
			notes = splitChord();
	}
}

RelLyToken::RelLyToken(const RelLyToken& r) : token(r.getToken()), id(r.getIdentity()), WCE_LineNumber(r.getWCE_LineNumber()), WCE_Pos(r.getWCE_Pos()), softBreak(r.hasSoftBreak()), location(r.getLocation()), notes(r.getNotes()) {

}

RelLyToken::RelLyToken() : token(""), id(UNKNOWN), location(""), WCE_LineNumber(0), WCE_Pos(0), softBreak(false) {

}

void RelLyToken::addTie() {
	token = token + "~";
}

void RelLyToken::addClosingBrace() {
	token = token + " }";
}

void RelLyToken::addSlurBegin() {
	token = token + "(";
}

void RelLyToken::addSlurEnd() {
	token = token + ")";
}

RelLyToken& RelLyToken::operator=(const RelLyToken& r) {
	if ( &r != this ) {
	}
	cerr << "Waarschuwing: RelLyToken& RelLyToken::operator=(const RelLyToken& r) gebruikt!!!!" << endl;
	assert(false);
	return *this;
}


RelLyToken::Identity RelLyToken::getIdentity() const {
	return id;
}

string RelLyToken::createAbsLyNote(int octave, int duration, int dots, SlurStatus slur, TieStatus tie) const {

	// what about (, [ etc.

	stringstream res;
	//first pitch:
	res << getPitchClass();
	//accidentals
	Accidental ac = getAccidental();
	switch(ac) {
		case DOUBLE_FLAT:  res << "eses"; break;
		case FLAT:         res << "es"; break;
		case SHARP:        res << "is"; break;
		case DOUBLE_SHARP: res << "isis"; break;
		default: {};
	}
	//octave
	if ( getPitchClass() != 'r' && getPitchClass() != 's' ) {
		octave = octave - 3;
		if (octave !=0 ) {
			for (int i=1; i<=abs(octave); i++) {
				if (octave<0) res << ","; else res << "'";
			}
		}
	}
	//notelength
	res << duration;
	for (int i=0; i<dots; i++) res << ".";
	//tie
	if( tie == START_TIE || tie == CONTINUE_TIE ) res << "~";
	//slur
	switch(slur) {
		case START_SLUR: res << "("; break;
		case END_SLUR: res << ")"; break;
		case ENDSTART_SLUR: res << ")("; break;
		default: {}
	}
	
	string s;
	res >> s;
	
	//DON'T USE RES ANYMORE!!!!
	
	string::size_type pos1, pos2;
	//NB don't forget to copy open and close braces!!!
	if ( containsOpeningBrace()) {
		pos1 = token.find("{");
		pos2 = token.find_first_of("abcdefgrs");
		if ( pos1 < pos2 ) s = "{ " + s; else s = s + " {";
	}
	if ( containsClosingBrace()) {
		pos1 = token.find("}");
		pos2 = token.find_first_of("abcdefgrs");
		if ( pos1 < pos2 ) s = "} " + s; else s = s + " }";
	}	
	
	return s;
}

int RelLyToken::computeOctave(int previous_octave, char previous_pitch, int octcorrection, char pitch) const {

	/*
	if (getIdentity()==CHORD) {
		cout << "previous_octave: " << previous_octave << endl;
		cout << "previous_pitch: " << previous_pitch << endl;
		cout << "octcorrection: " << octcorrection << endl;
		cout << "pitch: " << pitch << endl;
	}
	*/

	int res = previous_octave;
	const char relativeRoot=((pitch>='c') ? pitch : (pitch+7)); // representation shifted for a and b
	const char lastRelativeRoot=((previous_pitch>='c') ? previous_pitch : (previous_pitch+7));

	if ( pitch != 'r' && pitch != 's' ) {
		int diff = relativeRoot - lastRelativeRoot;
		if (abs(diff) > 3) {
			if (diff > 0 ) res--; else res++;
		}
	}

	res += octcorrection;

	/*
	if (getIdentity()==CHORD) {
		cout << "res_octave: " << res << endl;
	}
	*/

	return res;
}


string RelLyToken::createKernNote(int octave, int duration, int dots, bool triplet, SlurStatus slur, TieStatus tie, bool opensub, bool closesub, GraceType gt) const {
	if (getIdentity() == NOTE ) {
		return createKernSingleNote(octave, duration, dots, triplet, slur, tie, opensub, closesub, gt);
	} else if (getIdentity() == CHORD) {
		return createKernChordNote(octave, duration, dots, triplet, slur, tie, opensub, closesub, gt);
	}
	else {
		return ".";
	}
}

string RelLyToken::createKernChordNote(int octave_of_first, int duration, int dots, bool triplet, SlurStatus slur, TieStatus tie, bool opensub, bool closesub, GraceType gt) const {
	//cerr << location << " Warning: Only first note of chord exported to **kern: " << token << endl;
	if (notes.size() > 0) {
		vector<Pitchclass_Octave> allNotes = getAllOfChord(octave_of_first);
		int ih = getIndexHighestOfChord();
		//cout << ih << endl;
		return notes[ih].createKernNote(allNotes[ih].octave, duration, dots, triplet, slur, tie, opensub, closesub, gt);
	}
	else {
		cerr << location << " Error: RelLyToken::createKernChordNote() invoked, while notes.size() == " << notes.size() << endl;
		exit(1);
	}
}

string RelLyToken::createKernSingleNote(int octave, int duration, int dots, bool triplet, SlurStatus slur, TieStatus tie, bool opensub, bool closesub, GraceType gt) const {
	stringstream res;
	string editorial = "";
	//open sub
	//open phrase
	//open slur
	//if ( opensub ) res << "{"; // <- is now done with breath mark after note
	if (slur == START_SLUR || slur == ENDSTART_SLUR) res << "(";
	//open tie
	if (tie == START_TIE) res << "[";
	//duration
	if (triplet) duration = duration / 2 * 3;
	res << duration;
	for (int i=0; i<dots; i++) res << ".";
	//pitch and octave
	char pc = getPitchClass();
	if ( pc == 'r' ) {
		res << "r";
	} else if ( pc == 's' ) {
		res << "r";
		editorial = "yy";
	} else {
		char pc_up = toupper(pc);
		octave = octave - 3;
		if (octave !=0 ) {
			for (int i=1; i<=abs(octave); i++) {
				if (octave<0) res << pc_up; else res << pc;
			}
		} else
			res << pc_up; //octave-3=0
	}
	//accidental
	Accidental ac = getAccidental();
	switch(ac) {
		case DOUBLE_FLAT:  res << "--"; break;
		case FLAT:         res << "-"; break;
		case SHARP:        res << "#"; break;
		case DOUBLE_SHARP: res << "##"; break;
		default: {}
	}
	//Grace note?
	switch (gt) {
	case NOGRACE: break;
	case PLAINGRACE: res << "qq"; break;
	case APP: res << "qq"; break;
	case ACC: res << "q"; break;
	case AFTER: res << "qq"; break;
	}
	//Glissando end?
	if (getGlissandoEnd()) // yes
		res << "H";
	//Interpreted pitch?
	if (getInterpretedPitch()) // yes
		res << "x";
	//tie close
	if (tie == END_TIE) res << "]";
	if (tie == CONTINUE_TIE) res << "_";
	//slur close
	if (slur == END_SLUR || slur == ENDSTART_SLUR) res << ")";
	
	//subphrase
	//if ( closesub ) res << "}";
	//if ( closesub ) res << ",";
	if ( closesub ) res << ";"; //fermata can be imported by music21

	res << editorial;

	string s;
	res >> s;
	return s;
	
}

int RelLyToken::getDurationBase() const {
	if (getIdentity() != NOTE && getIdentity() != CHORD) return -1;
	string duration = "";
	string::size_type pos;
	string lt = token; //changable
	while( (pos = lt.find_first_of("123468")) != string::npos ) { duration = duration + lt[pos]; lt.erase(0,pos+1); }
	return atoi( duration.c_str() );
}

int RelLyToken::getDots() const { //return number of dots
	int res = 0;
	string::size_type pos;
	string lt = token;
	while( (pos = lt.find(".")) != string::npos ) { res++; lt.erase(pos,1); }
	return res;
}

bool RelLyToken::getNotDotted() const { //return true if no duration is given
	bool res = true;
	if (token.find(".") == string::npos ) res = false; //true: dot not found
	return res;
}

string RelLyToken::getClefType() const { //return the clef type for a clef change.
	string res = "treble";
	if (token.find("bass") != string::npos ) res = "bass";
	return res;
}

// in case of chord. Give Octave correction of first note
int RelLyToken::getOctaveCorrection() const {
	if (getIdentity() == NOTE) {
		string lt = token; //changable
		string::size_type pos;
		int res = 0;
		while( (pos = lt.find("'")) != string::npos ) { res++; lt.erase(pos,1); }
		while( (pos = lt.find(",")) != string::npos ) { res--; lt.erase(pos,1); }
		return res;
	}
	else if (getIdentity() == CHORD) {
		if (notes.size() > 0)
			return notes[0].getOctaveCorrection();
		else
			return 0;
	}
	else
		return 0;
}

vector<RelLyToken> RelLyToken::splitChord() const {
	vector<RelLyToken> notes;
	string flexline = token;
	istringstream iss(flexline);
	FlexLexer* lexer;
	lexer = new ChordNoteFlexLexer(&iss);
	int tok = lexer->yylex();
	while(tok != 0){
		//cout << tok << " " << lexer->YYText() << endl;
		if (tok == 2) {
			  string ctoken = lexer->YYText();
			  pvktrim(ctoken);
			  RelLyToken rlt = RelLyToken(ctoken, location, WCE_LineNumber, WCE_Pos, RelLyToken::NOTE, false, true);
			  notes.push_back(rlt);
		}
		tok = lexer->yylex();
	}

	//for (int i=0; i<notes.size(); i++)
	//	cout << notes[i].getToken() << " ";
	//cout << endl;

	if (notes.size() < 2 ) {
		cerr << location << " Error: Chord contains " << notes.size() << " note(s): " << token << endl;
		exit(1);
	}

	return notes;
}

//int previous_octave, char previous_pitch, int octcorrection, char pitch
vector<Pitchclass_Octave> RelLyToken::getAllOfChord(int initialoctave) const {
	vector<Pitchclass_Octave> res;
	for (int i=0; i<notes.size(); i++) {
		int oct;
		if ( i==0 )
			oct = initialoctave;
		else
			oct = computeOctave( res.back().octave, res.back().pitchclass, notes[i].getOctaveCorrection(), notes[i].getPitchClass() );
		char pc = notes[i].getPitchClass();
		Pitchclass_Octave note = Pitchclass_Octave( pc, oct );
		res.push_back(note);
	}
	return res;
}

// c d e f g a b ->

int RelLyToken::getIndexHighestOfChord() const {
	vector<Pitchclass_Octave> all = getAllOfChord(0);
	int res = -1;
	Pitchclass_Octave max;

	max.octave = 0;
	max.pitchclass = 'c';

	if ( all.size() > 0 ) {
		max.octave = all[0].octave;
		max.pitchclass = all[0].pitchclass;
		res = 0;
	} else {
		cerr << location << " Error: Empty chord" << endl;
		exit(1);
	}

	for ( int i=0; i<all.size(); i++) {
		if (all[i].octave > max.octave) {
			max = all[i];
			res = i;
		}
		if (all[i].octave == max.octave) {
			char cmp_note = all[i].pitchclass;
			if ( cmp_note < 'c' ) cmp_note += 5; // convert a and b to h and i
			char cmp_max = max.pitchclass;
			if ( cmp_max < 'c' ) cmp_max += 5;
			if ( cmp_note > cmp_max ) {
				max = all[i];
				res = i;
			}
		}
		//cout << i << " " << res << endl;
	}
	return res;
}

char RelLyToken::getPitchClass() const {
	if (getIdentity() == NOTE) {
		string::size_type pos;
		//remove \x or \gl
		string t = token;
		if ( (pos = t.find("\\x")) != string::npos ) t.erase(pos,2);
		if ( (pos = t.find("\\gl")) != string::npos ) t.erase(pos,3);
		if ( (pos = t.find_first_of("abcdefgsr")) == string::npos) return '.';
		return (char)t[pos];
	}
	else if (getIdentity() == CHORD ) { //return pitch class of highest note
		if (notes.size() > 0) {
			int ih = getIndexHighestOfChord();
			return notes[ih].getPitchClass();
		}
		else
			return '.';
	}
	else
		return '.';
}

//return the index of the pitch class in the token string
string::size_type RelLyToken::getPosOfPitchClass() const {
	if (getIdentity() == NOTE) {
		string::size_type pos;
		//remove \x or \gl //not remove, this will change position. Change to something not a pitch class
		string t = token;
		//if ( (pos = t.find("\\x")) != string::npos ) t.erase(pos,2);
		if ( (pos = t.find("\\gl")) != string::npos ) t[pos+1] = 'x'; // change \gl to \xl
		if ( (pos = t.find_first_of("abcdefgsr")) == string::npos) return pos;
	}
	else if (getIdentity() == CHORD ) { //return pos pitch class of highest note
		if (notes.size() > 0) {
			int ih = getIndexHighestOfChord();
			return notes[ih].getPosOfPitchClass();
		}
		else
			return string::npos;
	}
	return string::npos;
}

RelLyToken::SlurStatus RelLyToken::getSlur() const {
	if ( id != NOTE && id != CHORD ) return NO_SLUR_INFO;
	SlurStatus res = NO_SLUR;
	string::size_type pos;
	bool open = false;
	bool close = false;
	if ( (pos = token.find("(")) != string::npos) open = true;
	if ( (pos = token.find(")")) != string::npos) close = true;
	if (open && !close) res = START_SLUR;
	if (!open && close) res = END_SLUR;
	if (open && close) res = ENDSTART_SLUR;
	return res;
}

RelLyToken::TieStatus RelLyToken::getTie() const {
	if ( id != NOTE && id != CHORD) return NO_TIE_INFO;
	if ( token.find("~") != string::npos ) return START_TIE; else return NO_TIE;
	//never used:
	return NO_TIE_INFO;
}

//NOBARLINE, NORMALBAR, ENDBAR, DOUBLEBAR, BEGINREPEAT, ENDREPEAT, DOUBLEREPEAT

void RelLyToken::getBarLinePositions(string::size_type & pos1, string::size_type & pos2) const {
	if (id != BARLINE ) return;
	pos1 = token.find_first_of("\"") +1;
	pos2 = token.find_last_of("\"") -1;
}

RelLyToken::BarLineType RelLyToken::getBarLineType() const {
	if (id != BARLINE ) return NOBARLINE;
	//string::size_type pos1 = token.find_first_of("\"");
	//string::size_type pos2 = token.find_last_of("\"");
	string::size_type pos1, pos2;
	getBarLinePositions(pos1, pos2);
	if ( pos1 == string::npos || pos2 == string::npos) {
		cerr << "Error in barline token: " << endl;
		exit(-1);
	}
	if ( pos2 < pos1 ) {
		cerr << "Error in barline token: " << endl;
		exit(-1);
	}
	string::size_type len = pos2 - pos1 + 1;
	string barline = token.substr(pos1,len);
	pvktrim(barline);

	// cout << "Barline: " << barline << endl;

	if ( barline == "|:" )
		cerr << location << "Warning: barline |: is deprecated. New code is: .|:";
	if ( barline == ":|" )
		cerr << location << "Warning: barline :| is deprecated. New code is: :|.";
	if ( barline == ":|:" )
		cerr << location << "Warning: barline :|: is deprecated. New code is: :..:";

	if ( barline == "|" ) return NORMALBAR;
	else if ( barline == "|." ) return ENDBAR;
	else if ( barline == "||" ) return DOUBLEBAR;
	else if ( barline == "|:" || barline ==".|:" ) return BEGINREPEAT;
	else if ( barline == ":|" || barline ==":|." ) return ENDREPEAT;
	else if ( barline == ":|:" || barline ==":..:" ) return DOUBLEREPEAT;
	else { return UNKNOWNBAR; }

	return NOBARLINE;
}

RelLyToken::GraceType RelLyToken::getGraceType() const {
	if (id != GRACE) return NOGRACE;
	GraceType res = NOGRACE;
	string::size_type pos;
	if ( (pos = token.find("\\app")) != string::npos) res = APP; // --> kern qq (LEGACY)
	if ( (pos = token.find("\\kvs")) != string::npos) res = ACC; // --> kern q
	if ( (pos = token.find("\\grace")) != string::npos) res = PLAINGRACE; // --> kern qq
	if ( (pos = token.find("\\afterGrace")) != string::npos) res = AFTER; // --> kern qq
	return res;
}

string RelLyToken::getKernBarLine(const int barnumber, const bool unnumbered) const {

	//cout << "creating barline " << getBarLineType() << endl;

	stringstream ss;
	switch(getBarLineType()) {
	case NOBARLINE:
	case UNKNOWNBAR:
		ss << "."; break;
	case NORMALBAR:
	case BEGINREPEAT:
	case ENDREPEAT:
	case DOUBLEREPEAT:
		ss << "="; break;
	case DOUBLEBAR:
	case ENDBAR:
		ss << "=="; break;
	}

	if (!unnumbered)
		ss << barnumber;

	string res = "";
	switch(getBarLineType()) {
	case UNKNOWNBAR: ss << ""; break;
	case NOBARLINE: ss << ""; break;
	case NORMALBAR: ss << "|"; break;
	case ENDBAR: ss << "|!"; break;
	case DOUBLEBAR: ss << "||"; break;
	case BEGINREPEAT: ss << "!|:"; break;
	case ENDREPEAT: ss << ":|!"; break;
	case DOUBLEREPEAT: ss << ":!!:"; break;
	}

	string barstr = "";
	ss >> barstr;

	//cout << "ready: " << barstr << endl;

	return barstr;
}


RelLyToken::Accidental RelLyToken::getAccidental() const {
	
	//NB special care for as, asas, es and eses
	
	int res = 0;
	string lt = token;
	string::size_type pos;
	while( (pos = lt.find("is")) != string::npos ) { res++; lt.erase(pos,2); }
	while( (pos = lt.find("as")) != string::npos ) { res--; lt.erase(pos,2); } // to find as and ases
	while( (pos = lt.find("es")) != string::npos ) { res--; lt.erase(pos,2); }
	switch(res) {
		case -2: return DOUBLE_FLAT; break;
		case -1: return FLAT; break;
		case  0: return NO_ACCIDENTAL; break;
		case  1: return SHARP; break;
		case  2: return DOUBLE_SHARP; break;
		default: return NO_ACCIDENTAL;
	}
	return NO_ACCIDENTAL;
}

bool RelLyToken::getInterpretedPitch() const {
	bool res = false;
	if ( token.find("\\x") != string::npos ) // \x found
		res = true;
	return res;
}

bool RelLyToken::getGlissandoEnd() const {
	bool res = false;
	string::size_type pos;
	if ( (pos = token.find("\\gl")) != string::npos ) res = true;
	return res;	
}


bool RelLyToken::containsClosingBraceBeforeNote() const {
	bool res = false;
	string::size_type pos1, pos2;
	if ( containsClosingBrace()) {
		pos1 = token.find("}");
		pos2 = token.find_first_of("abcdefgrs");
		if ( pos1 != string::npos && pos2 != string::npos && pos1 < pos2 ) res = true;
	}
	return res;
}

bool RelLyToken::containsClosingBraceAfterNote() const {
	bool res = false;
	string::size_type pos1, pos2;
	if ( containsClosingBrace()) {
		pos1 = token.find("}");
		pos2 = token.find_first_of("abcdefgrs");
		if ( pos1 != string::npos && pos2 != string::npos && pos1 > pos2 ) res = true;
	}
	return res;
}

TimeSignature RelLyToken::getTimeSignature() const {
	return TimeSignature(token);
}

string RelLyToken::printSlurStatus(SlurStatus ss) {
	switch ( ss ) {
		case NO_SLUR_INFO: return "NO_SLUR_INFO";
		case START_SLUR: return "START_SLUR";
		case END_SLUR: return "END_SLUR";
		case ENDSTART_SLUR: return "ENDSTART_SLUR";
		case IN_SLUR: return "IN_SLUR";
		case NO_SLUR: return "NO_SLUR";
	}
	return "";
}

string RelLyToken::printTieStatus(TieStatus ts) {
	switch ( ts ) {
		case NO_TIE_INFO: return "NO_TIE_INFO";
		case START_TIE: return "START_TIE";
		case CONTINUE_TIE: return "CONTINUE_TIE";
		case END_TIE: return "END_TIE";
		case NO_TIE: return "NO_TIE";
	}
	return "";
}

string RelLyToken::printTextStatus(TextStatus ts) {
	switch ( ts ) {
		case SINGLE_WORD: return "SINGLE_WORD";
		case SINGLE_WORD_CONT: return "SINGLE_WORD_CONT";
		case BEGIN_WORD: return "BEGIN_WORD";
		case BEGIN_WORD_CONT: return "BEGIN_WORD_CONT";
		case END_WORD: return "END_WORD";
		case END_WORD_CONT: return "END_WORD_CONT";
		case IN_WORD: return "IN_WORD";
		case IN_WORD_CONT: return "IN_WORD_CONT";
		case NO_WORD: return "NO_WORD";
		case DONTKNOW: return "DONTKNOW";
	}
	return "";
}

string RelLyToken::printIdentity(Identity i) {
	switch ( i ) {
		case NOTE: return "NOTE";
		case TIME_COMMAND: return "TIME_COMMAND";
		case TIMES_COMMAND: return "TIMES_COMMAND";
		case TEXT: return "TEXT";
		case GRACE: return "GRACE";
		case CHORD: return "CHORD";
		case BARLINE: return "BARLINE";
		case CLEF_COMMAND: return "CLEF_COMMAND";
		case UNKNOWN: return "UNKNOWN";
	}
	return "";
}

bool RelLyToken::isRest() const {
	bool res = false;
	
	if ( getIdentity() == NOTE ) {
		if ( token.find_first_of("r") != string::npos ) { res = true; }
		if ( getAccidental() == NO_ACCIDENTAL ) // 's' is also in 'es' and 'is'
			if ( token.find_first_of("s") != string::npos ) { res = true; }
	}
	return res;
}

