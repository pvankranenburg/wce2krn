/*
 *  LilyLine.cpp
 *  wce2krn
 *
 *  Created by Peter Van Kranenburg on 10/27/06.
 *  Copyright 2006 Peter van Kranenburg. All rights reserved.
 *
 */

#include "SongLine.h"
#include "pvkutilities.h"
#include<string>
#include<iostream>
#include<cstdlib>
using namespace std;

SongLine::SongLine(vector<string> lines, RationalTime upb, TimeSignature timesig, int duration, bool dotted, int octave, char pitchclass, int barnumber) :
																   wcelines(lines),
																   initialUpbeat(upb),
																   initialTimeSignature(timesig),
																   initialDuration(duration),
																   initialDotted(dotted),
																   initialOctave(octave),
																   initialLastPitchClass(pitchclass),
																   initialBarnumber(barnumber),
																   finalTimeSignature(TimeSignature()),
																   finalUpbeat(RationalTime(0,1)),
																   finalOctave(4),
																   finalDuration(0),
																   finalLastPitchClass(pitchclass),
																   finalBarnumber(-1),
																   translationMade(false) {
	translate();
}

SongLine::SongLine() : wcelines(vector<string>()),
					   initialUpbeat(RationalTime(0,1)),
					   initialTimeSignature(TimeSignature()),
					   initialDuration(0),
					   initialDotted(false),
					   initialOctave(4),
					   initialLastPitchClass('g'),
					   initialBarnumber(0),
					   translationMade(false) {
	translate();
}

SongLine::SongLine(const SongLine& sl) : wcelines(sl.getWceLines()),
									     initialUpbeat(sl.getInitialUpbeat()),
										 initialTimeSignature(sl.getInitialTimeSignature()),
										 initialDuration(sl.getInitialDuration()),
										 initialDotted(sl.getInitialDotted()),
										 initialOctave(sl.getInitialOctave()),
										 initialLastPitchClass(sl.getInitialLastPitchClass()),
										 initialBarnumber(sl.getInitialBarnumber()),
										 finalTimeSignature(sl.getFinalTimeSignature()),
										 finalUpbeat(sl.getFinalUpbeat()),
										 finalOctave(sl.getFinalOctave()),
										 finalDuration(sl.getFinalDuration()),
										 finalLastPitchClass(sl.getFinalLastPitchClass()),
										 finalDotted(sl.getFinalDotted()),
										 finalBarnumber(sl.getFinalBarnumber()),
										 translationMade(false) {
	translate();
}

