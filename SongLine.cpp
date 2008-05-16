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
//#include <locale>
using namespace std;

#undef yyFlexLexer
#define yyFlexLexer LilyFlexLexer
#include <FlexLexer.h>

#undef yyFlexLexer
#define yyFlexLexer TextFlexLexer
#include <FlexLexer.h>


SongLine::SongLine(vector<string> lines, RationalTime upb, TimeSignature timesig, int duration, int dots, int octave, char pitchclass, bool initialtriplet, int keysig, int mtempo, string lytempo, int barnumber, bool meterinvisible, bool eachphrasenewstaff, string filename, int phraseno, int numphrases, string recordno, string stropheno, string str_title, int wcelineno, vector<string> fField) :
																   wcelines(lines),
																   initialUpbeat(upb),
																   initialTimeSignature(timesig),
																   initialDuration(duration),
																   initialDots(dots),
																   initialOctave(octave),
																   initialLastPitchClass(pitchclass),
																   initialTripletStatus(initialtriplet),
																   initialBarnumber(barnumber),
																   finalTimeSignature(timesig),
																   finalUpbeat(RationalTime(0,1)),
																   finalOctave(4),
																   finalDuration(0),
																   finalDots(0),
																   finalLastPitchClass(pitchclass),
																   finalBarnumber(-1),
																   finalTripletStatus(false),
																   keySignature(keysig),
																   midiTempo(mtempo),
																   lyTempo(lytempo),
																   translationMade(false),
																   meterInvisible(meterinvisible),
																   eachPhraseNewStaff(eachphrasenewstaff),
																   fileName(filename),
																   phraseNo(phraseno),
																   numPhrases(numphrases),
																   record(recordno),
																   strophe(stropheno),
																   title(str_title),
																   WCELineNumber(wcelineno),
																   footerField(fField) {
	translate();
}

SongLine::SongLine() : wcelines(vector<string>()),
					   initialUpbeat(RationalTime(0,1)),
					   initialTimeSignature(TimeSignature()),
					   initialDuration(0),
					   initialDots(0),
					   initialOctave(4),
					   initialLastPitchClass('g'),
					   initialTripletStatus(false),
					   initialBarnumber(0),
					   keySignature(0),
					   midiTempo(120),
					   lyTempo("4=120"),
					   translationMade(false),
					   meterInvisible(false),
					   eachPhraseNewStaff(true),
					   fileName("[noname]"),
					   phraseNo(0),
					   numPhrases(0),
					   record("unknown"),
					   strophe("0"),
					   title(""),
					   WCELineNumber(0),
					   footerField(vector<string>()) {
	translate();
}

