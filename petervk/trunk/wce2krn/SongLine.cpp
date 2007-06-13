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
#include<sstream>
#include<iostream>
#include<cstdlib>
#include<cassert>
#include <locale>
using namespace std;

SongLine::SongLine(vector<string> lines, RationalTime upb, TimeSignature timesig, int duration, bool dotted, int octave, char pitchclass, int keysig, int mtempo, int barnumber) :
																   wcelines(lines),
																   initialUpbeat(upb),
																   initialTimeSignature(timesig),
																   initialDuration(duration),
																   initialDotted(dotted),
																   initialOctave(octave),
																   initialLastPitchClass(pitchclass),
																   initialBarnumber(barnumber),
																   finalTimeSignature(timesig),
																   finalUpbeat(RationalTime(0,1)),
																   finalOctave(4),
																   finalDuration(0),
																   finalDotted(false),
																   finalLastPitchClass(pitchclass),
																   finalBarnumber(-1),
																   keySignature(keysig),
																   midiTempo(mtempo),
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
					   keySignature(0),
					   midiTempo(120),
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
										 keySignature(sl.getKeySignature()),
										 midiTempo(sl.getMidiTempo()),
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
	int indexFirstKernNote = -1;
	int indexLastKernNote = -1;
		
	//now scan the relative lilypond tokens and translate to AbsoluteLilypond and Kern
	//assume music in first line
	vector<RelLyToken>::iterator rl_it;
	RelLyToken::Identity id;

	//string debugstring = wcelines[0];
	//for( rl_it = (relLyTokens[0]).begin(); rl_it != (relLyTokens[0]).end(); rl_it++ ) {
	//	cout << (*rl_it).getToken() << '\t'; }
	//cout << endl;
	
	//for debugging
	//string rellytoken;
	//assume line 0 is melody, other lines are text
	//first do the melody
	string token = "";
	absLyTokens.push_back(vector<string>());
	kernTokens.push_back(vector<string>());
	//Upbeat and initial barnumber (if no upbeat)

	RationalTime timeInBar = RationalTime(0,1);
	if ( initialUpbeat != 0 ) {
		timeInBar = currentTimeSignature.getRationalTime() - initialUpbeat;
		//cout << "INITIAL " << timeInBar.getNumerator() << "/" << timeInBar.getDenominator() << endl;
		//cout << "INITIALUPBEAT " << initialUpbeat.getNumerator() << "/" << initialUpbeat.getDenominator() << endl;
	} else {
		stringstream ss;
		ss << "=" << currentBarnumber;
		string barstr = "";
		ss >> barstr;
		kernTokens[0].push_back(barstr);
		timeInBar = RationalTime(0,1);
	}
	for( rl_it = (relLyTokens[0]).begin(); rl_it != (relLyTokens[0]).end(); rl_it++ ) {
		//rellytoken = (*rl_it).getToken();

		//to note or not to note		
		id = (*rl_it).getIdentity();
		switch(id) {
			case RelLyToken::NOTE: {
		
				// new bar? : raise barnumber and write barlines to kern
				if ( timeInBar > currentTimeSignature.getRationalTime() ) {
					cerr << "Error: bar to long: " << currentBarnumber << endl;
				}
				if ( timeInBar == currentTimeSignature.getRationalTime() ) { // new bar
					//write in **kern
					stringstream ss;
					currentBarnumber++;
					ss << "=" << currentBarnumber;
					string barstr = "";
					ss >> barstr;
					kernTokens[0].push_back(barstr);
					timeInBar = RationalTime(0,1);
				}
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
				//RelLyToken::TieStatus rt = (*rl_it).getTie();
				if ( currentTieStatus == RelLyToken::START_TIE || currentTieStatus == RelLyToken::CONTINUE_TIE )
					if ( (*rl_it).getTie() == RelLyToken::START_TIE)
						currentTieStatus = RelLyToken::CONTINUE_TIE;
					else
						currentTieStatus = RelLyToken::END_TIE;
				else if ( currentTieStatus == RelLyToken::END_TIE || currentTieStatus == RelLyToken::NO_TIE )
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
				// if this was the first note: set indexFirstKernNote
				if (indexFirstKernNote == -1) indexFirstKernNote = kernTokens[0].size()-1;
				// set indexLastKernNote always. After finishing the line, this is correct
				indexLastKernNote = kernTokens[0].size()-1;
				//set LastPitchclass
				if ((*rl_it).getPitchClass() != 'r' && (*rl_it).getPitchClass() != 's' ) lastPitchClass = (*rl_it).getPitchClass();
				//set time
				timeInBar = timeInBar + rationalDuration(currentDuration, currentDotted, currentTripletStatus);
				// set tripletstatus to false if closingbrace after note
				RationalTime t = rationalDuration(currentDuration, currentDotted, currentTripletStatus);
				//cout << token << " bar:" << currentBarnumber << " " << timeInBar.getNumerator() << "/" << timeInBar.getDenominator() << 
				//	" " << t.getNumerator() << "/" << t.getDenominator() << endl;
				if ( currentTripletStatus && (*rl_it).containsClosingBraceAfterNote() ) currentTripletStatus = false;
			} break;
			
			case RelLyToken::TIME_COMMAND: { 
				//cout << (*rl_it).getToken() << ": time" << endl;
				//Change initialTimeSignature if \time is at begin of line
				bool firstOfLine = false;
				if ( timeInBar == RationalTime(0,1) && currentBarnumber == initialBarnumber ) {
					const TimeSignature &tsc = initialTimeSignature;
					TimeSignature &ts = const_cast<TimeSignature &>(tsc);
					ts = (*rl_it).getTimeSignature();
					firstOfLine = true;
				}
				//do this barline here
				if ( timeInBar != currentTimeSignature.getRationalTime() && timeInBar != RationalTime(0,1) ) {
					cerr << "Error: meter change out of place in bar: " << currentBarnumber << endl;
				}
				if ( timeInBar == currentTimeSignature.getRationalTime() ) { // new bar
					//write in **kern
					stringstream ss;
					currentBarnumber++;
					ss << "=" << currentBarnumber;
					string barstr = "";
					ss >> barstr;
					kernTokens[0].push_back(barstr);
					timeInBar = RationalTime(0,1);
				}				
				currentTimeSignature = (*rl_it).getTimeSignature();
				//absLy: just copy the token
				absLyTokens[0].push_back((*rl_it).getToken());
				//kern: conver tot kern
				//firstOfLine gaat mis als hele song wordt geconverteerd. Dan is er geen preamble voor elke regel.
				//if (!firstOfLine) kernTokens[0].push_back(currentTimeSignature.getKernTimeSignature());
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
				if ( (*rl_it).getToken().size() > 0 )
					cerr << "Warning: Unknown token: \"" << (*rl_it).getToken() << "\"" << endl;
				//exit(1);
			} break;
			
			default: {
				cerr << "Error: Unrecognized token: \"" << (*rl_it).getToken() << "\" This should not happen." << endl;
				exit(1);
			} break;
		}
	}
	
	// add the { and } phrase markers to the kern melody
	if ( indexFirstKernNote != -1 && indexLastKernNote != -1 ) {
		kernTokens[0][indexFirstKernNote] = "{" + kernTokens[0][indexFirstKernNote];
		kernTokens[0][indexLastKernNote] = kernTokens[0][indexLastKernNote] + "}"; 
	}
	
	finalOctave = currentOctave;
	finalDuration = currentDuration;
	finalDotted = currentDotted;
	finalTimeSignature = currentTimeSignature;
	if (lastPitchClass != 's' && lastPitchClass != 'r') finalLastPitchClass = lastPitchClass;
	if (timeInBar == currentTimeSignature.getRationalTime()) {
		finalUpbeat = RationalTime(0,1);
		finalBarnumber = currentBarnumber+1;
	} else {
		finalUpbeat = currentTimeSignature.getRationalTime() - timeInBar;
		finalBarnumber = currentBarnumber;
	}

	//now do text lines (if any). Take kern as reference.
	string str;
	vector<string>::iterator krn_it;
	int numLines = wcelines.size();
	for( int i = 1; i<numLines; i++ ) {
		absLyTokens.push_back(vector<string>());
		kernTokens.push_back(vector<string>());
		lyricsLines.push_back("");
	}
	int relly_index = 0;
	RelLyToken::TextStatus currentTextStatus = RelLyToken::SINGLE_WORD;
	
	for ( krn_it = kernTokens[0].begin(); krn_it != kernTokens[0].end(); krn_it++) {
		//now do the krn

		//if not a corresponding token in ly: add token from melodyline.
		if ( (*krn_it).find_first_of("!=*") != string::npos ) {
			if ( (*krn_it).find("*") == 0 )
				for ( int i = 1; i<numLines; i++ ) {
					kernTokens[i].push_back( "*" );
				}
			else
				for ( int i = 1; i<numLines; i++ ) {
					kernTokens[i].push_back( (*krn_it) );
				} 
			
			//it can be possible that there is a corresponding rel ly token (time). If so, add that to absLy
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
				else {
					if ( relLyTokens[0][relly_index].getIdentity() != RelLyToken::TIMES_COMMAND ) {
						//textstatus:

						bool dash = ( relLyTokens[i][relly_index].getToken().find("--") != string::npos );
						bool underscore = ( relLyTokens[i][relly_index].getToken().find("_") != string::npos );
						
						//if "_" and IN_WORD/BEGIN_WORD then "||" should be printed, if "_" and END_WORD/SINGLE WORD then '|' should be prined
						//so, status should not be changed.
						if ( !underscore ) {
							if ( currentTextStatus == RelLyToken::SINGLE_WORD ) {
								if (dash) currentTextStatus = RelLyToken::BEGIN_WORD;
								else currentTextStatus = RelLyToken::SINGLE_WORD;
							} else if ( currentTextStatus == RelLyToken::IN_WORD ) {
								if (dash) currentTextStatus = RelLyToken::IN_WORD;
								else currentTextStatus = RelLyToken::END_WORD;
							} else if ( currentTextStatus == RelLyToken::BEGIN_WORD ) {
								if (dash) currentTextStatus = RelLyToken::IN_WORD;
								else currentTextStatus = RelLyToken::END_WORD;
							} else if ( currentTextStatus == RelLyToken::END_WORD ) {
								if (dash) currentTextStatus = RelLyToken::BEGIN_WORD;
								else currentTextStatus = RelLyToken::SINGLE_WORD;
							}
						}
												
						// add translated text to appropriate vectors
						kernTokens[i].push_back( toText( relLyTokens[i][relly_index].getToken(), currentTextStatus, KERN ) );
						lyricsLines[i-1] = lyricsLines[i-1] + toText( relLyTokens[i][relly_index].getToken(), currentTextStatus, TEXT );
					} else
						krn_it--; // Same Token again. DO NOT USE krn_it AFTER THIS.
				}

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
	cerr.flush();
	assert(false);
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
	/*
	vector<vector<RelLyToken> >::iterator rlit;
	for ( rlit = relLyTokens.begin(); rlit != relLyTokens.end(); rlit++ ) {
		if ((*rlit).size() != (*relLyTokens.begin()).size() ) {
			int size2 = (*rlit).size();
			int size1 = (*relLyTokens.begin()).size();
			cerr << "Warning: not the same number of tokens on each line: " << size1 << ", " << size2 << endl;
			writeToStdout();
		}
	}
	*/
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

string SongLine::toText(string tok, RelLyToken::TextStatus ts, Representation repr) const {
	pvktrim(tok);
	
	if ( tok.size() == 0 )
		if ( repr == KERN ) return "."; else return "";
	if ( tok == "_" )
		if ( repr == KERN ) {
			if ( ts == RelLyToken::END_WORD || ts == RelLyToken::SINGLE_WORD ) return "|";
			else if ( ts == RelLyToken::IN_WORD || ts == RelLyToken::BEGIN_WORD ) return "||";
			else return ".";
		}
		else return "";
		
	if ( tok.find(" --") != string::npos) tok.erase(tok.find(" --"));
	
	//if only a dash, print a warning. Should be an underscore.
	if ( tok.find_first_not_of("-") == string::npos ) {
		cerr << "Warning: only a \"-\" under a note. Should be an \"_\"?" << endl;
	}
	
	switch (ts) {
		case RelLyToken::IN_WORD: {
			if ( repr == KERN ) tok = "-" + tok + "-"; //else ok
			break;
		}
		
		case RelLyToken::BEGIN_WORD: {
			if ( repr == KERN ) tok = tok + "-"; 
			break;
		}
		
		case RelLyToken::END_WORD: {
			if ( repr == KERN ) tok = "-" + tok; else tok = tok + " ";
			break;
		}
		
		case RelLyToken::SINGLE_WORD: {
			if ( repr == TEXT ) tok = tok + " ";
			break;
		}
	}
	
	//remove "
	string::size_type pos;
	while ( (pos = tok.find_first_of("\"")) != string::npos ) tok.erase(pos,1);
	
	return tok;
}

vector<string> SongLine::getLyLine(bool absolute) const{
	vector<string> res;
	
	if (!absolute) {
		res = wcelines;
		// add break at end
		if ( res.size() >0 ) res[0] = res[0] + " \\mBreak";
		// make sure first note has duration
		inheritFirstLynoteDuration( res[0], initialDuration);
	}
	
	return res;
}

vector<string> SongLine::getLyBeginSignature(bool absolute) const {
	vector<string> res;
	
	res.push_back("mBreak = { \\bar \"\" \\break }");
	res.push_back("x = {\\once\\override NoteHead #'style = #'cross }");
	res.push_back("\\let gl=\\glissando");
	res.push_back("\\version\"2.8.2\"");
	res.push_back("\\score {{");
	
	string key;
	//guess most probable key (not optimal)
	switch( keySignature ) {
		case -4: key = "f \\minor"; break;
		case -3: key = "es \\major"; break;
		case -2: key = "g \\minor"; break;
		case -1: key = "d \\minor"; break;
		case  0: key = "c \\major"; break;
		case  1: key = "g \\major"; break;
		case  2: key = "d \\major"; break;
		case  3: key = "a \\major"; break;
		case  4: key = "e \\major"; break;
	}
	res.push_back("\\key " + key);
	
	if ( !absolute ) {
		string relative;
		relative = initialLastPitchClass;
		int octave = initialOctave;
		octave = octave - 3;
		if (octave !=0 ) {
			for (int i=1; i<=abs(octave); i++) {
				if (octave<0) relative = relative + ","; else relative = relative + "'";
			}
		}
		res.push_back("\\relative " + relative);
	}
	
	res.push_back("{");
	res.push_back("\\set melismaBusyProperties = #\'()");
	
	string partial = upbeatToString(initialUpbeat);
	if ( partial != "" ) res.push_back("\\partial " + partial);
	
	string meter;
	stringstream meterss;
	meterss << initialTimeSignature.getNumerator() << "/" << initialTimeSignature.getDenominator();
	meterss >> meter;
	res.push_back("\\time " + meter);
	
	return res;
}

vector<string> SongLine::getLyEndSignature() const {
	vector<string> res;
	
	res.push_back(" }}");
	res.push_back(" \\midi { \\tempo 4=120 }");
	res.push_back(" \\layout {}");
	res.push_back("}");
	
	return res;
}

vector<string> SongLine::getKernLine() const {
	vector<string> res;
	string s;
	
	if ( kernTokens.size() == 0 ) return res;
	
	for(int j=0; j < kernTokens[0].size(); j++) {
		s = "";
		for (int i=0; i < kernTokens.size(); i++ ) {
			s = s + kernTokens[i][j] + "\t";
		}
		//remove last tab.
		s = s.substr(0,s.size()-1);
		res.push_back(s);
	}
	
	return res;
}

vector<string> SongLine::getKernBeginSignature() const {
	vector<string> res;
	string s;
	if ( kernTokens.size() == 0 ) return res;

	//identify spine
	s = "";
	for(int i=0; i < kernTokens.size(); i++ ) {
		if (i == 0 ) s = s + "**kern" + "\t" ; else s = s + "**text" + "\t";
	}
	//remove last tab.
	s = s.substr(0,s.size()-1);
	res.push_back(s);
	
	//time signature
	s = "";
	for(int i=0; i < kernTokens.size(); i++ ) {
		if ( i == 0 ) s = s + initialTimeSignature.getKernTimeSignature() + "\t"; else s = s + "*\t";
	}
	s = s.substr(0,s.size()-1);
	res.push_back(s);

	string keys = "";
	if (keySignature < 0) keys = "b-e-a-d-g-c-f-";
	if (keySignature > 0) keys = "f#c#g#d#a#e#b#";

	//midi tempo
	stringstream ss;
	ss << midiTempo;
	string midiTempo_str;
	ss >> midiTempo_str;
	s = "";
	for(int i=0; i < kernTokens.size(); i++ ) {
		if ( i == 0 ) s = s + "*MM" + midiTempo_str + "\t"; else s = s + "*\t";
	}
	s = s.substr(0,s.size()-1);
	res.push_back(s);
	
	//key signature
	int ks = abs(keySignature);
	s = "";
	for( int i=0; i< kernTokens.size(); i++ ) {
		if ( i == 0) s = s + "*k[" + keys.substr(0,2*ks) + "]\t"; else s = s + "*\t";
	}
	s = s.substr(0,s.size()-1);
	res.push_back(s);
	
	return res;
}

vector<string> SongLine::getKernEndSignature() const {
	vector<string> res;

	string s;
	if ( kernTokens.size() == 0 ) return res;

	//identify spine
	s = "";
	for(int i=0; i < kernTokens.size(); i++ ) {
		s = s + "*-" + "\t";
	}
	s = s.substr(0,s.size()-1);
	res.push_back(s);
	
	return res;
}

string SongLine::upbeatToString(RationalTime t) const {
	string res;
	
	if (initialUpbeat == 0 ) return "";
	
	stringstream ss;
	
	RationalTime rnum = initialUpbeat / RationalTime(1,32);
	int num = rnum.getNumerator()/rnum.getDenominator();
	ss << num;
	
	ss >> res;
	res = "32*" + res;
	
	return res;

}

bool SongLine::inheritFirstLynoteDuration( string& lyline, int duration) const {
	locale loc("");
	string::size_type pos = 0;
	stringstream ss;
	string strduration;
	
	
	int index = 0;
	if (relLyTokens.size() > 0) {

		while ( index < relLyTokens[0].size() )
			if ( relLyTokens[0][index].getIdentity() != RelLyToken::NOTE )
				index++;
			else
				break;	
	}
	else
		return false; //no tokens
		
	//index passed end?
	if ( index >= relLyTokens[0].size() ) return false;
	
	//firstnote has duration?	
	if ( relLyTokens[0][index].getDurationBase() != 0 ) return true;
	
	//find index of end of first note	
	while ( index > 0 ) {
		pos = lyline.find('\t');
		index--;
	}
	
	if ( (pos = lyline.find_first_of("abcdefgsr",pos)) == string::npos) return false;
	pos++;
	while ( pos < lyline.size() )
		if ( lyline.at(pos) == ',' || lyline.at(pos) == '\'' ) 
			pos++;
		else
			break;
	
	//here we should insert the duration

	ss << initialDuration;
	if ( initialDotted ) ss << '.';
	ss >> strduration;
	
	lyline.insert(pos, strduration);
	
	//cout << lyline << endl;
	
	return true;
}

string SongLine::getLyricsLine( int line ) const {
	return lyricsLines[line];
}