void SongLine::translate() {
	if ( wcelines.size() == 0 ) { translationMade = true; return; } // empty songline: nothing to translate

	//break wcelines into tokens
	breakWcelines();

	//initialization of context
	int currentOctave = initialOctave;
	char lastPitchClass = initialLastPitchClass;
	int currentDuration = initialDuration;
	bool currentDotted = initialDotted;
	TimeSignature currentTimeSignature = initialTimeSignature;
	int currentBarnumber = initialBarnumber;
	RelLyToken::SlurStatus currentSlurStatus = RelLyToken::NO_SLUR_INFO;
	RelLyToken::TieStatus  currentTieStatus = RelLyToken::NO_TIE;
	bool currentTripletStatus = false;
	
	RationalTime cur = currentTimeSignature.getRationalTime();
	RationalTime timeInBar = currentTimeSignature.getRationalTime() - initialUpbeat;
	
	//if no upbeat, and first line then barnumber = 1
	if ( initialUpbeat == RationalTime(0,1) && currentBarnumber == 0 ) currentBarnumber = 1;
	
	//now scan the relative lilypond tokens and translate to AbsoluteLilypond and Kern
	//assume music in first line
	vector<RelLyToken>::iterator rl_it;
	RelLyToken::Identity id;

	//string debugstring = wcelines[0];
	//for( rl_it = (relLyTokens[0]).begin(); rl_it != (relLyTokens[0]).end(); rl_it++ ) {
	//	cout << (*rl_it).getToken() << '\t'; }
	//cout << endl;
	
	//for debugging
	string rellytoken;
	//assume line 0 is melody, other lines are text
	//first do the melody
	string token = "";
	absLyTokens.push_back(vector<string>());
	kernTokens.push_back(vector<string>());
	for( rl_it = (relLyTokens[0]).begin(); rl_it != (relLyTokens[0]).end(); rl_it++ ) {
		rellytoken = (*rl_it).getToken();
		//to note or not to note
		id = (*rl_it).getIdentity();
		switch(id) {
			case RelLyToken::NOTE: {
				//cout << (*rl_it).getToken() << ": note" << endl;
				//now convert to abs ly and convert to kern
				//triplet (status is set by TIMES_COMMAND below. reset here, if brace is before note, otherwhise after creating note)
				if ( currentTripletStatus && (*rl_it).containsClosingBraceBeforeNote() ) currentTripletStatus = false;
				//duration:
				if ( (*rl_it).getDurationBase() != 0 ) {
					currentDuration = (*rl_it).getDurationBase();
					//dotted
					currentDotted = (*rl_it).getDotted();
				}
				//octave
				currentOctave = computeOctave( currentOctave, (*rl_it).getPitchClass(), lastPitchClass, (*rl_it).getOctaveCorrection() );
				//tie
				if ( currentTieStatus == RelLyToken::START_TIE || currentTieStatus == RelLyToken::CONTINUE_TIE )
					if ( (*rl_it).getTie() == RelLyToken::START_TIE )
						currentTieStatus = RelLyToken::CONTINUE_TIE;
					else
						currentTieStatus = RelLyToken::END_TIE;
				if ( currentTieStatus == RelLyToken::END_TIE || currentTieStatus == RelLyToken::NO_TIE )
					if ( (*rl_it).getTie() == RelLyToken::START_TIE )
						currentTieStatus = RelLyToken::START_TIE;
					else
						currentTieStatus = RelLyToken::NO_TIE;				
				//slur
				if (currentSlurStatus == RelLyToken::END_SLUR) currentSlurStatus = RelLyToken::NO_SLUR_INFO;
				if ( (*rl_it).getSlur() != RelLyToken::NO_SLUR_INFO ) //there is slurinfo, just copy
					currentSlurStatus = (*rl_it).getSlur();
				else {
					//so, no slur info
					//determine whether in slur or not, if so, status is IN_SLUR, otherwise leave currentstatus untouched.
					if ( currentSlurStatus == RelLyToken::START_SLUR || currentSlurStatus == RelLyToken::IN_SLUR)
						currentSlurStatus = RelLyToken::IN_SLUR;
				}
				//create note-token
				token = (*rl_it).createKernNote(currentOctave,
				                                currentDuration,
												currentDotted,
												currentTripletStatus,
												currentSlurStatus,
												currentTieStatus);
				kernTokens[0].push_back(token);
				token = (*rl_it).createAbsLyNote(currentOctave,
				                                 currentDuration,
												 currentDotted,
												 currentSlurStatus,
												 currentTieStatus);
				absLyTokens[0].push_back(token);
				//set LastPitchclass
				if ((*rl_it).getPitchClass() != 'r' && (*rl_it).getPitchClass() != 's' ) lastPitchClass = (*rl_it).getPitchClass();
				//set time
				timeInBar = timeInBar + rationalDuration(currentDuration, currentDotted, currentTripletStatus);
				// set tripletstatus to false if closingbrace after note
				if ( currentTripletStatus && (*rl_it).containsClosingBraceAfterNote() ) currentTripletStatus = false;
			} break;
			
			case RelLyToken::TIME_COMMAND: { 
				//cout << (*rl_it).getToken() << ": time" << endl;
				currentTimeSignature = (*rl_it).getTimeSignature();
				//absLy: just copy the token
				absLyTokens[0].push_back((*rl_it).getToken());
				//kern: conver tot kern
				kernTokens[0].push_back(currentTimeSignature.getKernTimeSignature());
			} break;
			
			case RelLyToken::TIMES_COMMAND: { //for now assume '\times 2/3'
				//cout << (*rl_it).getToken() << ": times" << endl;
				currentTripletStatus = true;
				//first for absLy
				token = (*rl_it).getToken(); //just copy, then also opening brace is copied.
				absLyTokens[0].push_back((*rl_it).getToken());
				//kern: nothing to add, only adjust durations (is done in RelLyToken::createKernNote)
			} break;
			
			case RelLyToken::TEXT: {
				cerr << "Error: Tekst in melody line: \"" << (*rl_it).getToken() << "\"" << endl;
				exit(1);
			} break;
			
			case RelLyToken::UNKNOWN: {
				cerr << "Warning: Unknown token: \"" << (*rl_it).getToken() << "\"" << endl;
				//exit(1);
			} break;
			
			default: {
				cerr << "Error: Unrecognized token: \"" << (*rl_it).getToken() << "\" This should not happen." << endl;
				exit(1);
			} break;
		}
		// new bar? : raise barnumber and write barlines to kern
		if ( timeInBar == currentTimeSignature.getRationalTime() ) { // new bar
			//write in **kern
			currentBarnumber++;
			stringstream ss;
			ss << "=" << currentBarnumber;
			string barstr;
			ss >> barstr;
			kernTokens[0].push_back(barstr);
			timeInBar = RationalTime(0,1);
		}
	}

	finalOctave = currentOctave;
	finalDuration = currentDuration;
	finalDotted = currentDotted;
	finalBarnumber = currentBarnumber;
	finalTimeSignature = currentTimeSignature;
	if (lastPitchClass != 's' && lastPitchClass != 'r') finalLastPitchClass = lastPitchClass;
	finalUpbeat = currentTimeSignature.getRationalTime() - timeInBar;


	//now do text lines (if any). Take kern as reference.
	string str;
	vector<string>::iterator krn_it;
	int numLines = wcelines.size();
	for( int i = 1; i<numLines; i++ ) {
		absLyTokens.push_back(vector<string>());
		kernTokens.push_back(vector<string>());
	}
	int relly_index = 0;
	for ( krn_it = kernTokens[0].begin(); krn_it != kernTokens[0].end(); krn_it++) {
		//now do the krn
		//if not a corresponding token in ly: add token from melodyline.
		if ( (*krn_it).find_first_of("!=*") != string::npos ) {
			for ( int i = 1; i<numLines; i++ ) {
				kernTokens[i].push_back( (*krn_it) );
			} 
			
			//it can be possible that there is a corresponding rel ly token (time / times). If so, add that to absLy
			if ( relly_index < relLyTokens[0].size() ) {
				if ( relLyTokens[0][relly_index].getIdentity() != RelLyToken::NOTE ) {
					for (int i = 1; i<numLines; i++ ) {
						absLyTokens[i].push_back("");
					}
					relly_index++;
				}
			}
		} else { // text
			for ( int i = 1; i<numLines; i++ ) {
				if ( relly_index >= relLyTokens[i].size() )
					kernTokens[i].push_back( "." );
				else
					kernTokens[i].push_back( toKernText( relLyTokens[i][relly_index].getToken() ) );

				//for absLy: just copy:
				for( int i = 1; i<numLines; i++ )  {
					//check
					if ( relly_index >= relLyTokens[i].size() )
						absLyTokens[i].push_back( "" );
					else
						absLyTokens[i].push_back( relLyTokens[i][relly_index].getToken() );
				}
			}
			relly_index++;
		}
	}
	
	//debug
	/*
	vector<string>::iterator debugit;
	vector<RelLyToken>::iterator debugit2;
	cout << endl;
	for( debugit2 = (relLyTokens[0]).begin(); debugit2 != (relLyTokens[0]).end(); debugit2++ ) {
		cout << (*debugit2).getToken() << '\t'; }
	cout << endl;
	for( debugit = (absLyTokens[1]).begin(); debugit != (absLyTokens[1]).end(); debugit++ ) {
		cout << (*debugit) << '\t'; }
	cout << endl;
	for( debugit = (absLyTokens[0]).begin(); debugit != (absLyTokens[0]).end(); debugit++ ) {
		cout << (*debugit) << '\t'; }
	cout << endl;
	for( debugit = (absLyTokens[1]).begin(); debugit != (absLyTokens[1]).end(); debugit++ ) {
		cout << (*debugit) << '\t'; }
	cout << endl;
	for( debugit = (kernTokens[0]).begin(); debugit != (kernTokens[0]).end(); debugit++ ) {
		cout << (*debugit) << '\t'; }
	cout << endl;
	for( debugit = (kernTokens[1]).begin(); debugit != (kernTokens[1]).end(); debugit++ ) {
		cout << (*debugit) << '\t'; }
	cout << endl;
	*/
	translationMade = true;
}