SongLine::SongLine(const SongLine& sl) : wcelines(sl.getWceLines()),
									     initialUpbeat(sl.getInitialUpbeat()),
										 initialTimeSignature(sl.getInitialTimeSignature()),
										 initialDuration(sl.getInitialDuration()),
										 initialDots(sl.getInitialDots()),
										 initialOctave(sl.getInitialOctave()),
										 initialLastPitchClass(sl.getInitialLastPitchClass()),
										 initialTripletStatus(sl.initialTripletStatus),
										 initialBarnumber(sl.getInitialBarnumber()),
										 finalTimeSignature(sl.getFinalTimeSignature()),
										 finalUpbeat(sl.getFinalUpbeat()),
										 finalOctave(sl.getFinalOctave()),
										 finalDuration(sl.getFinalDuration()),
										 finalLastPitchClass(sl.getFinalLastPitchClass()),
										 finalDots(sl.getFinalDots()),
										 finalBarnumber(sl.getFinalBarnumber()),
										 finalTripletStatus(sl.finalTripletStatus),
										 keySignature(sl.getKeySignature()),
										 midiTempo(sl.getMidiTempo()),
										 lyTempo(sl.lyTempo),
										 translationMade(sl.translationMade),
										 relLyTokens(sl.relLyTokens),
										 absLyTokens(sl.absLyTokens),
										 kernTokens(sl.kernTokens),
										 absLyLine(sl.absLyLine),
										 lyricsLines(sl.lyricsLines),
										 meterInvisible(sl.meterInvisible),
										 eachPhraseNewStaff(sl.eachPhraseNewStaff),
										 text_ann(sl.text_ann),
										 ties_ann(sl.ties_ann),
										 slurs_ann(sl.slurs_ann),
										 fileName(sl.fileName),
										 phraseNo(sl.phraseNo),
										 numPhrases(sl.numPhrases),
										 record(sl.record),
										 strophe(sl.strophe),
										 title(sl.title),
										 annotations(sl.annotations),
										 WCELineNumber(sl.WCELineNumber),
										 footerField(sl.footerField) {
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
	//clog << getWCELineNumber() << endl;
	
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
	bool currentTripletStatus = initialTripletStatus;
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
	//first do the melody
	string token = "";
	absLyTokens.push_back(vector<string>());
	kernTokens.push_back(vector<string>()); //line for kern spine
	kernTokens.push_back(vector<string>()); //line for location spine
	
	
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
			kernTokens[1].push_back(barstr);
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
						cerr << getLocation() << ": Error: bar too long: " << currentBarnumber << endl;
					}
					if ( timeInBar >= currentTimeSignature.getRationalTime() ) { // new bar
						//write in **kern
						stringstream ss;
						currentBarnumber++;
						ss << "=" << currentBarnumber;
						string barstr = "";
						ss >> barstr;
						kernTokens[0].push_back(barstr);
						kernTokens[1].push_back(barstr);
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
				kernTokens[1].push_back( (*rl_it).getWCEPosition() );
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
					kernTokens[1].push_back(barstr);
					timeInBar = RationalTime(0,1);
				}				
				currentTimeSignature = (*rl_it).getTimeSignature();
				//absLy: just copy the token
				absLyTokens[0].push_back((*rl_it).getToken());
				//kern: conver tot kern
				//firstOfLine gaat mis als hele song wordt geconverteerd. Dan is er geen preamble voor elke regel.
				//if (!firstOfLine) kernTokens[0].push_back(currentTimeSignature.getKernTimeSignature());
				kernTokens[0].push_back(currentTimeSignature.getKernTimeSignature());
				kernTokens[1].push_back((*rl_it).getWCEPosition());
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
			
			case RelLyToken::BARLINE: {
				ties_ann.push_back(RelLyToken::NO_TIE_INFO);
				slurs_ann.push_back(RelLyToken::NO_SLUR_INFO);
			} break;

			case RelLyToken::UNKNOWN: {
				if ( (*rl_it).getToken().size() > 0 )
					cerr << getLocation() << ": Warning: Unknown token: \"" << (*rl_it).getToken() << "\"" << endl;
					ties_ann.push_back(RelLyToken::NO_TIE_INFO);
					slurs_ann.push_back(RelLyToken::NO_SLUR_INFO);
				//exit(1);
			} break;
			
			case RelLyToken::GRACE: {
				//clog << "GRACE detected" << endl;
				
				//figure out the current octave after the grace notes
				//make songline of gracenotes and ask for finallastpitchclass and final octave

				
				if (lastPitchClass != 's' && lastPitchClass != 'r') finalLastPitchClass = lastPitchClass;
				string gracestring = (*rl_it).getToken();
				string::size_type grpos1, grpos2;
				//remove "\grace{" and "}"
				if ( ( grpos1 = gracestring.find("{") ) == string::npos ) {
					cerr << getLocation() << ": Error: \"{\" missing in gracenote. This should never happen." << endl;
					exit(-1);
				}
				if ( ( grpos2 = gracestring.find("}") ) == string::npos ) {
					cerr << getLocation() << ": Error: \"}\" missing in gracenote. This should never happen." << endl;
					exit(-1);
				}
				gracestring = gracestring.substr(grpos1+1, grpos2 - grpos1 -1 );
				pvktrim(gracestring);
				vector<string> gracelines;
				gracelines.push_back(gracestring);
				SongLine grace   (gracelines,
							      RationalTime(0,1), //from previous line
								  currentTimeSignature,
								  8,
								  0,
								  currentOctave,
								  lastPitchClass,
								  false,
								  getKeySignature(),
								  getMidiTempo(),
								  getLyTempo(),
								  currentBarnumber,
								  getMeterInvisible(),
								  getEachPhraseNewStaff(),
								  fileName,
								  phraseNo,
								  numPhrases,
								  record,
								  strophe,
								  title,
								  WCELineNumber,
								  footerField);

				//grace.printAnnotations();
				//cout << grace.getInitialOctave() << " - " << grace.getFinalOctave() << endl;
				currentOctave = grace.getFinalOctave();
				lastPitchClass = grace.getFinalLastPitchClass();
				
				ties_ann.push_back(RelLyToken::NO_TIE_INFO);
				slurs_ann.push_back(RelLyToken::NO_SLUR_INFO);
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
	finalTripletStatus = currentTripletStatus;
	if (lastPitchClass != 's' && lastPitchClass != 'r') finalLastPitchClass = lastPitchClass;
	if (timeInBar == currentTimeSignature.getRationalTime()) {
		finalUpbeat = RationalTime(0,1);
		finalBarnumber = currentBarnumber+1;
	} else {
		finalUpbeat = currentTimeSignature.getRationalTime() - timeInBar;
		finalBarnumber = currentBarnumber;
	}
	if (meterInvisible) finalUpbeat = RationalTime(0,1);

	//now do text lines (if any). Take kern as reference. (10-10-2007: was wrong decision. ly shoud be taken as ref because it is the input...
	string str;
	vector<string>::iterator krn_it;
	int numLines = wcelines.size();
	vector<RelLyToken::TextStatus> vrt;
	for( int i = 1; i<numLines; i++ ) {
		absLyTokens.push_back(vector<string>());
		kernTokens.push_back(vector<string>()); //text line
		kernTokens.push_back(vector<string>()); //line for location spine
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
					kernTokens[2*i].push_back( "*" );
					kernTokens[2*i+1].push_back( "*" );
				}
			else
				for ( int i = 1; i<numLines; i++ ) {
					kernTokens[2*i].push_back( (*krn_it) );
					kernTokens[2*i+1].push_back( (*krn_it) );
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
					kernTokens[2*i].push_back( "." );
					kernTokens[2*i+1].push_back( "." );
					// rest at end of line could be tolerated
					if ( relLyTokens[0][relly_index].getPitchClass() == 'r' ) relLyTokens[i].push_back( RelLyToken("", "", getLocation(), RelLyToken::TEXT, false) );
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
						kernTokens[2*i].push_back( toText( relLyTokens[i][relly_index].getToken(), currentTextStatus, KERN ) );
						kernTokens[2*i+1].push_back( relLyTokens[i][relly_index].getWCEPosition() );						
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
	bool is_music = true; //first line is music line

	string::size_type pos;
	string ctoken = "";
	vector<string>::const_iterator it;
	int pos_in_line = 0;
	int line_offset = 0;		
	for( it = wcelines.begin(); it != wcelines.end(); it++) {
		relLyTokens.push_back(emptyline);
		line = *it;

		//ask flex
		string flexline = line + " "; //add whitespace at end!!
		istringstream iss(flexline);
		FlexLexer* lexer;
		
		//cout << flexline << endl;
		
		string lexline = "";
		
		if ( is_music ) {
		  pos_in_line = 0;
		  lexer = new LilyFlexLexer(&iss);

		  int tok = lexer->yylex();
		  bool eerste = true;
		  while(tok != 0){
			
			if ( tok == -1 ) {
			  ctoken = lexer->YYText();
			  if ( ctoken == "." )
			  	cerr << getLocation() << ": Warning: Unrecognized dot. Probably duration without digits: " << ctoken << endl;
			  else
			  	cerr << getLocation() << ": Warning: Unrecognized token: " << ctoken << endl;
			}
			else if ( tok == 4 ) {
				//clog << getLocation() << ": INSTR: " << lexer->YYText() << endl;
			}
			else if ( tok == 5 ) { //grace
				ctoken = lexer->YYText();
				//put this with "\grace{" and "}" int relLyTokens 
				(relLyTokens.back()).push_back(RelLyToken(ctoken, getLocation(), convertToString(WCELineNumber) + ":" + convertToString(pos_in_line), RelLyToken::GRACE, is_music));
		  	}	
			else if ( tok == 6 ) { //barline
				ctoken = lexer->YYText();
				pvktrim(ctoken);
				(relLyTokens.back()).push_back(RelLyToken(ctoken, getLocation(), convertToString(WCELineNumber) + ":" + convertToString(pos_in_line), RelLyToken::BARLINE, is_music));
		  	}	
			else if ( tok == 2 ) { //time
				ctoken = lexer->YYText();
				//put this with "\grace{" and "}" int relLyTokens 
				(relLyTokens.back()).push_back(RelLyToken(ctoken, getLocation(), convertToString(WCELineNumber) + ":" + convertToString(pos_in_line), RelLyToken::TIME_COMMAND, is_music));
		  	}	
			else if ( tok == 3 ) { //times
				ctoken = lexer->YYText();
				//put this with "\grace{" and "}" int relLyTokens 
				(relLyTokens.back()).push_back(RelLyToken(ctoken, getLocation(), convertToString(WCELineNumber) + ":" + convertToString(pos_in_line), RelLyToken::TIMES_COMMAND, is_music));
		  	}
		  	else if ( tok == -2 ) { //whitespace
		  	}
		  	else if ( tok == 7 ) { //separate tie, should be added to last note
		  		//cout << "separate tie" << endl;
		  		int ix = relLyTokens.back().size() -1;
		  		//cout << ix << endl;
		  		while ( ix >= 0 && (relLyTokens.back())[ix].getIdentity() != RelLyToken::NOTE )
		  			ix--;
		  		if ( ix < 0 ) cerr << getLocation() << ": Warning: tie symbol could not be attached to a note." << endl;
		  		else
		  			(relLyTokens.back())[ix].addTie();
		  	}	
		  	else if ( tok == 8 ) { //separate closing brace, should be added to last note
		  		//cout << "separate closing brace" << endl;
		  		int ix = relLyTokens.back().size() -1;
		  		//cout << ix << endl;
		  		while ( ix >= 0 && (relLyTokens.back())[ix].getIdentity() != RelLyToken::NOTE )
		  			ix--;
		  		if ( ix < 0 ) cerr << getLocation() << ": Warning: closing brace symbol could not be attached to a note." << endl;
		  		else
		  			(relLyTokens.back())[ix].addClosingBrace();
		  	}	
			else {
			  ctoken = lexer->YYText();
			  pvktrim(ctoken);
			  //add token to list of tokens
			  (relLyTokens.back()).push_back(RelLyToken(ctoken, getLocation(), convertToString(WCELineNumber) + ":" + convertToString(pos_in_line), RelLyToken::NOTE, is_music));
			
			} 
			
			//cout << tok << " - " << lexer->YYText() << " - " << pos_in_line << endl;

			pos_in_line += lexer->YYLeng();			
			tok = lexer->yylex();
			
		  }		  
		  delete lexer;
		  
		} else { //textline
		  
		  pos_in_line = 0;
		  int current_note_index = 0;
		  
		  lexer = new TextFlexLexer(&iss);

		  int tok = lexer->yylex();
		  bool eerste = true;
		  while(tok != 0){
			if ( tok == -1 ) {
			  cerr << getLocation() << ": Warning: Unrecognized token: " << lexer->YYText() << endl;
			} else if ( tok == 2 ) { //whitespace.
				//cout << "Whitespace detected: " << lexer->YYLeng() << endl;
			} else {
			  ctoken = lexer->YYText();
			  pvktrim(ctoken);
			  //if not a note in corresponding music, add empty tokens first.
			  while ( relLyTokens[0][current_note_index].getIdentity() != RelLyToken::NOTE || relLyTokens[0][current_note_index].isRest() ) {
			  	(relLyTokens.back()).push_back(RelLyToken("", getLocation(), convertToString(WCELineNumber+line_offset) + ":" + convertToString(pos_in_line), RelLyToken::TEXT, is_music));
			  	current_note_index++;
			  	if ( current_note_index > relLyTokens[0].size() ) {
			  		cerr << getLocation() << ": Error: too much text: " << ctoken << endl;
			  		exit(1);
			  	}
			  }
			  //add token to list of tokens
			  (relLyTokens.back()).push_back(RelLyToken(ctoken, getLocation(), convertToString(WCELineNumber+line_offset) + ":" + convertToString(pos_in_line), RelLyToken::TEXT, is_music));
			  current_note_index++;
			} 
			
			// any case: add length to position;
			//cout << "pos in line: " << pos_in_line << endl;
			pos_in_line += lexer->YYLeng();
			
			tok = lexer->yylex();
		  }		  
		  delete lexer;
		  
		  //add text tokens if the line is not long enough (e.g. when a rest or \time command is at the and)
		  while ( relLyTokens.back().size() < relLyTokens[0].size() ) {
		  	(relLyTokens.back()).push_back(RelLyToken("", getLocation(), convertToString(WCELineNumber+line_offset) + ":" + convertToString(pos_in_line), RelLyToken::TEXT, is_music));
		  }
		  
		}
		is_music = false;
		line_offset++;
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

vector<string> SongLine::getLyLine(bool absolute, bool lines, int ly_ver) const{
	// if lines:
	// take care of triples that are across line breaks.
	
	vector<string> res;
		
	if (!absolute) {
		res = wcelines;
		
		// make sure first note has duration
		// do this before changing the line (like adding \times 2/3 at the beginning)
		inheritFirstLynoteDuration( res[0], initialDuration);

		if ( lines ) {
			if ( initialTripletStatus ) {
				if ( res.size() > 0 ) res[0] = "\\times 2/3 { " + res[0];
			}
			if ( finalTripletStatus ) {
				if ( res.size() > 0 ) res[0] = res[0] + "}";
			}
		}

		// add break at end
		// if there is a baline, don't add the empty bar.
		
		// if not barline is provided at the end of the line, print an appropriate one.
		
		if (!endsWithBarLine() ) {
			if ( phraseNo == numPhrases ) {
				if ( res.size() >0 ) res[0] = res[0] + " \\bar \"|.\"";
			} else {
				if ( finalUpbeat.getNumerator() == 0 ) {
					if ( res.size() >0 ) res[0] = res[0] + " \\mBreak \\bar \"|\"";
					}
				else {
					if ( res.size() >0 ) res[0] = res[0] + " \\mBreak";
				}
			}
		}
		else
			if ( phraseNo != numPhrases ) {
				if ( res.size() >0 ) res[0] = res[0] + " \\bBreak";
			}
		
	}
	
	//cout << phraseNo << " of " << numPhrases << endl;
	
	return res;
}

vector<string> SongLine::getLyBeginSignature(bool absolute, bool lines, bool weblily, int ly_ver) const {
	vector<string> res;

	string key;
	string mode;
	//guess most probable key (not optimal)
	switch( keySignature ) {
		//major 0
		case 7: key = "cis \\major"; break;
		case 6: key = "fis \\major"; break;
		case 5: key = "b \\major"; break;
		case 4: key = "e \\major"; break;
		case 3: key = "a \\major"; break;
		case 2: key = "d \\major"; break;
		case 1: key = "g \\major"; break;
		case 0: key = "c \\major"; break;
		case -1: key = "f \\major"; break;
		case -2: key = "bes \\major"; break;
		case -3: key = "es \\major"; break;
		case -4: key = "as \\major"; break;
		case -5: key = "des \\major"; break;
		case -6: key = "ges \\major"; break;
		case -7: key = "ces \\major"; break;
		//minor 30
		case 37: key = "ais \\minor"; break;
		case 36: key = "dis \\minor"; break;
		case 35: key = "gis \\minor"; break;
		case 34: key = "cis \\minor"; break;
		case 33: key = "fis \\minor"; break;
		case 32: key = "b \\minor"; break;
		case 31: key = "e \\minor"; break;
		case 30: key = "a \\minor"; break;
		case 29: key = "d \\minor"; break;
		case 28: key = "g \\minor"; break;
		case 27: key = "c \\minor"; break;
		case 26: key = "f \\minor"; break;
		case 25: key = "bes \\minor"; break;
		case 24: key = "es \\minor"; break;
		case 23: key = "as \\minor"; break;
		//ionian 60
		case 67: key = "cis \\ionian"; break;
		case 66: key = "fis \\ionian"; break;
		case 65: key = "b \\ionian"; break;
		case 64: key = "e \\ionian"; break;
		case 63: key = "a \\ionian"; break;
		case 62: key = "d \\ionian"; break;
		case 61: key = "g \\ionian"; break;
		case 60: key = "c \\ionian"; break;
		case 59: key = "f \\ionian"; break;
		case 58: key = "bes \\ionian"; break;
		case 57: key = "es \\ionian"; break;
		case 56: key = "as \\ionian"; break;
		case 55: key = "des \\ionian"; break;
		case 54: key = "ges \\ionian"; break;
		case 53: key = "ces \\ionian"; break;
		//dorian 90
		case 97: key = "dis \\dorian"; break;
		case 96: key = "gis \\dorian"; break;
		case 95: key = "cis \\dorian"; break;
		case 94: key = "fis \\dorian"; break;
		case 93: key = "b \\dorian"; break;
		case 92: key = "e \\dorian"; break;
		case 91: key = "a \\dorian"; break;
		case 90: key = "d \\dorian"; break;
		case 89: key = "g \\dorian"; break;
		case 88: key = "c \\dorian"; break;
		case 87: key = "f \\dorian"; break;
		case 86: key = "bes \\dorian"; break;
		case 85: key = "es \\dorian"; break;
		case 84: key = "as \\dorian"; break;
		case 83: key = "des \\dorian"; break;
		//phrygian 120
		case 127: key = "eis \\phrygian"; break;
		case 126: key = "ais \\phrygian"; break;
		case 125: key = "dis \\phrygian"; break;
		case 124: key = "gis \\phrygian"; break;
		case 123: key = "cis \\phrygian"; break;
		case 122: key = "fis \\phrygian"; break;
		case 121: key = "b \\phrygian"; break;
		case 120: key = "e \\phrygian"; break;
		case 119: key = "a \\phrygian"; break;
		case 118: key = "d \\phrygian"; break;
		case 117: key = "g \\phrygian"; break;
		case 116: key = "c \\phrygian"; break;
		case 115: key = "f \\phrygian"; break;
		case 114: key = "bes \\phrygian"; break;
		case 113: key = "es \\phrygian"; break;
		//lydian 150
		case 157: key = "fis \\lydian"; break;
		case 156: key = "b \\lydian"; break;
		case 155: key = "e \\lydian"; break;
		case 154: key = "a \\lydian"; break;
		case 153: key = "d \\lydian"; break;
		case 152: key = "g \\lydian"; break;
		case 151: key = "c \\lydian"; break;
		case 150: key = "f \\lydian"; break;
		case 149: key = "bes \\lydian"; break;
		case 148: key = "es \\lydian"; break;
		case 147: key = "as \\lydian"; break;
		case 146: key = "des \\lydian"; break;
		case 145: key = "ges \\lydian"; break;
		case 144: key = "ces \\lydian"; break;
		case 143: key = "ces \\lydian"; break;
		//mixolydian 180
		case 187: key = "gis \\mixolydian"; break;
		case 186: key = "cis \\mixolydian"; break;
		case 185: key = "fis \\mixolydian"; break;
		case 184: key = "b \\mixolydian"; break;
		case 183: key = "e \\mixolydian"; break;
		case 182: key = "a \\mixolydian"; break;
		case 181: key = "d \\mixolydian"; break;
		case 180: key = "g \\mixolydian"; break;
		case 179: key = "c \\mixolydian"; break;
		case 178: key = "f \\mixolydian"; break;
		case 177: key = "bes \\mixolydian"; break;
		case 176: key = "es \\mixolydian"; break;
		case 175: key = "as \\mixolydian"; break;
		case 174: key = "des \\mixolydian"; break;
		case 173: key = "ges \\mixolydian"; break;
		//aeolian 210
		case 217: key = "ais \\aeolian"; break;
		case 216: key = "dis \\aeolian"; break;
		case 215: key = "gis \\aeolian"; break;
		case 214: key = "cis \\aeolian"; break;
		case 213: key = "fis \\aeolian"; break;
		case 212: key = "b \\aeolian"; break;
		case 211: key = "e \\aeolian"; break;
		case 210: key = "a \\aeolian"; break;
		case 209: key = "d \\aeolian"; break;
		case 208: key = "g \\aeolian"; break;
		case 207: key = "c \\aeolian"; break;
		case 206: key = "f \\aeolian"; break;
		case 205: key = "bes \\aeolian"; break;
		case 204: key = "es \\aeolian"; break;
		case 203: key = "as \\aeolian"; break;		
		//locrian 240
		case 247: key = "bis \\locrian"; break;
		case 246: key = "eis \\locrian"; break;
		case 245: key = "ais \\locrian"; break;
		case 244: key = "dis \\locrian"; break;
		case 243: key = "gis \\locrian"; break;
		case 242: key = "cis \\locrian"; break;
		case 241: key = "fis \\locrian"; break;
		case 240: key = "b \\locrian"; break;
		case 239: key = "e \\locrian"; break;
		case 238: key = "a \\locrian"; break;
		case 237: key = "d \\locrian"; break;
		case 236: key = "g \\locrian"; break;
		case 235: key = "c \\locrian"; break;
		case 234: key = "f \\locrian"; break;
		case 233: key = "bes \\locrian"; break;
		
	}
	
	res.push_back("%");
	res.push_back("% produced by wce2krn " + version + " (" + releasedate + ")");
	res.push_back("%");	
	if ( ly_ver == 10 ) res.push_back("\\version\"2.10.0\"");
	else if ( ly_ver == 11 ) res.push_back("\\version\"2.11.0\"");
	else res.push_back("\\version\"2.8.2\"");
	if ( weblily ) {
		res.push_back("#(append! paper-alist '((\"long\" . (cons (* 210 mm) (* 2000 mm)))))");
		res.push_back("#(set-default-paper-size \"long\")");
		//res.push_back("#(set-global-staff-size 10)");
		//res.push_back("\\paper {");
		//res.push_back("  line-width = 3.5 \\in");
		//res.push_back("  between-system-space = 1 \\mm");
		//res.push_back("  between-system-padding = 2 \\mm");
		//res.push_back("  top-margin = 1 \\mm");
		//res.push_back("  bottom-margin = 1 \\mm");
		//res.push_back("}");
	}
	if ( eachPhraseNewStaff ) {
		res.push_back("mBreak = { \\bar \"\" \\break }");
		res.push_back("bBreak = { \\break }");
	} else {
		res.push_back("mBreak = { }");
		res.push_back("bBreak = { }");
	}
	res.push_back("x = {\\once\\override NoteHead #'style = #'cross }");
	res.push_back("\\let gl=\\glissando");
	//** for instrumental music
	res.push_back("ficta = {\\once\\set suggestAccidentals = ##t}");
	res.push_back("sb = {\\breathe}");
	res.push_back("fine = {\\once\\override Score.RehearsalMark #'self-alignment-X = #1 \\mark \\markup {\\italic{Fine}}}");
	res.push_back("dc = {\\once\\override Score.RehearsalMark #'self-alignment-X = #1 \\mark \\markup {\\italic{D.C.}}}");
	res.push_back("dcf = {\\once\\override Score.RehearsalMark #'self-alignment-X = #1 \\mark \\markup {\\italic{D.C. al Fine}}}");
	res.push_back("dcc = {\\once\\override Score.RehearsalMark #'self-alignment-X = #1 \\mark \\markup {\\italic{D.C. al Coda}}}");
	res.push_back("ds = {\\once\\override Score.RehearsalMark #'self-alignment-X = #1 \\mark \\markup {\\italic{D.S.}}}");
	res.push_back("dsf = {\\once\\override Score.RehearsalMark #'self-alignment-X = #1 \\mark \\markup {\\italic{D.S. al Fine}}}");
	res.push_back("dsc = {\\once\\override Score.RehearsalMark #'self-alignment-X = #1 \\mark \\markup {\\italic{D.S. al Coda}}}");
	//** end for instrumental music
	res.push_back("\\header{ tagline = \"\"");
	string songtitle = title;
	if ( songtitle.size() == 0 ) {
		songtitle = "piece = \"Record " + record + " - Strophe " + strophe;
		if ( lines ) {
			stringstream s;
			s << phraseNo;
			string str_phraseNo = "";
			s >> str_phraseNo;
			songtitle = songtitle + " - Phrase " + str_phraseNo;
		}
		songtitle = songtitle + "\"";
	} else {
		songtitle = "title = \"" + songtitle;
		if ( lines ) {
			stringstream s;
			s << phraseNo;
			string str_phraseNo = "";
			s >> str_phraseNo;
			songtitle = songtitle + " (phrase " + str_phraseNo + ")";
		}
		songtitle = songtitle + "\"";		
	}
	if ( !weblily ) res.push_back(songtitle);
	res.push_back("}");
	res.push_back("\\score {{");
	
	//now write key
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
	
	//tempo
	res.push_back("\\tempo " + lyTempo);
	res.push_back("\\override Score.MetronomeMark #'transparent = ##t");
	
	//** for instrumental music
	res.push_back("\\override Score.RehearsalMark #'break-visibility = #(vector #t #t #f)");
	
	return res;
}

vector<string> SongLine::getLyEndSignature(int ly_ver, bool lines) const {
	vector<string> res;
	
	res.push_back(" }}");
	res.push_back(" \\midi { }"); //tempo is in the score block. This works for ly 2.8, 2.10 and 2.11
	if ( !meterInvisible )
		res.push_back(" \\layout { indent = 0.0\\cm }");
	else {
		res.push_back(" \\layout { indent = 0.0\\cm");
		res.push_back("           \\context { \\Staff \\remove \"Bar_engraver\" \\remove \"Time_signature_engraver\" }");
		res.push_back("           \\context { \\Score \\remove \"Bar_number_engraver\" } }");
	}
	res.push_back("}");
	
	if (!lines) {
		if ( footerField.size() > 0 ) {
			//first figure out whether there is something in it at all
			bool doen = true;
			if ( footerField.size() == 1) {
				string tmpstr = footerField[0];
				pvktrim(tmpstr);
				if ( tmpstr.size() == 0 ) doen = false; 
			}
			if(doen) {
				for ( int i = 0; i < footerField.size(); i++ )
					res.push_back( footerField[i] );
			}
		}
	}	
	return res;
}

vector<string> SongLine::getKernLine(bool lines) const {
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
	
	//add closing barline to last line
	//otherwise only add barline at end a line of a meterless song.
	if ( phraseNo == numPhrases ) {
		s = "";
		for( int i=0; i < kernTokens.size(); i++) s = s + "==|!" + "\t";
		s = s.substr(0,s.size()-1); //remove last tab
		res.push_back(s);
	} else {
		s = "";
		if ( ( meterInvisible && lines ) || lines) {
			for( int i=0; i < kernTokens.size(); i++) s = s + "==||" + "\t";
			s = s.substr(0,s.size()-1); //remove last tab
			res.push_back(s);
		}
	}
	
	
	return res;
}

vector<string> SongLine::getKernBeginSignature(bool lines) const {
	vector<string> res;
	string s;
	if ( kernTokens.size() == 0 ) return res;
		
	//identify spine
	s = "";
	for(int i=0; i < kernTokens.size() / 2; i++ ) {
		if (i == 0 ) s = s + "**kern" + "\t" + "**loc" + "\t"; else s = s + "**text" + "\t" + "**loc" + "\t";
	}
	//remove last tab.
	s = s.substr(0,s.size()-1);
	res.push_back(s);
	
	//time signature
	s = "";
	for(int i=0; i < kernTokens.size() / 2; i++ ) {
		if ( i == 0 ) s = s + initialTimeSignature.getKernTimeSignature() + "\t*\t"; else s = s + "*\t*\t";
	}
	s = s.substr(0,s.size()-1);
	res.push_back(s);

	//midi tempo
	stringstream ss;
	ss << midiTempo;
	string midiTempo_str;
	ss >> midiTempo_str;
	s = "";
	for(int i=0; i < kernTokens.size() / 2; i++ ) {
		if ( i == 0 ) s = s + "*MM" + midiTempo_str + "\t*\t"; else s = s + "*\t*\t";
	}
	s = s.substr(0,s.size()-1);
	res.push_back(s);
	
	//key signature
	int tempkey = keySignature;
	
	tempkey = ((tempkey+15)%30)-15;
		
	string keys = "";
	if (tempkey < 0) keys = "b-e-a-d-g-c-f-";
	if (tempkey > 0) keys = "f#c#g#d#a#e#b#";

	int ks = abs(tempkey);
	s = "";
	for( int i=0; i< kernTokens.size() / 2; i++ ) {
		if ( i == 0) s = s + "*k[" + keys.substr(0,2*ks) + "]\t*\t"; else s = s + "*\t*\t";
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
	for(int i=0; i < kernTokens.size() / 2; i++ ) {
		s = s + "*-" + "\t*-\t";
	}
	s = s.substr(0,s.size()-1);
	res.push_back(s);
	
	//comment
	res.push_back("!! produced by wce2krn " + version + " ( released on " + releasedate + ")");

	
	return res;
}

string SongLine::upbeatToString(RationalTime t) const {
	string res;
	
	if (initialUpbeat == 0 ) return "";
	if (meterInvisible) return "";
	
	stringstream ss;
	
	RationalTime rnum = initialUpbeat / RationalTime(1,32);
	int num = rnum.getNumerator()/rnum.getDenominator();
	ss << num;
	
	ss >> res;
	res = "32*" + res;
	
	return res;

}

bool SongLine::inheritFirstLynoteDuration( string & lyline, int duration) const {
	//locale loc("");
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
	
	RelLyToken::Accidental ac = relLyTokens[0][index].getAccidental();
	
	//find index of end of first note	
	while ( index > 0 ) {
		pos = lyline.find('\t');
		index--;
	}
	
	//ptichclass
	if ( (pos = lyline.find_first_of("abcdefgsr", pos)) == string::npos) return false;
		pos++;

	/*
	-is
	-isis
	-es
	-eses
	es
	as
	eses
	ases
	*/

	//alterations
	/*
	if ( pos+1 < lyline.size() { //not past line end
	
		if ( lyline[pos+1] == 's' ) { //es, as, eses, asas
			if ( substr
		}
	
		if ( lyline[pos+1] == 'i' || lyline[pos+1] == 'e') { //-is -isis
		}
	}
	*/
	
	string s = "s";
	
	switch( ac ) {
		case RelLyToken::DOUBLE_FLAT:
			if ( lyline[pos] == 's' ) // eses or ases
				pos = pos + 3;
			else
				pos = pos + 4;
			break;
		case RelLyToken::FLAT:
			if ( lyline[pos] == 's' ) // es or as
				pos = pos + 1;
			else
				pos = pos + 2;
			break;
		case RelLyToken::SHARP:
			pos = pos + 2;
			break;
		case RelLyToken::DOUBLE_SHARP:
			pos = pos + 4;
			break;
	}

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
	
	if ( relLyTokens.size() < 2 ) { return true; } //no text. check not needed
	if ( text_ann.size() < 1 ) { cerr << getLocation() << ": Error: no annotations" << endl; return false; }
			
	//walk through the text annotation.
	//if continuation, either tie or slur should be present.
	//only check first text line
	
	char lastPC; //remember last pitch class to compare -> slur or tie
	lastPC = relLyTokens[0][0].getPitchClass();
	int lastNoteIndex = 0;
	
	for( int i=0; i<text_ann[0].size(); i++ ) {
		if ( ( text_ann[0][i] == RelLyToken::BEGIN_WORD_CONT ) ||
		     ( text_ann[0][i] == RelLyToken::SINGLE_WORD_CONT ) ||
		     ( text_ann[0][i] == RelLyToken::END_WORD_CONT ) ||
			 ( text_ann[0][i] == RelLyToken::IN_WORD_CONT ) ) {
			if ( relLyTokens[0][i].getIdentity() == RelLyToken::NOTE ) {
				if ( //( slurs_ann[i] != RelLyToken::START_SLUR ) &&
				     ( slurs_ann[i] != RelLyToken::IN_SLUR ) &&
					 ( slurs_ann[i] != RelLyToken::END_SLUR ) &&
					 //( ties_ann[i] != RelLyToken::START_TIE ) &&
					 ( ties_ann[i] != RelLyToken::CONTINUE_TIE ) &&
					 ( ties_ann[i] != RelLyToken::END_TIE ) ) {
						if ( relLyTokens[0][i].getPitchClass() != lastPC )
							cerr << getLocation() << ": Error: slur missing at notes: " << relLyTokens[0][lastNoteIndex].getToken()
							     << " - " << relLyTokens[0][i].getToken() << endl;
						else
							cerr << getLocation() << ": Error: tie missing at notes: " << relLyTokens[0][lastNoteIndex].getToken()
							     << " - " << relLyTokens[0][i].getToken() << endl;
						res = false;
				}
			}
		}
		if ( i < relLyTokens[0].size() )
			if (relLyTokens[0][i].getIdentity() == RelLyToken::NOTE ) {
				lastPC = relLyTokens[0][i].getPitchClass(); //remember current pitch.
				lastNoteIndex = i; //also remember index of last note
			}

	}

	return res;

}

bool SongLine::checkTextPlacing() const {
	bool res = true;
	string syl = "#";
		
	if ( relLyTokens.size() < 2 ) { return true; } //no text. check not needed
	if ( text_ann.size() < 1 ) { cerr << getLocation() << ": Error: no annotations" << endl; return false; }
			
	//walk through the music annotation
	//if \time or \times, no text should be in the lyrics
	//only check first text line

	for ( int i=0; i<relLyTokens[0].size(); i++ ) {
		if ( relLyTokens[0][i].getIdentity() != RelLyToken::NOTE ) {
			if ( i<text_ann[0].size() )
				if ( text_ann[0][i] == RelLyToken::NO_WORD ) {
					if ( i<relLyTokens[1].size() )
						syl = relLyTokens[1][i].getToken();
					else
						syl = "";
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
	
	int field_width = 17;
		
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
		part = relLyTokens[0][i].getWCEPosition();
		columnize(part, field_width); st = st + part;
		//cout << part;
		
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

bool SongLine::endsWithBarLine() const {
	return (relLyTokens[0].back()).getIdentity() == RelLyToken::BARLINE;
}


bool SongLine::checkTies() const {
	bool res = true;
	
	if ( relLyTokens[0].size() < 2 ) return true; //apparently one note on the line

	//walk trough note sequence. If slur end and previous note has slur start or in slur, and pitches are the same: should be a tie. Unless a slur and a tie end at the same note
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
				if ( relLyTokens[0][p].getPitchClass() == relLyTokens[0][i].getPitchClass() &&
				     relLyTokens[0][p].getAccidental() == relLyTokens[0][i].getAccidental() &&
					 relLyTokens[0][i].getOctaveCorrection() == 0 ) {
					if ( ties_ann[i] != RelLyToken::END_TIE )
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
				
				cerr << getLocation() << ": Warning: textline too short: " << endl;
				cerr << wcelines[0] << endl;
				cerr << wcelines[i] << endl;
				res = false;
			}
		}
	}
	return res;
}
