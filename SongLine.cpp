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
#include "wce2krn.h"
#include<string>
#include<iostream>
#include<iomanip>
#include<sstream>
#include<cstdlib>
#include<cassert>
#include <locale>
using namespace std;

SongLine::SongLine(vector<string> lines, RationalTime upb, TimeSignature timesig, int duration, int dots, int octave, char pitchclass, int keysig, int mtempo, int barnumber, bool meterinv, string filename, int phraseno, string recordno, string stropheno) :
																   wcelines(lines),
																   initialUpbeat(upb),
																   initialTimeSignature(timesig),
																   initialDuration(duration),
																   initialDots(dots),
																   initialOctave(octave),
																   initialLastPitchClass(pitchclass),
																   initialBarnumber(barnumber),
																   finalTimeSignature(timesig),
																   finalUpbeat(RationalTime(0,1)),
																   finalOctave(4),
																   finalDuration(0),
																   finalDots(0),
																   finalLastPitchClass(pitchclass),
																   finalBarnumber(-1),
																   keySignature(keysig),
																   midiTempo(mtempo),
																   translationMade(false),
																   meterInvisible(meterinv),
																   fileName(filename),
																   phraseNo(phraseno),
																   record(recordno),
																   strophe(stropheno) {
	translate();
}

SongLine::SongLine() : wcelines(vector<string>()),
					   initialUpbeat(RationalTime(0,1)),
					   initialTimeSignature(TimeSignature()),
					   initialDuration(0),
					   initialDots(0),
					   initialOctave(4),
					   initialLastPitchClass('g'),
					   initialBarnumber(0),
					   keySignature(0),
					   midiTempo(120),
					   translationMade(false),
					   meterInvisible(false),
					   fileName("[noname]"),
					   phraseNo(0),
					   record("unknown"),
					   strophe("0") {
	translate();
}

SongLine::SongLine(const SongLine& sl) : wcelines(sl.getWceLines()),
									     initialUpbeat(sl.getInitialUpbeat()),
										 initialTimeSignature(sl.getInitialTimeSignature()),
										 initialDuration(sl.getInitialDuration()),
										 initialDots(sl.getInitialDots()),
										 initialOctave(sl.getInitialOctave()),
										 initialLastPitchClass(sl.getInitialLastPitchClass()),
										 initialBarnumber(sl.getInitialBarnumber()),
										 finalTimeSignature(sl.getFinalTimeSignature()),
										 finalUpbeat(sl.getFinalUpbeat()),
										 finalOctave(sl.getFinalOctave()),
										 finalDuration(sl.getFinalDuration()),
										 finalLastPitchClass(sl.getFinalLastPitchClass()),
										 finalDots(sl.getFinalDots()),
										 finalBarnumber(sl.getFinalBarnumber()),
										 keySignature(sl.getKeySignature()),
										 midiTempo(sl.getMidiTempo()),
										 translationMade(sl.translationMade),
										 relLyTokens(sl.relLyTokens),
										 absLyTokens(sl.absLyTokens),
										 kernTokens(sl.kernTokens),
										 absLyLine(sl.absLyLine),
										 lyricsLines(sl.lyricsLines),
										 meterInvisible(sl.meterInvisible),
										 text_ann(sl.text_ann),
										 ties_ann(sl.ties_ann),
										 slurs_ann(sl.slurs_ann),
										 fileName(sl.fileName),
										 phraseNo(sl.phraseNo),
										 record(sl.record),
										 strophe(sl.strophe),
										 annotations(sl.annotations) {
	//translate();
}