SongLine& SongLine::operator=(const SongLine& sl) {
	if ( &sl != this ) {
	}
	cerr << "Waarschuwing: SongLine& SongLine::operator=(const SongLine& sl) gebruikt!!!!" << endl;
	return *this;
}

void SongLine::writeToStdout() const {
	cout << wcelines.size() << " lines" << endl;
	vector<string>::const_iterator it;
	for( it = wcelines.begin(); it != wcelines.end(); it++)
		cout << "line: " << *it << endl;
}


void SongLine::breakWcelines() {
	string line;
	string token;
	vector<RelLyToken> emptyline;

	string::size_type pos;
	vector<string>::const_iterator it;
	for( it = wcelines.begin(); it != wcelines.end(); it++) {
		relLyTokens.push_back(emptyline);
		line = *it;
		while ( line.size() > 0 ) {
			if ( ( pos=line.find_first_of("\t") ) == string::npos) { // not found => last token
				pvktrim(line);
				(relLyTokens.back()).push_back(RelLyToken(line));
				line.clear();
			} else { //found -> extract token and add to 'matrix'
				token = line.substr(0, pos);
				pvktrim(token);
				(relLyTokens.back()).push_back(RelLyToken(token));
				line.erase(0, pos+1); //also erase tab
				//if (line.size() == 0) line = "r";
			}
		}
	}
	//now check wether all lines have the same number of tokens
	vector<vector<RelLyToken> >::iterator rlit;
	for ( rlit = relLyTokens.begin(); rlit != relLyTokens.end(); rlit++ ) {
		if ((*rlit).size() != (*relLyTokens.begin()).size() ) {
			int size2 = (*rlit).size();
			int size1 = (*relLyTokens.begin()).size();
			cerr << "Warning: not the same number of tokens on each line: " << size1 << ", " << size2 << endl;
			writeToStdout();
		}
	}
}

