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
using namespace std;

RelLyToken::RelLyToken(string t) : token(t) {
}

RelLyToken::RelLyToken(const RelLyToken& r) : token(r.getToken()) {
}

RelLyToken::RelLyToken() : token("") {
}

RelLyToken& RelLyToken::operator=(const RelLyToken& r) {
	if ( &r != this ) {
	}
	cerr << "Waarschuwing: RelLyToken& RelLyToken::operator=(const RelLyToken& r) gebruikt!!!!" << endl;
	return *this;
}

RelLyToken::Identity RelLyToken::getIdentity() const {
	Identity res = UNKNOWN;
	
	string lt = token; //changable copy
	pvktrim(lt); //to be sure
	
	string::size_type pos;
	if( pos = lt.find("\\time ") != string::npos ) {
		// make sure there is no note after the time command
		lt.erase(pos, 6);
		pvktrim(lt);
		if ( lt.find_first_of("abcdefgrs") == string::npos ) { // there is no note
			res = TIME_COMMAND;
			return res;
		}
	}
	if( pos = lt.find("\\times 2/3") != string::npos ) {
		//make sure there is only the times command with maybe a brace.
		lt.erase(pos, 10);
		pvktrim(lt);
		if ( lt.find_first_of("abcdefgrs") == string::npos ) { // there is no note
			res = TIMES_COMMAND;
			return res;
		}
	}
	//now find out if it is a note
	//remove all allowed nonnote characters so that only the notename remains
	while( (pos = lt.find_first_of("{}().~0123456789,' ")) != string::npos )
		{ lt.erase(pos,1); }
	while( (pos = lt.find("is") != string::npos )) lt.erase(pos,2);
	while( (pos = lt.find("es") != string::npos )) lt.erase(pos,2);
	//now there sould be only one pitch, rest ('r') or space ('s') left
	if ( lt.find_first_of("abcdefgrs") != string::npos && lt.size()==1 ) { res = NOTE; return res; }
	
	//unknown
	return res;
}

string RelLyToken::createAbsLyNote(int octave, int duration, bool dotted, SlurStatus slur, TieStatus tie) const {
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
	if (dotted) res << ".";
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

string RelLyToken::createKernNote(int octave, int duration, bool dotted, bool triplet, SlurStatus slur, TieStatus tie) const {
	stringstream res;
	//open phrase
	//open slur
	if (slur == START_SLUR) res << "(";
	//open tie
	if (tie == START_TIE) res << "[";
	//duration
	if (triplet) duration = duration / 4 * 3;
	res << duration;
	if (dotted) res << ".";
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
		}
	}
	//accidental
	Accidental ac = getAccidental();
	switch(ac) {
		case DOUBLE_FLAT:  res << "--"; break;
		case FLAT:         res << "-"; break;
		case SHARP:        res << "#"; break;
		case DOUBLE_SHARP: res << "##"; break;
	}
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
	if (getIdentity() != NOTE ) return 0;
	string duration = "";
	string::size_type pos;
	string lt = token; //changable
	while( (pos = lt.find_first_of("123468")) != string::npos ) { duration = duration + lt[pos]; lt.erase(0,pos+1); }
	return atoi( duration.c_str() );
}

bool RelLyToken::getDotted() const { //return false if no duration is given
	bool res = false;
	if (token.find(".") != string::npos ) res = true; //true: dot found
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
	if ( (pos = token.find_first_of("abcdefgsr")) == string::npos) return '.';
	return (char)token[pos];
}

RelLyToken::SlurStatus RelLyToken::getSlur() const {
	SlurStatus res = NO_SLUR_INFO;
	string::size_type pos;
	if ( (pos = token.find("(")) != string::npos) res = START_SLUR;
	if ( (pos = token.find(")")) != string::npos) res = END_SLUR;
	return res;
}

RelLyToken::TieStatus RelLyToken::getTie() const {
	if ( token.find("~") != string::npos ) return START_TIE; else return NO_TIE_INFO;
}

RelLyToken::Accidental RelLyToken::getAccidental() const {
	int res = 0;
	string lt = token;
	string::size_type pos;
	while( (pos = lt.find("is")) != string::npos ) { res++; lt.erase(pos,2); }
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