/* old copy constructor
SongLine::SongLine(const SongLine& sl) : wcelines(sl.getWceLines()),
									     initialUpbeat(sl.getInitialUpbeat()),
										 initialTimeSignature(sl.getInitialTimeSignature()),
										 initialDuration(sl.getInitialDuration()),
										 initialDots(sl.getInitialDots()),
										 initialOctave(sl.getInitialOctave()),
										 initialLastPitchClass(sl.getInitialLastPitchClass()),
										 initialBarnumber(sl.getInitialBarnumber()),
										 finalTimeSignature(sl.getFinalTimeSignature()),
										 finalUpbeat(sl.getFinalUpbeat()),
										 finalOctave(sl.getFinalOctave()),
										 finalDuration(sl.getFinalDuration()),
										 finalLastPitchClass(sl.getFinalLastPitchClass()),
										 finalDots(sl.getFinalDots()),
										 finalBarnumber(sl.getFinalBarnumber()),
										 keySignature(sl.getKeySignature()),
										 midiTempo(sl.getMidiTempo()),
										 translationMade(false) {
	translate();
}
*/

void SongLine::translate() {
	
	//if ( translationMade ) exit(3);
	
	if ( wcelines.size() == 0 ) { translationMade = true; return; } // empty songline: nothing to translate

	//clog << getLocation() << endl;

	//break wcelines into tokens
	breakWcelines();

	//initialization of context
	int currentOctave = initialOctave;
	char lastPitchClass = initialLastPitchClass;
	int currentDuration = initialDuration;
	int currentDots = initialDots;
	TimeSignature currentTimeSignature = initialTimeSignature;
	int currentBarnumber = initialBarnumber;
	RelLyToken::SlurStatus currentSlurStatus = RelLyToken::NO_SLUR;
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
	if ( !meterInvisible ) { // if meter invisible, no barlines at all
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
	}
	for( rl_it = (relLyTokens[0]).begin(); rl_it != (relLyTokens[0]).end(); rl_it++ ) {
		//rellytoken = (*rl_it).getToken();

		//to note or not to note
		//in each case the slur and tie annotation has to be updated, for keeping the ananotations in sync with the melody.		
		id = (*rl_it).getIdentity();
		//cout << (*rl_it).getToken() << " : " << id << endl;
		switch(id) {
			case RelLyToken::NOTE: {
		
				// new bar? : raise barnumber and write barlines to kern
				if ( !meterInvisible ) {
					if ( timeInBar > currentTimeSignature.getRationalTime() ) {
						cerr << getLocation() << ": Error: bar to long: " << currentBarnumber << endl;
					}
					if ( timeInBar >= currentTimeSignature.getRationalTime() ) { // new bar
						//write in **kern
						stringstream ss;
						currentBarnumber++;
						ss << "=" << currentBarnumber;
						string barstr = "";
						ss >> barstr;
						kernTokens[0].push_back(barstr);
						timeInBar = RationalTime(0,1);
					}
				}
				//cout << (*rl_it).getToken() << ": note" << endl;
				//now convert to abs ly and convert to kern
				//triplet (status is set by TIMES_COMMAND below. reset here, if brace is before note, otherwhise after creating note)
				if ( currentTripletStatus && (*rl_it).containsClosingBraceBeforeNote() ) currentTripletStatus = false;
				//duration:
				if ( (*rl_it).getDurationBase() != 0 ) {
					currentDuration = (*rl_it).getDurationBase();
					//dots
					currentDots = (*rl_it).getDots();
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
				if (currentSlurStatus == RelLyToken::END_SLUR) currentSlurStatus = RelLyToken::NO_SLUR;
				if ( (*rl_it).getSlur() != RelLyToken::NO_SLUR ) //there is slurinfo, just copy
					currentSlurStatus = (*rl_it).getSlur();
				else {
					//so, no slur info
					//determine whether in slur or not, if so, status is IN_SLUR, otherwise leave currentstatus untouched.
					if ( currentSlurStatus == RelLyToken::START_SLUR || currentSlurStatus == RelLyToken::IN_SLUR)
						currentSlurStatus = RelLyToken::IN_SLUR;
				}
				// make annotation
				slurs_ann.push_back(currentSlurStatus);
				ties_ann.push_back(currentTieStatus);
				
				// Glissando end? Then previous note should have glissando start.
				// Glissandi can not cross line breaks (yet?)
				if ( (*rl_it).getGlissandoEnd() ) {
					if ( kernTokens[0].size() > 1 ) { // there must be a previous kern note.
						kernTokens[0][kernTokens[0].size()-1] = kernTokens[0][kernTokens[0].size()-1] + "h";
					}
				} 
				//create note-token
				token = (*rl_it).createKernNote(currentOctave,
				                                currentDuration,
												currentDots,
												currentTripletStatus,
												currentSlurStatus,
												currentTieStatus);
				kernTokens[0].push_back(token);
				token = (*rl_it).createAbsLyNote(currentOctave,
				                                 currentDuration,
												 currentDots,
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
				timeInBar = timeInBar + rationalDuration(currentDuration, currentDots, currentTripletStatus);
				// set tripletstatus to false if closingbrace after note
				RationalTime t = rationalDuration(currentDuration, currentDots, currentTripletStatus);
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
					cerr << getLocation() << ": Error: meter change out of place in bar: " << currentBarnumber << endl;
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
				//annotation has also to be done here
				ties_ann.push_back(RelLyToken::NO_TIE_INFO);
				slurs_ann.push_back(RelLyToken::NO_SLUR_INFO);
			} break;
			
			case RelLyToken::TIMES_COMMAND: { //for now assume '\times 2/3' (or '\times2/3')
				//cout << (*rl_it).getToken() << ": times" << endl;
				currentTripletStatus = true;
				//first for absLy
				token = (*rl_it).getToken(); //just copy, then also opening brace is copied.
				absLyTokens[0].push_back((*rl_it).getToken());
				//kern: nothing to add, only adjust durations (is done in RelLyToken::createKernNote)

				//annotation has also to be done here
				ties_ann.push_back(RelLyToken::NO_TIE_INFO);
				slurs_ann.push_back(RelLyToken::NO_SLUR_INFO);
			} break;
			
			case RelLyToken::TEXT: {
				cerr << getLocation() << ": Error: Tekst in melody line: \"" << (*rl_it).getToken() << "\"" << endl;
				exit(1);
			} break;
			
			case RelLyToken::UNKNOWN: {
				if ( (*rl_it).getToken().size() > 0 )
					cerr << getLocation() << ": Warning: Unknown token: \"" << (*rl_it).getToken() << "\"" << endl;
					ties_ann.push_back(RelLyToken::NO_TIE_INFO);
					slurs_ann.push_back(RelLyToken::NO_SLUR_INFO);
				//exit(1);
			} break;
			
			default: {
				cerr << getLocation() << ": Error: Unrecognized token: \"" << (*rl_it).getToken() << "\" This should not happen." << endl;
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
	finalDots = currentDots;
	finalTimeSignature = currentTimeSignature;
	if (lastPitchClass != 's' && lastPitchClass != 'r') finalLastPitchClass = lastPitchClass;
	if (timeInBar == currentTimeSignature.getRationalTime()) {
		finalUpbeat = RationalTime(0,1);
		finalBarnumber = currentBarnumber+1;
	} else {
		finalUpbeat = currentTimeSignature.getRationalTime() - timeInBar;
		finalBarnumber = currentBarnumber;
	}

	//now do text lines (if any). Take kern as reference. (10-10-2007: was wrong decision. ly shoud be taken as ref because it is the input...
	string str;
	vector<string>::iterator krn_it;
	int numLines = wcelines.size();
	vector<RelLyToken::TextStatus> vrt;
	for( int i = 1; i<numLines; i++ ) {
		absLyTokens.push_back(vector<string>());
		kernTokens.push_back(vector<string>());
		lyricsLines.push_back("");
		text_ann.push_back(vrt);
	}
	int relly_index = 0;
	RelLyToken::TextStatus currentTextStatus = RelLyToken::SINGLE_WORD;
	//RelLyToken::Identity correspondingIdentity = RelLyToken::UNKNOWN;
	
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
						
			//it can be possible that there is a corresponding rel ly token (time / unknown). If so, add that to absLy
			if ( relly_index < relLyTokens[0].size() ) {
				if ( relLyTokens[0][relly_index].getIdentity() != RelLyToken::NOTE ) {
					for (int i = 1; i<numLines; i++ ) {
						absLyTokens[i].push_back("");
						if ( relLyTokens[0][relly_index].getIdentity() == RelLyToken::UNKNOWN )
							text_ann[i-1].push_back( RelLyToken::DONTKNOW );
						else
							text_ann[i-1].push_back( RelLyToken::NO_WORD );
					}
					relly_index++;
				}
			}
		} else { // text or TIMES or UNKNOWN with corresponding kern token
			for ( int i = 1; i<numLines; i++ ) {
				
				if ( relly_index >= relLyTokens[i].size() ) {
					kernTokens[i].push_back( "." );
					// rest at end of line could be tolerated
					if ( relLyTokens[0][relly_index].getPitchClass() == 'r' ) relLyTokens[i].push_back( RelLyToken("", getLocation(), false) );
					// any case: annotate
					text_ann[i-1].push_back( RelLyToken::NO_WORD );
				} else {
					if ( relLyTokens[0][relly_index].getIdentity() == RelLyToken::NOTE ) {
						//textstatus:

						bool dash = ( relLyTokens[i][relly_index].getToken().find("--") != string::npos );
						bool underscore = ( relLyTokens[i][relly_index].getToken().find("_") != string::npos );
						
						//if "_" and IN_WORD/BEGIN_WORD then "||" should be printed, if "_" and END_WORD/SINGLE WORD then '|' should be prined
						//"_" means continuation
						if ( !underscore ) {
							if ( currentTextStatus == RelLyToken::SINGLE_WORD || currentTextStatus == RelLyToken::SINGLE_WORD_CONT ) {
								if (dash) currentTextStatus = RelLyToken::BEGIN_WORD;
								else currentTextStatus = RelLyToken::SINGLE_WORD;
							} else if ( currentTextStatus == RelLyToken::IN_WORD || currentTextStatus == RelLyToken::IN_WORD_CONT ) {
								if (dash) currentTextStatus = RelLyToken::IN_WORD;
								else currentTextStatus = RelLyToken::END_WORD;
							} else if ( currentTextStatus == RelLyToken::BEGIN_WORD || currentTextStatus == RelLyToken::BEGIN_WORD_CONT ) {
								if (dash) currentTextStatus = RelLyToken::IN_WORD;
								else currentTextStatus = RelLyToken::END_WORD;
							} else if ( currentTextStatus == RelLyToken::END_WORD || currentTextStatus == RelLyToken::END_WORD_CONT ) {
								if (dash) currentTextStatus = RelLyToken::BEGIN_WORD;
								else currentTextStatus = RelLyToken::SINGLE_WORD;
							}
						} else { //underscore
							if ( currentTextStatus == RelLyToken::SINGLE_WORD )
								currentTextStatus = RelLyToken::SINGLE_WORD_CONT;
							if ( currentTextStatus == RelLyToken::END_WORD )
								currentTextStatus = RelLyToken::END_WORD_CONT;
							if ( currentTextStatus == RelLyToken::IN_WORD )
								currentTextStatus = RelLyToken::IN_WORD_CONT;
							if ( currentTextStatus == RelLyToken::BEGIN_WORD )
								currentTextStatus = RelLyToken::BEGIN_WORD_CONT;
						}
												
						// add translated text to appropriate vectors
						kernTokens[i].push_back( toText( relLyTokens[i][relly_index].getToken(), currentTextStatus, KERN ) );
						lyricsLines[i-1] = lyricsLines[i-1] + toText( relLyTokens[i][relly_index].getToken(), currentTextStatus, TEXT );
						// annotation
						//if rest or empty: NO_WORD
						if ( relLyTokens[0][relly_index].getPitchClass() == 'r' )
							text_ann[i-1].push_back(RelLyToken::NO_WORD);
						else if ( relLyTokens[i][relly_index].getToken().size() == 0 )
							text_ann[i-1].push_back(RelLyToken::NO_WORD);
						else
							text_ann[i-1].push_back(currentTextStatus);
						
					} else { //TIMES or UNKNOWN
						//if UNKNOWN: DONTKNOW
						if ( relLyTokens[0][relly_index].getIdentity() == RelLyToken::UNKNOWN )
							text_ann[i-1].push_back(RelLyToken::DONTKNOW);
						else
							text_ann[i-1].push_back( RelLyToken::NO_WORD );
						krn_it--; // Same Token again. DO NOT USE krn_it AFTER THIS.
					}
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
	//tokens left in 
	
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

	//some checks
	
	// 
	checkLengths();
	
	//for debuging
	createAnnotations();
	//printAnnotations(); // now done with command line switch
	
	//check melismas
	checkMelisma();
	//check ties
	checkTies();
	//check whether syllable under \time or \times
	checkTextPlacing();
	
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
	bool is_music = true;

	string::size_type pos;
	vector<string>::const_iterator it;
	for( it = wcelines.begin(); it != wcelines.end(); it++) {
		relLyTokens.push_back(emptyline);
		line = *it;
		while ( line.size() > 0 ) {
			if ( ( pos=line.find_first_of("\t") ) == string::npos) { // not found => last token
				pvktrim(line);
				(relLyTokens.back()).push_back(RelLyToken(line, getLocation(), is_music));
				line.clear();
			} else { //found -> extract token and add to 'matrix'
				token = line.substr(0, pos);
				pvktrim(token);
				(relLyTokens.back()).push_back(RelLyToken(token, getLocation(), is_music));
				line.erase(0, pos+1); //also erase tab
				if (line.size() == 0) (relLyTokens.back()).push_back(RelLyToken("", getLocation(), is_music)); // if last token is empty, nothing remains, but token has to be added.
			}
		}
		is_music = false;
	}
	
	//now remove empty tokens that are at the end of the music line
	//and remove the last tokens from text lines that are longer than music line.
	
	int ti = relLyTokens[0].size()-1;
	int empty = 0;
	while ( relLyTokens[0][ti].getToken() == "" ) {empty++; ti--;}
	//cout << "empty: " << empty << endl;
	//cout << "size before: " << relLyTokens[0].size() << endl;
	//now empty contains the number of empty tokens at the end of the music line.
	relLyTokens[0].erase(relLyTokens[0].end()-empty, relLyTokens[0].end());
	for ( int i=0; i<wcelines.size(); i++ ) {
		int diff = relLyTokens[i].size() - relLyTokens[0].size();
		if ( diff > 0 ) relLyTokens[i].erase(relLyTokens[i].end()-diff, relLyTokens[i].end());
	}
	//cout << "size after: " << relLyTokens[0].size() << endl;
	
	//still do a check whether there are empty token in the middle of a melodyline
	for ( int i=0; i<relLyTokens[0].size(); i++ )
		if ( relLyTokens[0][i].getToken() == "" ) {
			cerr << getLocation() << ": Warning: empty token" << endl;
		}
	
	
	/*
	for ( int i = 0; i < wcelines.size(); i++ ) {
		cout << "length " << i << " : " << relLyTokens[i].size() << endl;
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

RationalTime SongLine::rationalDuration(int duration, int dots, bool triplet) const {
		int n = 1;
		int d = duration;
		
		//dots
		for( int i=0; i<dots; i++) {n = n*2 + 1; d = d*2; } //if ( dotted ) { n = 3; d = 2*duration; }
		
		if ( triplet ) { d = (d/2)*3; }
		//cout << n << "/" << d << endl;
		return RationalTime(n,d);
}

string SongLine::toText(string tok, RelLyToken::TextStatus ts, Representation repr) const {
	pvktrim(tok);
	
	if ( tok.size() == 0 )
		if ( repr == KERN ) return "."; else return "";
	if ( tok == "_" )
		if ( repr == KERN ) {
			if ( ts == RelLyToken::END_WORD || ts == RelLyToken::SINGLE_WORD || ts == RelLyToken::END_WORD_CONT || ts == RelLyToken::SINGLE_WORD_CONT ) return "|";
			else if ( ts == RelLyToken::IN_WORD || ts == RelLyToken::BEGIN_WORD || ts == RelLyToken::IN_WORD_CONT || ts == RelLyToken::BEGIN_WORD_CONT ) return "||";
			else return ".";
		}
		else return "";
		
	if ( tok.find(" --") != string::npos) tok.erase(tok.find(" --"));
	
	//if only a dash, print a warning. Should be an underscore.
	if ( tok.find_first_not_of("-") == string::npos ) {
		cerr << getLocation() << ": Warning: only a \"-\" under a note. Should be an \"_\"?" << endl;
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
	
	//remove " and &quot;
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
	
	res.push_back("%");
	res.push_back("% produced by wce2krn " + version + " (" + releasedate + ")");
	res.push_back("%");	
	res.push_back("\\version\"2.8.2\"");
	res.push_back("mBreak = { \\bar \"\" \\break }");
	res.push_back("x = {\\once\\override NoteHead #'style = #'cross }");
	res.push_back("\\let gl=\\glissando");
	res.push_back("\\header{ tagline = \"\" }");
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
	if ( !meterInvisible )
		res.push_back(" \\layout { indent = 0.0\\cm }");
	else {
		res.push_back(" \\layout { indent = 0.0\\cm");
		res.push_back("           \\context { \\Staff \\remove \"Bar_engraver\" \\remove \"Time_signature_engraver\" }");
		res.push_back("           \\context { \\Score \\remove \"Bar_number_engraver\" } }");
	}
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
	
	//comment
	res.push_back("!! produced by wce2krn " + version + " (" + releasedate + ")");

	
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
	for(int i=0; i<initialDots; i++) ss << '.';
	ss >> strduration;
	
	lyline.insert(pos, strduration);
	
	//cout << lyline << endl;
	
	return true;
}

string SongLine::getLyricsLine( int line ) const {
	return lyricsLines[line];
}

bool SongLine::checkMelisma() const {
	bool res = true;
		
	if ( relLyTokens.size() < 2 ) { cerr << getLocation() << ": Error: not enough lines" << endl; return false; }
	if ( text_ann.size() < 1 ) { cerr << getLocation() << ": Error: no annotations" << endl; return false; }
			
	//walk through the text annotation.
	//if continuation, either tie or slur should be present.
	//only check first text line

	for( int i=0; i<text_ann[0].size(); i++ ) {
		if ( ( text_ann[0][i] == RelLyToken::BEGIN_WORD_CONT ) ||
		     ( text_ann[0][i] == RelLyToken::SINGLE_WORD_CONT ) ||
		     ( text_ann[0][i] == RelLyToken::END_WORD_CONT ) ||
			 ( text_ann[0][i] == RelLyToken::IN_WORD_CONT ) ) {
			if ( relLyTokens[0][i].getIdentity() == RelLyToken::NOTE ) {
				if ( ( slurs_ann[i] != RelLyToken::START_SLUR ) &&
				     ( slurs_ann[i] != RelLyToken::IN_SLUR ) &&
					 ( slurs_ann[i] != RelLyToken::END_SLUR ) &&
					 ( ties_ann[i] != RelLyToken::START_TIE ) &&
					 ( ties_ann[i] != RelLyToken::CONTINUE_TIE ) &&
					 ( ties_ann[i] != RelLyToken::END_TIE ) ) {
						cerr << getLocation() << ": Error: slur or tie missing at note: " << relLyTokens[0][i].getToken() << endl;
						res = false;
				}
			}
		}
	}

	return res;

}

bool SongLine::checkTextPlacing() const {
	bool res = true;
	string syl = "*";
		
	if ( relLyTokens.size() < 2 ) { cerr << getLocation() << ": Error: not enough lines" << endl; return false; }
	if ( text_ann.size() < 1 ) { cerr << getLocation() << ": Error: no annotations" << endl; return false; }
			
	//walk through the music annotation
	//if \time or \times, no text should be in the lyrics
	//only check first text line

	for ( int i=0; i<relLyTokens[0].size(); i++ ) {
		if ( relLyTokens[0][i].getIdentity() != RelLyToken::NOTE ) {
			if ( i<text_ann[0].size() )
				if ( text_ann[0][i] == RelLyToken::NO_WORD ) {
					if ( i<relLyTokens[1].size() ) syl = relLyTokens[1][i].getToken();
					pvktrim(syl);
					if ( syl.size() != 0 )
						cerr << getLocation() << ": Error: \\time or \\times should not have lyrics: " << syl << endl;
				}
			//else
				//
		}
	}

	return res;

}


void SongLine::createAnnotations() {
	//stringstream ss;
	string st = "";
	string part;
	
	//STRINGSTREAM DOESNT WORK
	
	int field_width = 15;
		
	for ( int i=0; i<relLyTokens[0].size(); i++ ) {
		part = (relLyTokens[0][i].getToken() + "*");
		columnize(part, field_width); st = st + part;
		part = RelLyToken::printIdentity(relLyTokens[0][i].getIdentity());
		columnize(part, field_width); st = st + part;
		part = RelLyToken::printSlurStatus(slurs_ann[i]);
		columnize(part, field_width); st = st + part;
		part = RelLyToken::printTieStatus(ties_ann[i]);
		columnize(part, field_width); st = st + part;
		for ( int j=1; j<relLyTokens.size(); j++ ) {
			if ( i<relLyTokens[j].size() ) {
				part = (relLyTokens[j][i].getToken() + "*");
				columnize(part, field_width); st = st + part;
				if ( i<text_ann[j-1].size() ) {
					part = RelLyToken::printTextStatus(text_ann[j-1][i]);
					columnize(part, field_width); st = st + part;
				} else {
					part = "**ERROR**";
					columnize(part, field_width); st = st + part;
				}
			} else {
				part = "**ERROR**";
				columnize(part, field_width); st = st + part;
				part = "**ERROR**";
				columnize(part, field_width); st = st + part;
			}
		}
		annotations.push_back(st); st = "";
	}
	
}

void SongLine::printAnnotations() const {
	
	clog << getLocation() << endl;
	for (int i=0; i<annotations.size(); i++)
		clog << annotations[i] << endl;
		
	/*
	int field_width = 15;
	//clog << setw(field_width) << left << relLyTokens[0].size();
	//clog << setw(field_width) << left << relLyTokens[0].size();
	//clog << setw(field_width) << left << slurs_ann.size();
	//clog << setw(field_width) << left << ties_ann.size();
	//clog << setw(field_width) << left << relLyTokens[1].size();
	//clog << setw(field_width) << left << text_ann[0].size() << endl;
	//
	//clog << setw(field_width) << left << "Token";
	//clog << setw(field_width) << left << "Type";
	//clog << setw(field_width) << left << "Slurs";
	//clog << setw(field_width) << left << "Ties";
	//clog << setw(field_width) << left << "Text";
	//clog << setw(field_width) << left << "Type" << endl;

	for ( int i=0; i<relLyTokens[0].size(); i++ ) {
		clog << setw(field_width) << left << relLyTokens[0][i].getToken() + "*";
		clog << setw(field_width) << left << RelLyToken::printIdentity(relLyTokens[0][i].getIdentity());
		clog << setw(field_width) << left << RelLyToken::printSlurStatus(slurs_ann[i]);
		clog << setw(field_width) << left << RelLyToken::printTieStatus(ties_ann[i]);
		clog << flush;
		for ( int j=1; j<relLyTokens.size(); j++ ) {
			if ( i<relLyTokens[j].size() ) {
				clog << setw(field_width) << left << relLyTokens[j][i].getToken() + "*";
				if ( i<text_ann[j-1].size() )
					clog << setw(field_width) << left << RelLyToken::printTextStatus(text_ann[j-1][i]);
				else
					clog << setw(field_width) << left << "**ERROR**";
			} else {
				clog << setw(field_width) << left << "**ERROR**";
				clog << setw(field_width) << left << "**ERROR**";
			}
		}
		clog << endl;
	}
	*/

}

string SongLine::getLocation() const {
	stringstream ss;
	ss << phraseNo;
	string phr;
	ss >> phr;

	string fn = fileName;
	string::size_type pos;
	if ( (pos = fn.find_last_of("/")) != string::npos ) fn = fn.substr(pos+1);
	return fn + ": Record " + record + " - Strophe " + strophe + " - Phrase " + phr;
}

bool SongLine::checkTies() const {
	bool res = true;
	
	if ( relLyTokens.size() < 1 ) { cerr << getLocation() << ": Error: not enough lines" << endl; return false; }
	if ( relLyTokens[0].size() < 2 ) return true; //apparently one note on the line

	//walk trough note sequence. If slur end and previous note has slur start or in slur, and pitches are the same: should be a tie
	for( int i=1; i<relLyTokens[0].size(); i++ ) { //starting from the second token
		if ( relLyTokens[0][i].getIdentity() != RelLyToken::NOTE ) continue; //only notes
		if ( slurs_ann[i] == RelLyToken::END_SLUR ) {
			//search previous NOTE
			int p = i-1;
			while ( p>=0 && slurs_ann[p] != RelLyToken::START_SLUR && slurs_ann[p] != RelLyToken::IN_SLUR ) p--;
			if ( p == -1 ) {
				cerr << getLocation() << ": Error: slur ends at note: " << relLyTokens[0][i].getToken() << ", but no start" << endl;
				continue; //no previous note
			}
			//now p has index of previous note
			//
			if ( slurs_ann[p] == RelLyToken::START_SLUR || slurs_ann[p] == RelLyToken::IN_SLUR )
				if ( relLyTokens[0][p].getPitchClass() == relLyTokens[0][i].getPitchClass() ) {
					cerr << getLocation() << ": Warning: Slur should probably be a tie at notes: " << relLyTokens[0][p].getToken()
					     << " and " << relLyTokens[0][i].getToken() << endl;
					res = false;
				}
		}
	}
	
	return res;
}

bool SongLine::checkLengths() const {
	bool res = true;
	int length = relLyTokens[0].size();
	//check lengths.
	for ( int i=1; i<relLyTokens.size(); i++ ) {
		//cout << "length: " << length << endl;
		//cout << "txt_" << i << " : " << relLyTokens[i].size() << endl;
		if ( relLyTokens[i].size() < length ) {
			if ( wcelines.size() > i ) {
				//no error if last token is not a note or an END_TIE.
				
				//is it because of ties?
				//count number of ties at end of line
				int ti = ties_ann.size()-1;
				int ties = 0;
				while ( ties_ann[ti] == RelLyToken::START_TIE || 
						ties_ann[ti] == RelLyToken::CONTINUE_TIE ||
						ties_ann[ti] == RelLyToken::END_TIE ) {
					ties = ties + 1;
					ti = ti - 1;
				}
				ties = ties-1; //END_TIE doesn't count.
				//cout << "ties  : " << ties << endl;
				//cout << "length: " << length << endl;
				//cout << "txt   : " << relLyTokens[i].size() << endl; 
				if ( relLyTokens[i].size()+ties >= length ) { 
					res = true;
					continue;
				}
				
				/* THIS IS HANDLED SOMEWHERE ELSE
				//is it because of rests?
				int rests = 0;
				int ri = length-1;
				while ( relLyTokens[0][ri].isRest() ) {
					ri = ri - 1;
					rests = rests + 1;
				}
				if ( relLyTokens[i].size()+rests >= length ) {
					res = true;
					continue;
				}
				*/
				
				cerr << getLocation() << ": Warning: textline to short: " << endl;
				cerr << wcelines[0] << endl;
				cerr << wcelines[i] << endl;
				res = false;
			}
		}
	}
	return res;
}