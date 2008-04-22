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

RelLyToken::RelLyToken(string t, string loc, string wcepos, RelLyToken::Identity token_id, bool is_music) : token(t), id(token_id), location(loc), WCEPosition(wcepos) {
	
	//id = computeIdentity(is_music);
	//cout << token << ": " << printIdentity(id) << endl;
}

RelLyToken::RelLyToken(const RelLyToken& r) : token(r.getToken()), id(r.getIdentity()), WCEPosition(r.WCEPosition) {
	//id = computeIdentity();
}

RelLyToken::RelLyToken() : token("") {
	id = UNKNOWN;
}

RelLyToken& RelLyToken::operator=(const RelLyToken& r) {
	if ( &r != this ) {
	}
	cerr << "Waarschuwing: RelLyToken& RelLyToken::operator=(const RelLyToken& r) gebruikt!!!!" << endl;
	assert(false);
	return *this;
}

RelLyToken::Identity RelLyToken::computeIdentity(bool is_music) const {
	
	// if not music (dus text), return TEXT
	if ( !is_music ) return TEXT;
		
	Identity res = UNKNOWN;
	
	//cout << token << endl;
	
	string lt = token; //changable copy
	pvktrim(lt); //to be sure

	//NB first check for 'times' and then for 'time', because 'time' is in 'times'
	
	string::size_type pos;
	if( ( (pos = lt.find("\\times 2/3")) != string::npos) || ( (pos = lt.find("\\times2/3")) != string::npos) ) {
		//make sure there is only the times command with maybe a brace.
		//cout << lt << " - " << flush;
		if ( (pos = lt.find("\\times 2/3")) != string::npos) lt.erase(pos, 10);
		if ( (pos = lt.find("\\times2/3")) != string::npos) lt.erase(pos, 9);
		//cout << lt << endl << flush;
		pvktrim(lt);
		if ( lt.find_first_of("abcdefgrs") == string::npos ) { // there is no note
			res = TIMES_COMMAND;
			return res;
		} else {
			cerr << location << ": Warning: Unknown token: " << token << endl;
			return UNKNOWN;
		}
	}
	if( (pos = lt.find("\\time")) != string::npos ) {
		// make sure there is no note after the time command
		//cout << lt << " - ";
		lt.erase(pos, 5);
		pvktrim(lt);
		//cout << lt << endl;
		if ( lt.find_first_of("abcdefgrs") == string::npos ) { // there is no note
			res = TIME_COMMAND;
			return res;
		} else {
			cerr << location << ": Warning: Unknown token: " << token << endl;
			return UNKNOWN;
		}
	}
	
	//now find out if it is a note
	//remove all allowed nonnote characters so that only the notename remains
	//cout << lt << " - ";
	pvktrim(lt);
	while( (pos = lt.find_first_of("{}().~0123456789,' ")) != string::npos )
		{ lt.erase(pos,1); }
	//cout << lt << " - ";
	if ( (pos = lt.find("\\x")) != string::npos ) {
		lt.erase(pos,2);
		//if ( pos != 0 ) cerr << location << ": Warning: \\x not at beginning of token " << token << endl; //not good in case of \gl\x or { \x
	}
	//cout << lt << " - ";
	if ( (pos = lt.find("\\gl")) != string::npos ) {
		lt.erase(pos,3);
		if ( pos != 0 )
			cerr << location << ": Warning: \\gl not at beginning of token " << token << endl;
	}
	//cout << lt << endl;
	//now there sould be only one pitch, rest ('r') or space ('s') left
	if ( lt.find_first_of("abcdefgrs") != string::npos ) { res = NOTE; return res; }
	
	//warning for empty token // only if in melody ---> moved to SongLine::breakWcelines()
	//take new trim of token
	//lt = token;
	//pvktrim(lt);
	//if ( is_music && (lt.size() == 0) )
	//	cerr << location << ": Warning: empty token " << token << endl;
	
	return res;
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

string RelLyToken::createKernNote(int octave, int duration, int dots, bool triplet, SlurStatus slur, TieStatus tie) const {
	stringstream res;
	//open phrase
	//open slur
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
	//phrase close
	
	string s;
	res >> s;
	return s;
	
}

int RelLyToken::getDurationBase() const {
	if (getIdentity() != NOTE ) return -1;
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
	string lt = token; //changable
	string::size_type pos;
	int res = 0;
	while( (pos = lt.find("'")) != string::npos ) { res++; lt.erase(pos,1); }
	while( (pos = lt.find(",")) != string::npos ) { res--; lt.erase(pos,1); }
	return res;
}

char RelLyToken::getPitchClass() const {
	if (getIdentity() != NOTE) return '.'; //not sure what to return
	string::size_type pos;
	//remove \x or \gl
	string t = token;
	if ( (pos = t.find("\\x")) != string::npos ) t.erase(pos,2);
	if ( (pos = t.find("\\gl")) != string::npos ) t.erase(pos,3); 
	if ( (pos = t.find_first_of("abcdefgsr")) == string::npos) return '.';
	return (char)t[pos];
}

RelLyToken::SlurStatus RelLyToken::getSlur() const {
	if ( id != NOTE ) return NO_SLUR_INFO;
	SlurStatus res = NO_SLUR;
	string::size_type pos;
	if ( (pos = token.find("(")) != string::npos) res = START_SLUR;
	if ( (pos = token.find(")")) != string::npos) res = END_SLUR;
	return res;
}

RelLyToken::TieStatus RelLyToken::getTie() const {
	if ( id != NOTE ) return NO_TIE_INFO;
	if ( token.find("~") != string::npos ) return START_TIE; else return NO_TIE;
	//never used:
	return NO_TIE_INFO;
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
		case BARLINE: return "BARLINE";
		case UNKNOWN: return "UNKNOWN";
	}
	return "";
}

bool RelLyToken::isRest() const {
	bool res = false;
	
	if ( getIdentity() == NOTE ) {
		if ( token.find_first_of("r") != string::npos ) { res = true; }
	}
	
	return res;
}

