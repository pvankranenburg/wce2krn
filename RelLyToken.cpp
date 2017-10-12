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

}

RelLyToken::RelLyToken(const RelLyToken& r) : token(r.getToken()), id(r.getIdentity()), WCE_LineNumber(r.getWCE_LineNumber()), WCE_Pos(r.getWCE_Pos()), softBreak(r.hasSoftBreak()), location(r.getLocation()) {

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

string RelLyToken::createKernNote(int octave, int duration, int dots, bool triplet, SlurStatus slur, TieStatus tie, bool opensub, bool closesub) const {
	if (getIdentity() == NOTE ) {
		return createKernSingleNote(octave, duration, dots, triplet, slur, tie, opensub, closesub);
	}
	else if (getIdentity() == CHORD) {
		return createKernChordNote(octave, duration, dots, triplet, slur, tie, opensub, closesub);
	}
	else {
		return ".";
	}
}

string RelLyToken::createKernChordNote(int octave, int duration, int dots, bool triplet, SlurStatus slur, TieStatus tie, bool opensub, bool closesub) const {
	cerr << location << " Warning: Only first note of chord exported to **kern: " << token << endl;
	vector<RelLyToken> notes = splitChord();
	return notes[0].createKernNote(octave, duration, dots, triplet, slur, tie, opensub, closesub);
}

string RelLyToken::createKernSingleNote(int octave, int duration, int dots, bool triplet, SlurStatus slur, TieStatus tie, bool opensub, bool closesub) const {
	stringstream res;
	//open sub
	//open phrase
	//open slur
	//if ( opensub ) res << "{"; // <- is now done with breath mark after note
	if (slur == START_SLUR) res << "(";
	//open tie
	if (tie == START_TIE) res << "[";
	//duration
	if (triplet) duration = duration / 2 * 3;
	res << duration;
	for (int i=0; i<dots; i++) res << ".";
	//pitch and octave
	char pc = getPitchClass();
	if ( pc == 'r' || pc == 's' )
		res << 'r';
	else {
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
	if (slur == END_SLUR) res << ")";
	
	//subphrase
	//if ( closesub ) res << "}";
	//if ( closesub ) res << ",";
	if ( closesub ) res << ";"; //fermata can be imported by music21

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
		vector<RelLyToken> notes = splitChord();
		return notes[0].getOctaveCorrection();
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

	return notes;
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
	else if (getIdentity() == CHORD ) { //return pitch class of first note ??
		vector<RelLyToken> notes = splitChord();
		return notes[0].getPitchClass();
	}
	else
		return '.';
}

RelLyToken::SlurStatus RelLyToken::getSlur() const {
	if ( id != NOTE && id != CHORD ) return NO_SLUR_INFO;
	SlurStatus res = NO_SLUR;
	string::size_type pos;
	if ( (pos = token.find("(")) != string::npos) res = START_SLUR;
	if ( (pos = token.find(")")) != string::npos) res = END_SLUR;
	return res;
}

RelLyToken::TieStatus RelLyToken::getTie() const {
	if ( id != NOTE && id != CHORD) return NO_TIE_INFO;
	if ( token.find("~") != string::npos ) return START_TIE; else return NO_TIE;
	//never used:
	return NO_TIE_INFO;
}

//NOBARLINE, NORMALBAR, ENDBAR, DOUBLEBAR, BEGINREPEAT, ENDREPEAT, DOUBLEREPEAT

RelLyToken::BarLineType RelLyToken::getBarLineType() const {
	if (id != BARLINE ) return NOBARLINE;
	string::size_type pos1 = token.find_first_of("\"");
	string::size_type pos2 = token.find_last_of("\"");
	if ( pos1 == string::npos || pos2 == string::npos) {
		cerr << "Error in barline token: " << endl;
		exit(-1);
	}
	if ( pos2 <= pos1 ) {
		cerr << "Error in barline token: " << endl;
		exit(-1);
	}
	string::size_type len = pos2 - pos1;
	string barline = token.substr(pos1,len);
	pvktrim(barline);
	if ( barline == "|" ) return NORMALBAR;
	else if ( barline == "|." ) return ENDBAR;
	else if ( barline == "||" ) return DOUBLEBAR;
	else if ( barline == "|:" ) return BEGINREPEAT;
	else if ( barline == ":|" ) return ENDREPEAT;
	else if ( barline == ":|:" ) return DOUBLEREPEAT;

	return NOBARLINE;
}

string RelLyToken::getKernBarLine() const {
	string res = "";
	switch(getBarLineType()) {
	case NOBARLINE: res = "|"; break;
	case NORMALBAR: res = "="; break;
	case ENDBAR: res = "==|!"; break;
	case DOUBLEBAR: res = "=="; break;
	case BEGINREPEAT: res = "==!|:"; break;
	case ENDREPEAT: res = "==:|!"; break;
	case DOUBLEREPEAT: res = "==:!!:"; break;
	}
	return res;

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
		case STOPBAR: return "STOPBAR";
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