int SongLine::computeOctave(int curoct, char pitch, char lastPitch, int octcorrection) const {
	int res = curoct;

	/*const char relativeRoot=((pitch>='c') ? pitch : (pitch+8)); // representation shifted for a and b
	const char lastRelativeRoot=((lastPitch>='c') ? lastPitch : (lastPitch+8));
	
	if (root!='r' && root!='s') {
		char relative=relativeRoot-lastRelativeRoot;
		if (relative>3) // an additional ' must be given in relative encoding, so we must remove it here
			res--;
		if (relative<-3) // analog
			res++;
	}
	*/
	
	const char relativeRoot=((pitch>='c') ? pitch : (pitch+7)); // representation shifted for a and b
	const char lastRelativeRoot=((lastPitch>='c') ? lastPitch : (lastPitch+7));
	
	if ( pitch != 'r' && pitch != 's' ) {
		int diff = relativeRoot - lastRelativeRoot;
		if (abs(diff) > 3) {
			if (diff > 0 ) res--; else res++;
		}
	}
	
	res += octcorrection;

	return res;
}

RationalTime SongLine::rationalDuration(int duration, bool dotted, bool triplet) const {
		int n = 1;
		int d = duration;
		if ( dotted ) { n = 3; d = 2*duration; }
		if ( triplet ) { d = (d/2)*3; }
		return RationalTime(n,d);
}

string SongLine::toKernText(string tok) const {
	return tok;
}
