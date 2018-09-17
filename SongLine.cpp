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
#include<cctype>
#include<algorithm>
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

SongLine::SongLine(vector<string> lines, RationalTime upb, TimeSignature timesig, string clef, int duration, int dots, int octave, char pitchclass, bool initialtriplet, RelLyToken::TieStatus initialTie, RelLyToken::SlurStatus initialSlur, int initialKeysig, int mtempo, string lytempo, int barnumber, bool meterinvisible, bool eachphrasenewstaff, string filename, int phraseno, int numphrases, string recordno, string stropheno, string str_title, int wcelineno, bool instr, RelLyToken::GraceType gt, vector<string> fField) :
																   wcelines(lines),
																   initialUpbeat(upb),
																   finalUpbeat(RationalTime(0,1)),
																   initialTimeSignature(timesig),
																   finalTimeSignature(timesig),
																   initialOctave(octave),
																   finalOctave(4),
																   initialLastPitchClass(pitchclass),
																   finalLastPitchClass(pitchclass),
																   initialDuration(duration),
																   finalDuration(0),
																   initialDots(dots),
																   finalDots(0),
																   initialClef(clef),
																   finalClef(clef),
																   initialBarnumber(barnumber),
																   finalBarnumber(-1),
																   initialTripletStatus(initialtriplet),
																   finalTripletStatus(false),
																   initialKeySignature(initialKeysig),
																   finalKeySignature(initialKeysig),
																   initialTieStatus(initialTie),
																   finalTieStatus(RelLyToken::NO_TIE),
																   initialSlurStatus(initialSlur),
																   finalSlurStatus(RelLyToken::NO_SLUR),
																   midiTempo(mtempo),
																   lyTempo(lytempo),
																   translationMade(false),
																   meterInvisible(meterinvisible),
																   eachPhraseNewStaff(eachphrasenewstaff),
																   fileName(filename),
																   title(str_title),
																   phraseNo(phraseno),
																   numPhrases(numphrases),
																   record(recordno),
																   strophe(stropheno),
																   WCELineNumber(wcelineno),
																   footerField(fField),
																   instrumental(instr),
																   graceType(gt) {
	translate();
}

SongLine::SongLine() : wcelines(vector<string>()),
					   initialUpbeat(RationalTime(0,1)),
					   initialTimeSignature(TimeSignature()),
					   initialClef("treble"),
					   initialKeySignature(0),
					   initialDuration(0),
					   initialDots(0),
					   initialOctave(4),
					   initialLastPitchClass('g'),
					   initialTripletStatus(false),
					   initialTieStatus(RelLyToken::NO_TIE),
					   initialSlurStatus(RelLyToken::NO_SLUR),
					   initialBarnumber(0),
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
					   instrumental(true),
					   graceType(RelLyToken::NOGRACE),
					   footerField(vector<string>()) {
	translate();
}

/*
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
*/

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
	int currentOctave = 0;
	int lastOctave = initialOctave; // we need both current and last octave to compute octaves
	char lastPitchClass = initialLastPitchClass;
	int currentDuration = initialDuration;
	string currentClef = initialClef;
	int currentKeySignature = initialKeySignature;
	finalKeySignature = initialKeySignature;
	int currentDots = initialDots;
	TimeSignature currentTimeSignature = initialTimeSignature;
	int currentBarnumber = initialBarnumber;
	RelLyToken::SlurStatus currentSlurStatus = initialSlurStatus;
	RelLyToken::TieStatus  currentTieStatus = initialTieStatus;
	bool currentTripletStatus = initialTripletStatus;
	//int indexFirstKernNote = -1; //complication: what if phrase starts with grace
	//int indexLastKernNote = -1; //complication: what if phrase ends with aftergrace
	bool newSubPhrase = false;
	int subPhraseNo = 1; // 0 already out in Song.cpp

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

	//cout << "----" << endl;
	//cout << getLocation() << " initialtimeinbar: " << timeInBar.toString() << endl;
	//cout << getLocation() << " initialupbeat: " << initialUpbeat.toString() << endl;


	for( rl_it = (relLyTokens[0]).begin(); rl_it != (relLyTokens[0]).end(); rl_it++ ) {
		//rellytoken = (*rl_it).getToken();

		//to note or not to note
		//in each case the slur and tie annotation have to be updated, for keeping the annotations in sync with the melody.
		id = (*rl_it).getIdentity();
		//cout << (*rl_it).getToken() << " : " << id << endl;
		switch(id) {
			case RelLyToken::NOTE:
			case RelLyToken::CHORD: {

				// new bar? : raise barnumber and write barlines to kern
				// This will only be executed if no explicit bar command was entered (see below).
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
				//triplet (status is set by TIMES_COMMAND below. reset here, if brace is before note, otherwise after creating note)
				if ( currentTripletStatus && (*rl_it).containsClosingBraceBeforeNote() ) currentTripletStatus = false;
				//duration:
				if ( (*rl_it).getDurationBase() != 0 ) {
					currentDuration = (*rl_it).getDurationBase();
					//dots
					currentDots = (*rl_it).getDots();
				}
				//octave
				currentOctave = (*rl_it).computeOctave(lastOctave, lastPitchClass, (*rl_it).getOctaveCorrection(), (*rl_it).getRefPitchClass());
				//tie
				//RelLyToken::TieStatus rt = (*rl_it).getTie();
				if ( currentTieStatus == RelLyToken::START_TIE || currentTieStatus == RelLyToken::CONTINUE_TIE ) {
					if ( (*rl_it).getIdentity() != RelLyToken::CHORD ) {
						if ( (*rl_it).getPitchClass() != lastPitchClass ) {
							cerr << getLocation() << ": Error: tied notes should have same pitch and octave: " << lastPitchClass << " ~ " << (*rl_it).getPitchClass() << endl;
						}
					}
					if ( (*rl_it).getTie() == RelLyToken::START_TIE) {
						currentTieStatus = RelLyToken::CONTINUE_TIE;
					} else {
						currentTieStatus = RelLyToken::END_TIE;
					}
				}
				else if ( currentTieStatus == RelLyToken::END_TIE || currentTieStatus == RelLyToken::NO_TIE ) {
					if ( (*rl_it).getTie() == RelLyToken::START_TIE ) {
						currentTieStatus = RelLyToken::START_TIE;
					} else {
						currentTieStatus = RelLyToken::NO_TIE;
					}
				}
				//slur
				if (currentSlurStatus == RelLyToken::END_SLUR) currentSlurStatus = RelLyToken::NO_SLUR;
				//check nested slurs:
				/*
				if ( (currentSlurStatus == RelLyToken::START_SLUR || currentSlurStatus == RelLyToken::ENDSTART_SLUR || currentSlurStatus == RelLyToken::IN_SLUR)
						&& ((*rl_it).getSlur() == RelLyToken::START_SLUR || (*rl_it).getSlur() == RelLyToken::ENDSTART_SLUR)  ) {
					cerr << getLocation() << ": Error: Nested slurs are not allowed: " << (*rl_it).getToken() <<  endl;
					exit(1);
				}
				*/
				//
				if ( (*rl_it).getSlur() != RelLyToken::NO_SLUR ) //there is slurinfo, just copy
					currentSlurStatus = (*rl_it).getSlur();
				else {
					//so, no slur info
					//determine whether in slur or not, if so, status is IN_SLUR, otherwise leave currentstatus untouched.
					if ( currentSlurStatus == RelLyToken::START_SLUR || currentSlurStatus == RelLyToken::ENDSTART_SLUR || currentSlurStatus == RelLyToken::IN_SLUR)
						currentSlurStatus = RelLyToken::IN_SLUR;
				}

				// make annotation
				slurs_ann.push_back(currentSlurStatus);
				ties_ann.push_back(currentTieStatus);

				/*
				// Glissando end? Then previous note should have glissando start.
				// Glissandi can not cross line breaks (yet?)
				if ( (*rl_it).getGlissandoEnd() ) {
					if ( kernTokens[0].size() > 1 ) { // there must be a previous kern note.
						kernTokens[0][kernTokens[0].size()-1] = kernTokens[0][kernTokens[0].size()-1] + "h";
					}
				}
				*/

				//softbreak
				bool closeSubPhrase = (*rl_it).hasSoftBreak(); //followed by softbreak?

				//create note-token

				//cout << (*rl_it).getToken() << " - " << (*rl_it).getGlissandoBegin() << (*rl_it).getGlissandoEnd() << endl;

				token = (*rl_it).createKernNote(currentOctave,
				                                currentDuration,
												currentDots,
												currentTripletStatus,
												currentSlurStatus,
												currentTieStatus,
												newSubPhrase, //if previous note has softbreak
												closeSubPhrase, //if current note has softbreak
												graceType);

				if (closeSubPhrase) {
					token = token + "}";
				}

				if (newSubPhrase) {
					stringstream ss;
					ss << getPhraseNo()-1 << "." << subPhraseNo;
					kernTokens[0].push_back("!! segment " + ss.str());
					kernTokens[1].push_back("");
					token = "{" + token;
					subPhraseNo++;
				}

				kernTokens[0].push_back(token);
				kernTokens[1].push_back( (*rl_it).getWCEPosition() );

				token = (*rl_it).createAbsLyNote(currentOctave,
				                                 currentDuration,
												 currentDots,
												 currentSlurStatus,
												 currentTieStatus);
				absLyTokens[0].push_back(token);
				// if this was the first note: set indexFirstKernNote
				//if (indexFirstKernNote == -1) indexFirstKernNote = kernTokens[0].size()-1;
				// set indexLastKernNote always. After finishing the line, this is correct
				//indexLastKernNote = kernTokens[0].size()-1;
				//set LastPitchclass
				if ((*rl_it).getPitchClass() != 'r' && (*rl_it).getPitchClass() != 's' ) lastPitchClass = (*rl_it).getRefPitchClass();
				//set LastOctave for next note
				lastOctave = currentOctave;
				//set time
				timeInBar = timeInBar + rationalDuration(currentDuration, currentDots, currentTripletStatus);
				// set tripletstatus to false if closingbrace after note
				RationalTime t = rationalDuration(currentDuration, currentDots, currentTripletStatus);
				//cout << token << " bar:" << currentBarnumber << " " << timeInBar.toString() << " " << t.toString() << endl;
				if ( currentTripletStatus && (*rl_it).containsClosingBraceAfterNote() ) currentTripletStatus = false;
				//set subphrase
				newSubPhrase = false;
				if ( closeSubPhrase ) newSubPhrase = true;

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

			case RelLyToken::CLEF_COMMAND: {
				//cout << (*rl_it).getToken() << ": clef" << endl;
				token = (*rl_it).getToken(); //just copy for ly output
				absLyTokens[0].push_back((*rl_it).getToken());

				//what is new clef?
				currentClef = (*rl_it).getClefType();
				string krnclef = "*clefG2";
				if (currentClef == "bass") krnclef = "*clefF4";

				//write in **kern
				kernTokens[0].push_back(krnclef);
				kernTokens[1].push_back("*");

				//annotation has also to be done here
				ties_ann.push_back(RelLyToken::NO_TIE_INFO);
				slurs_ann.push_back(RelLyToken::NO_SLUR_INFO);
			} break;

			case RelLyToken::KEY_COMMAND: {
				//cout << (*rl_it).getToken() << ": clef" << endl;
				token = (*rl_it).getToken(); //just copy for ly output
				absLyTokens[0].push_back((*rl_it).getToken());

				//what is new key?
				currentKeySignature = translateKeySignature(token);

				string newkey = getKernKey(currentKeySignature);
				string newkeysignature = getKernKeySignature(currentKeySignature);

				//write in **kern
				kernTokens[0].push_back(newkey);
				kernTokens[1].push_back("*");
				kernTokens[0].push_back(newkeysignature);
				kernTokens[1].push_back("*");

				//annotation has also to be done here
				ties_ann.push_back(RelLyToken::NO_TIE_INFO);
				slurs_ann.push_back(RelLyToken::NO_SLUR_INFO);
			} break;

			case RelLyToken::FREETEXT: {
				//insert free tekst as local comment
				kernTokens[0].push_back("! " + (*rl_it).getFreeText());
				kernTokens[1].push_back("!");
				//do slur and tie annotation
				ties_ann.push_back(RelLyToken::NO_TIE_INFO);
				slurs_ann.push_back(RelLyToken::NO_SLUR_INFO);
			} break;

			case RelLyToken::TEXT: {
				cerr << getLocation() << ": Error: Text in melody line: \"" << (*rl_it).getToken() << "\"" << endl;
				exit(1);
			} break;

			case RelLyToken::BARLINE: {
				//cout << (*rl_it).getBarLineType() << endl;

				if ((*rl_it).getBarLineType()==RelLyToken::UNKNOWNBAR) {
					cerr << getLocation() << " Error: unknown bar line type: " << (*rl_it).getToken() << " Ignoring." << endl;
				}


				// new bar? : raise barnumber and write barlines to kern
				if ( !meterInvisible ) {
					if ( timeInBar > currentTimeSignature.getRationalTime() ) {
						cerr << getLocation() << ": Error: bar too long: " << currentBarnumber << endl;
					}
					if ( timeInBar >= currentTimeSignature.getRationalTime() ) { // new bar
						//write in **kern
						stringstream ss;
						currentBarnumber++;
						string barstr = (*rl_it).getKernBarLine(currentBarnumber);
						kernTokens[0].push_back(barstr);
						kernTokens[1].push_back(barstr);
						timeInBar = RationalTime(0,1);
					} else { //bar in middle of measure
						string barstr = (*rl_it).getKernBarLine(0,true);
						kernTokens[0].push_back(barstr);
						kernTokens[1].push_back(barstr);
					}
				}

				if ( meterInvisible ) { //do explicitly indicated barlines
					string barstr = (*rl_it).getKernBarLine(0,true);
					kernTokens[0].push_back(barstr);
					kernTokens[1].push_back(barstr);
				}

				//do slur and tie annotation
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

				//figure out the current octave and duration after the grace notes
				//make songline of gracenotes and ask for finallastpitchclass and final octave

				//cout << "Processing gracenotes: " << (*rl_it).getToken() << endl;

				if (lastPitchClass != 's' && lastPitchClass != 'r') finalLastPitchClass = lastPitchClass;
				string gracestring = (*rl_it).getToken();

				//check for \app { }
				//if app then a slur should be added
				//slur closing has been added while parsing
				//slur opening for the first note will be added by setting currentSlurStatus to STARTNEW
				string::size_type app_pos;
				if ( ( app_pos = gracestring.find("\\app") ) != string::npos ) {
					currentSlurStatus = RelLyToken::START_NEW_SLUR;
					//this will be handeld in SongLine::breakWcelines() !
				}

				string::size_type grpos1, grpos2;
				//remove "\grace{" and "}" or "\app{" or "\vs{" or "\slashedGrace{" or "\afterGrace{"
				if ( ( grpos1 = gracestring.find("{") ) == string::npos ) {
					cerr << getLocation() << ": Error: \"{\" missing in gracenote. This should never happen:" << endl;
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
							      RationalTime(70000,1), //huge upbeat to prevent putting barline at the beginning
								  TimeSignature(80000,1), //unreasonably large measure
								  currentClef,
								  currentDuration,
								  currentDots,
								  lastOctave,
								  lastPitchClass,
								  false, //it makes no sense to have triplet duration values for grace notes.
								  currentTieStatus,
								  currentSlurStatus,
								  currentKeySignature,
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
								  instrumental,
								  (*rl_it).getGraceType(), //grace
								  footerField);

				//grace.printAnnotations();
				//cout << grace.getInitialOctave() << " - " << grace.getFinalOctave() << endl;
				lastOctave = grace.getFinalOctave();
				lastPitchClass = grace.getFinalLastPitchClass();
				currentDuration = grace.getFinalDuration();
				currentDots = grace.getFinalDots();

				currentTieStatus = grace.finalTieStatus;
				currentSlurStatus = grace.finalSlurStatus;

				vector<string> gracekern = grace.getKernLine(false);
				//for (int i=0; i<gracekern.size(); i++)
				//	cout << "Graceline " << i << " : " << gracekern[i] << endl;

				//add to kern
				//issue: where to put a possible barline?
				//in case of aftergrace, not here
				//in other cases: here.
				if ( (*rl_it).getGraceType() != RelLyToken::AFTER ) {
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
				//now write the kern notes
				for (int i=0; i<gracekern.size(); i++) {
					kernTokens[0].push_back(gracekern[i]);
					kernTokens[1].push_back((*rl_it).getWCEPosition());
				}

				//ties_ann.push_back(RelLyToken::NO_TIE_INFO);
				//slurs_ann.push_back(RelLyToken::NO_SLUR_INFO);
				ties_ann.push_back(currentTieStatus);
				slurs_ann.push_back(currentSlurStatus);
			} break;

			default: {
				cerr << getLocation() << ": Error: Unrecognized token: \"" << (*rl_it).getToken() << "\" This should not happen." << endl;
				exit(1);
			} break;
		}
	}

	// TODO: canonical order violated
	// add the { and } phrase markers to the kern melody
	// find out first and last note
	// first and last item in kerntokens not starting with = or * or !
	// NOT for GRACE
	if (graceType == RelLyToken::NOGRACE) {
		int ix_first = 0;
		int ix_last = kernTokens[0].size()-1;
		if (ix_last < 0) {
			cerr << getLocation() << ": Warning: empty phrase." << endl;
		} else {
			bool found = true;
			while ( kernTokens[0][ix_first][0] == '*' || kernTokens[0][ix_first][0] == '=' || kernTokens[0][ix_first][0] == '!' ) {
				ix_first++;
				if (ix_first >= kernTokens[0].size()) {
					cerr << getLocation() << ": Error: No note or rest in phrase to attach { to." << endl;
					found = false;
					break;
				}
			}
			while ( kernTokens[0][ix_last][0] == '*' || kernTokens[0][ix_last][0] == '=' || kernTokens[0][ix_last][0] == '!' ) {
				ix_last--;
				if (ix_last < 0) {
					cerr << getLocation() << ": Error: No note or rest in phrase to attach } to." << endl;
					found = false;
					break;
				}
			}
			if (found) {
				kernTokens[0][ix_first] = "{" + kernTokens[0][ix_first];
				kernTokens[0][ix_last] = kernTokens[0][ix_last] + "}";
			}
		}
	}


	finalOctave = lastOctave;
	finalDuration = currentDuration;
	finalClef = currentClef;
	finalDots = currentDots;
	finalTimeSignature = currentTimeSignature;
	finalTripletStatus = currentTripletStatus;
	finalTieStatus = currentTieStatus;
	finalSlurStatus = currentSlurStatus;
	finalKeySignature = currentKeySignature;
	if (lastPitchClass != 's' && lastPitchClass != 'r') finalLastPitchClass = lastPitchClass;
	if (timeInBar == currentTimeSignature.getRationalTime()) {
		finalUpbeat = RationalTime(0,1);
		finalBarnumber = currentBarnumber+1;
	} else {
		finalUpbeat = currentTimeSignature.getRationalTime() - timeInBar;
		finalBarnumber = currentBarnumber;
	}
	//DEBUG
	//cout << getLocation() << " finaltimeinbar: " << timeInBar.toString() << endl;
	//cout << getLocation() << " finalupbeat: " << finalUpbeat.toString() << endl;

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

		//if ( graceType == RelLyToken::NOGRACE )
		//	cout << "[ " << (*krn_it) << " -- " << relLyTokens[0][relly_index].getToken() << " ]" << endl;

		//if tandem or barline.
		if ( (*krn_it)[0] == '=' || (*krn_it)[0] == '*' ) {
			if ( (*krn_it).find("*") == 0 ) //tandem
				for ( int i = 1; i<numLines; i++ ) {
					kernTokens[2*i].push_back( "*" );
					kernTokens[2*i+1].push_back( "*" );
				}
			else //barline
				for ( int i = 1; i<numLines; i++ ) {
					kernTokens[2*i].push_back( (*krn_it) );
					kernTokens[2*i+1].push_back( (*krn_it) );
				}

			//for everything in relLyTokens[0] not corresponding to **kern data token, increase relly_index
			if ( relly_index < relLyTokens[0].size() ) {
				if ( relLyTokens[0][relly_index].getIdentity() != RelLyToken::NOTE &&
				     relLyTokens[0][relly_index].getIdentity() != RelLyToken::CHORD &&
					 relLyTokens[0][relly_index].getIdentity() != RelLyToken::GRACE &&
					 relLyTokens[0][relly_index].getIdentity() != RelLyToken::FREETEXT ) {
					for (int i = 1; i<numLines; i++ ) {
						absLyTokens[i].push_back("");
						if ( relLyTokens[0][relly_index].getIdentity() == RelLyToken::UNKNOWN )
							text_ann[i-1].push_back( RelLyToken::DONTKNOW );
						else {
							text_ann[i-1].push_back( RelLyToken::NO_WORD );
						}
					}
					relly_index++;
				}
			}
		} else if ( isLocalComment(*krn_it)  ) {
			//cout << "Local comment: " << (*krn_it) << endl;
			for ( int i = 1; i<numLines; i++ ) {
				kernTokens[2*i].push_back( "!" );
				kernTokens[2*i+1].push_back( "!" );
			}
			//annotate corresponding RelLyToken
			if ( relly_index < relLyTokens[0].size() ) {
				if ( relLyTokens[0][relly_index].getIdentity() != RelLyToken::FREETEXT) {
					cerr << getLocation() << ": Warning: No RelLyToken for FREETEXT " << relLyTokens[0][relly_index].getToken() << endl;
				} else {
					for (int i=1; i<numLines; i++) {
						text_ann[i-1].push_back( RelLyToken::NO_WORD);
					}
					relly_index++;
				}
			}
		} else if ( isGlobalComment(*krn_it)  ) {
			//cout << "Global comment: " << (*krn_it) << endl;
			for ( int i = 1; i<numLines; i++ ) {
				kernTokens[2*i].push_back( "" );
				kernTokens[2*i+1].push_back( "" );
			}
			//There should be no corresponding RelLyToken.
		} else if( hasGrace(*krn_it) ) { //grace note in **kern. Do not place a syllable
			for (int i=1; i<numLines; i++) {
				kernTokens[2*i].push_back(".");
				kernTokens[2*i+1].push_back(".");
			}
			//Only increase relly_index if it points to grace. There could be more than one grace note
			if (relLyTokens[0][relly_index].getGraceType() != RelLyToken::NOGRACE) {
				for(int i=1; i<numLines;i++) text_ann[i-1].push_back(RelLyToken::NO_WORD);
				relly_index++;
			}
		} else { // not tandem, not comment, not grace, not barline
			for ( int i = 1; i<numLines; i++ ) {
				if ( relly_index >= relLyTokens[i].size() ) {
					//for ( int knn = 0; knn<kernTokens[2*i].size(); knn++) cout <<  kernTokens[2*i][knn] << endl;
					cerr << getLocation() << ": Warning: Text line too short." << endl;
					kernTokens[2*i].push_back( "." );
					kernTokens[2*i+1].push_back( "." );
					// rest at end of line could be tolerated
					if ( relly_index < relLyTokens[0].size() ) {
						if ( relLyTokens[0][relly_index].getPitchClass() == 'r' || relLyTokens[0][relly_index].getPitchClass() == 's' ) relLyTokens[i].push_back( RelLyToken("", "", 0, 0, RelLyToken::TEXT, false) );
					}
					// any case: annotate
					text_ann[i-1].push_back( RelLyToken::NO_WORD );
				} else {
					if ( relLyTokens[0][relly_index].getIdentity() == RelLyToken::NOTE  ||
						 relLyTokens[0][relly_index].getIdentity() == RelLyToken::CHORD ) {
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

						//for (int kk=0; kk<relLyTokens.size(); kk++)
						//	cout << kk << " " << relLyTokens[kk].size() << endl;

						if ( relLyTokens[0][relly_index].getPitchClass() == 'r' || relLyTokens[0][relly_index].getPitchClass() == 's' ) {
							text_ann[i-1].push_back(RelLyToken::NO_WORD);
						}
						else if ( relLyTokens[i][relly_index].getToken().size() == 0 ) {
							text_ann[i-1].push_back(RelLyToken::NO_WORD);
							cerr << getLocation() << ": Error: Lyric line too short." << endl;
						}
						else {
							text_ann[i-1].push_back(currentTextStatus);

						}
					} else { //Not a NOTE or a CHORD: //TIMES or UNKNOWN or SOMETHING ELSE
						//if UNKNOWN: DONTKNOW
						if ( relLyTokens[0][relly_index].getIdentity() == RelLyToken::UNKNOWN )
							text_ann[i-1].push_back(RelLyToken::DONTKNOW);
						else
							text_ann[i-1].push_back( RelLyToken::NO_WORD );
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
			//Everything that has a rellytoken, but not a kerntoken:
			if ( relLyTokens[0][relly_index].getIdentity() == RelLyToken::TIMES_COMMAND ||
				 relLyTokens[0][relly_index].getIdentity() == RelLyToken::UNKNOWN )
					krn_it--; // Same Token again. DO NOT USE krn_it AFTER THIS.

			relly_index++;
		}
	}
	//tokens left in


	//cout << "Number of text syllables: " <<

	//debug
	/*
	vector<string>::iterator debugit;
	vector<RelLyToken>::iterator debugit2;
	cout << endl;
	for( debugit2 = (relLyTokens[0]).begin(); debugit2 != (relLyTokens[0]).end(); debugit2++ ) {
		cout << (*debugit2).getToken() << '\t'; }
	cout << endl;
	//for( debugit = (absLyTokens[1]).begin(); debugit != (absLyTokens[1]).end(); debugit++ ) {
	//	cout << (*debugit) << '\t'; }
	//cout << endl;
	//for( debugit = (absLyTokens[0]).begin(); debugit != (absLyTokens[0]).end(); debugit++ ) {
	//	cout << (*debugit) << '\t'; }
	//cout << endl;
	//for( debugit = (absLyTokens[1]).begin(); debugit != (absLyTokens[1]).end(); debugit++ ) {
	//	cout << (*debugit) << '\t'; }
	//cout << endl;
	for( debugit = (kernTokens[0]).begin(); debugit != (kernTokens[0]).end(); debugit++ ) {
		cout << (*debugit) << '\t'; }
	cout << endl;
	for( debugit = (kernTokens[1]).begin(); debugit != (kernTokens[1]).end(); debugit++ ) {
		cout << (*debugit) << '\t'; }
	cout << endl;
	vector<RelLyToken::TextStatus>::iterator debugit3;
	for( debugit3 = (text_ann[0]).begin(); debugit3 != (text_ann[0]).end(); debugit3++ ) {
		cout << (*debugit3) << '\t'; }
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
		cout << getNLBIdentifier() << " phrase " << phraseNo << ": " << *it << endl;
}

//offset: python style negative index where to start searching for last note
//eg. offset=-2. Start at rts.back().back()[len-2]
//where len = size of rts.back().back()
//default value -1 (last item)
int SongLine::ixLastNote(vector<vector<RelLyToken> > rts, string warningMessage, int offset ) {
	//find last note
	int ix = rts.back().size() + offset;
	//cout << ix << endl;
	while ( ix >= 0 && (relLyTokens.back())[ix].getIdentity() != RelLyToken::NOTE && (relLyTokens.back())[ix].getIdentity() != RelLyToken::CHORD )
		ix--;
	if ( ix < 0 ) cerr << getLocation() << ": Warning: " << warningMessage << endl;
	return ix;
}

void SongLine::breakWcelines() {

	string line;
	string token;
	vector<RelLyToken> emptyline;
	bool is_music = true; //first line is music line
	bool aftergrace = false; //set to true if the \afterGrace command was in input. Then first braced is taken as grace notes.
	bool ficta = false; //set to true for next note to have ficta.

	//string::size_type pos;
	string ctoken = "";
	vector<string>::iterator it;
	int pos_in_line = 0;
	int line_offset = 0;

	//quick hack to colorize and make visible 's'
	bool colorize = false;
	//bool colorize = true;
	vector<RestPos> rests;
	//////////////////////////////////////////////

//	for( it = wcelines.begin(); it != wcelines.end(); it++) {
	for( size_t wce_ix=0; wce_ix < wcelines.size(); wce_ix++) {
		relLyTokens.push_back(emptyline);
		//line = *it;
		line = wcelines[wce_ix];

		//ask flex
		string flexline = line + " "; //add whitespace at end!!
		istringstream iss(flexline);
		FlexLexer* lexer;

		//cout << flexline << endl;

		string lexline = "";
		bool addOpeningBrace = false; //true if an opening brace has to be added to the next note
	       //Also set to true if the \times command was in input. The { should be re-added to next note!
		   //reason: the { is scanned with the \times command, because otherwise the braced notes are detected as whole group, which
		   //is intended for the afterGrace... UGLY.....
		bool addSlurEnding = false; //true if a slur ending has to be added to the next note
		   //This is used for \app { } grace notes
		   //The slur beginning on the first note of the \app is handled by inspecting initialSlurStatus

		if ( is_music ) {
			pos_in_line = 0;
			lexer = new LilyFlexLexer(&iss);

			int tok = lexer->yylex();
			bool eerste = true;
			while(tok != 0) {

				//cout << lexer->YYText() << " ; pos: " << pos_in_line << " ; tok: " << tok << endl ;

				switch(tok) {
					case -2 : { //whitespace
						//ignore
					} break;
					case -1 : {
					  ctoken = lexer->YYText();
					  if ( ctoken == "." )
						cerr << getLocation() << ": Error: Unrecognized dot. Probably duration without digits: " << ctoken << endl;
					  else
						cerr << getLocation() << ": Error: Unrecognized token: " << ctoken << endl;
					} break;
					case 1 : { //Note (tok==1)
					  ctoken = lexer->YYText();
					  pvktrim(ctoken);

					  if ( addOpeningBrace ) {
						  ctoken = "{ " + ctoken;
						  addOpeningBrace = false;
					  }

					  RelLyToken rlt = RelLyToken(ctoken, getLocation(), WCELineNumber, pos_in_line, RelLyToken::NOTE, false, is_music);

					  //for \app grace notes.
					  if ( initialSlurStatus == RelLyToken::START_NEW_SLUR) {
						  rlt.addSlurBegin();
						  initialSlurStatus = RelLyToken::START_SLUR;
					  }
					  if ( addSlurEnding ) {
						  rlt.addSlurEnd();
						  addSlurEnding = false;
					  }


					  relLyTokens.back().push_back(rlt);

					  if (colorize) {
						  if (rlt.isRest() && rlt.getPitchClass() == 's') {
							  string::size_type colorpos = pos_in_line;
							  string::size_type pitchclasspos = colorpos + rlt.getPosOfPitchClass() + 1;
							  rests.push_back(RestPos(pitchclasspos, colorpos));
						  }
					  }

					  //handle glissando. If note had glissando ending, a glissando start should be added to previous note
					  if ( relLyTokens.back().back().getGlissandoEnd() ) {
						  //find previous note
						  int ix = relLyTokens.back().size() -2; //do not start at this note
						  //cout << ix << endl;
						  while ( ix >= 0 && (relLyTokens.back())[ix].getIdentity() != RelLyToken::NOTE && (relLyTokens.back())[ix].getIdentity() != RelLyToken::CHORD )
							  ix--;
						  if ( ix < 0 ) cerr << getLocation() << ": Warning: start of glissando could not be attached to a note." << endl;
						  else
							  (relLyTokens.back())[ix].setGlissandoBegin();
					  }

					} break;
					case 2 : { //time
						ctoken = lexer->YYText();
						(relLyTokens.back()).push_back(RelLyToken(ctoken, getLocation(), WCELineNumber, pos_in_line, RelLyToken::TIME_COMMAND, is_music));
					} break;
					case 3 : { //times
						ctoken = lexer->YYText();
						(relLyTokens.back()).push_back(RelLyToken(ctoken, getLocation(), WCELineNumber, pos_in_line, RelLyToken::TIMES_COMMAND, is_music));
						addOpeningBrace = true;
					} break;
					case 4 : {
						//clog << getLocation() << ": INSTR: " << lexer->YYText() << endl;
						//ignore
					} break;
					case 5 : { //grace or app or kvs (not afterGrace - see below)
						ctoken = lexer->YYText();
						//if app, a slur ending should be added to next note.
						string::size_type app_pos;
						if ( ( app_pos = ctoken.find("\\app") ) != string::npos ) {
							addSlurEnding = true;
						}
						//put this with "\grace{" and "}" int relLyTokens
						(relLyTokens.back()).push_back(RelLyToken(ctoken, getLocation(), WCELineNumber, pos_in_line, RelLyToken::GRACE, is_music));
					} break;
					case 6 : { //barline
						ctoken = lexer->YYText();
						pvktrim(ctoken);
						(relLyTokens.back()).push_back(RelLyToken(ctoken, getLocation(), WCELineNumber, pos_in_line, RelLyToken::BARLINE, is_music));
					} break;
					case 7 : { //separate tie, should be added to last note
						//cout << "separate tie" << endl;
						int ix = relLyTokens.back().size() -1;
						//cout << ix << endl;
						while ( ix >= 0 && (relLyTokens.back())[ix].getIdentity() != RelLyToken::NOTE && (relLyTokens.back())[ix].getIdentity() != RelLyToken::CHORD )
							ix--;
						if ( ix < 0 ) cerr << getLocation() << ": Warning: tie symbol could not be attached to a note." << endl;
						else
							(relLyTokens.back())[ix].addTie();
					} break;
					case 8 : { //separate closing brace, should be added to last note
						//cout << "separate closing brace" << endl;
						int ix = relLyTokens.back().size() -1;
						//cout << ix << endl;
						while ( ix >= 0 && (relLyTokens.back())[ix].getIdentity() != RelLyToken::NOTE && (relLyTokens.back())[ix].getIdentity() != RelLyToken::CHORD )
							ix--;
						if ( ix < 0 ) cerr << getLocation() << ": Warning: closing brace symbol could not be attached to a note." << endl;
						else
							(relLyTokens.back())[ix].addClosingBrace();
					} break;
					case 9 : { //separate opening brace, should be remembered for next note
						//cout << "separate opening brace" << endl;
						addOpeningBrace = true;
					} break;
					case 11 : { //softbreak
						//clog << "softbreak" << endl;
						//find last note
						int ix = relLyTokens.back().size() -1;
						//cout << ix << endl;
						while ( ix >= 0 && (relLyTokens.back())[ix].getIdentity() != RelLyToken::NOTE && (relLyTokens.back())[ix].getIdentity() != RelLyToken::CHORD )
							ix--;
						if ( ix < 0 ) cerr << getLocation() << ": Warning: softbreak symbol could not be attached to a note." << endl;
						else
							(relLyTokens.back())[ix].setSoftBreak();
					} break;
					case 12 : { //chord
						ctoken = lexer->YYText();
						RelLyToken rlt = RelLyToken(ctoken, getLocation(), WCELineNumber, pos_in_line, RelLyToken::CHORD, false, is_music);
						relLyTokens.back().push_back(rlt);
						//cout << "chord: " << ctoken << endl;
						//cout << "location: " << getLocation() << endl;
					} break;
					case 13 : { // single '('  should be attached to last note
						ctoken = lexer->YYText();
						//find last note
						int ix = relLyTokens.back().size() -1;
						//cout << ix << endl;
						while ( ix >= 0 && (relLyTokens.back())[ix].getIdentity() != RelLyToken::NOTE && (relLyTokens.back())[ix].getIdentity() != RelLyToken::CHORD )
							ix--;
						if ( ix < 0 ) cerr << getLocation() << ": Warning: ( symbol could not be attached to a note." << endl;
						else
							(relLyTokens.back())[ix].addSlurBegin();
					} break;
					case 14 : { // single ')'  should be attached to last note
						ctoken = lexer->YYText();
						//find last note
						int ix = relLyTokens.back().size() -1;
						//cout << ix << endl;
						while ( ix >= 0 && (relLyTokens.back())[ix].getIdentity() != RelLyToken::NOTE && (relLyTokens.back())[ix].getIdentity() != RelLyToken::CHORD )
							ix--;
						if ( ix < 0 ) cerr << getLocation() << ": Warning: ) symbol could not be attached to a note." << endl;
						else
							(relLyTokens.back())[ix].addSlurEnd();
					} break;
					case 15 : { //CLEF change
						ctoken = lexer->YYText();
						(relLyTokens.back()).push_back(RelLyToken(ctoken, getLocation(), WCELineNumber, pos_in_line, RelLyToken::CLEF_COMMAND, is_music));
					} break;
					case 16 : { //afterGrace command
						aftergrace = true;
					} break;
					case 17 : { //Volta
						//ignore for now
					} break;
					case 18 : { //braced (used for afterGrace)
						ctoken = lexer->YYText();
						if (!aftergrace ) {
							cerr << getLocation() << ": Error: braced notes only allowed for grace and triplets: " << ctoken << endl;
							exit(1);
						}
						string aftergr = "\\afterGrace " + ctoken;
						(relLyTokens.back()).push_back(RelLyToken(aftergr, getLocation(), WCELineNumber, pos_in_line, RelLyToken::GRACE, is_music));
						aftergrace = false;
					} break;
					case 19 : { // key change
						ctoken = lexer->YYText();
						(relLyTokens.back()).push_back(RelLyToken(ctoken, getLocation(), WCELineNumber, pos_in_line, RelLyToken::KEY_COMMAND, is_music));
					} break;
					case 20 : { //ficta
						ficta = true;
					} break;
					case 21 : { // trill - should be attached to last note
						int ix = ixLastNote(relLyTokens, "trill could not be attached to last note");
						if (ix>=0) {
							(relLyTokens.back())[ix].addOrnament(RelLyToken::TRILL);
						}
					} break;
					case 22 : { // prall - should be attached to last note
						int ix = ixLastNote(relLyTokens, "prall could not be attached to last note");
						if (ix>=0) {
							(relLyTokens.back())[ix].addOrnament(RelLyToken::PRALL);
						}
					} break;
					case 23 : { // prallprall - should be attached to last note
						int ix = ixLastNote(relLyTokens, "prallprall could not be attached to last note");
						if (ix>=0) {
							(relLyTokens.back())[ix].addOrnament(RelLyToken::PRALLPRALL);
						}
					} break;
					case 24 : { // mordent - should be attached to last note
						int ix = ixLastNote(relLyTokens, "mordent could not be attached to last note");
						if (ix>=0) {
							(relLyTokens.back())[ix].addOrnament(RelLyToken::MORDENT);
						}
					} break;
					case 25 : { // turn - should be attached to last note
						int ix = ixLastNote(relLyTokens, "turn could not be attached to last note");
						if (ix>=0) {
							(relLyTokens.back())[ix].addOrnament(RelLyToken::TURN);
						}
					} break;
					case 26 : { // "//" - should be attached to last note
						int ix = ixLastNote(relLyTokens, "// could not be attached to last note");
						if (ix>=0) {
							(relLyTokens.back())[ix].addOrnament(RelLyToken::DOUBLESLASH);
						}
					} break;
					case 27 : { // -. or staccato - should be attached to last note
						int ix = ixLastNote(relLyTokens, "staccato could not be attached to last note");
						if (ix>=0) {
							(relLyTokens.back())[ix].addArticulation(RelLyToken::STACCATO);
						}
					} break;
					case 28 : { // staccatissimo - should be attached to last note
						int ix = ixLastNote(relLyTokens, "staccatissimo could not be attached to last note");
						if (ix>=0) {
							(relLyTokens.back())[ix].addArticulation(RelLyToken::STACCATISSIMO);
						}
					} break;
					case 29 : { // fermata - should be attached to last note
						int ix = ixLastNote(relLyTokens, "fermata could not be attached to last note");
						if (ix>=0) {
							(relLyTokens.back())[ix].setFermata();
						}
					} break;
					case 30 : { // accent - should be attached to last note
						int ix = ixLastNote(relLyTokens, "accent could not be attached to last note");
						if (ix>=0) {
							(relLyTokens.back())[ix].addArticulation(RelLyToken::ACCENT);
						}
					} break;
					case 31 : { // stopped - should be attached to last note
						int ix = ixLastNote(relLyTokens, "+ could not be attached to last note");
						if (ix>=0) {
							(relLyTokens.back())[ix].addArticulation(RelLyToken::STOPPED);
						}
					} break;
					case 32 : { // tenuto - should be attached to last note
						int ix = ixLastNote(relLyTokens, "tenuto could not be attached to last note");
						if (ix>=0) {
							(relLyTokens.back())[ix].addArticulation(RelLyToken::TENUTO);
						}
					} break;
					case 33 : { // free text - last note should get mark (? in kern). Text will be local comment
						int ix = ixLastNote(relLyTokens, "text could not be attached to last note");
						if (ix>=0) {
							(relLyTokens.back())[ix].setFreeText();
						}
						ctoken = lexer->YYText();
						(relLyTokens.back()).push_back(RelLyToken(ctoken, getLocation(), WCELineNumber, pos_in_line, RelLyToken::FREETEXT, is_music));

					} break;
					default: {
						ctoken = lexer->YYText();
						cerr << getLocation() << ": Error: Unrecognized token: " << ctoken << endl;
					} break;

				}//switch


				//cout << tok << " - " << lexer->YYText() << " - " << pos_in_line << endl;

				pos_in_line += lexer->YYLeng();
				tok = lexer->yylex();

			} //while tok
			delete lexer;

			//now handle colored rests (work backwards to preserve indices):
			for (long int p=rests.size()-1; p >= 0; p--) {
			  wcelines[wce_ix].replace(rests[p].pitchclasspos, 1, "r");
			  wcelines[wce_ix].insert(rests[p].colorpos, "\\once\\override Rest.color = #red ");
			}

		} else { //textline

		  pos_in_line = 0;
		  int current_note_index = 0;

		  lexer = new TextFlexLexer(&iss);

		  int tok = lexer->yylex();
		  bool eerste = true;
		  while(tok != 0){

			//cout << lexer->YYText() << " " << tok << endl ;

			if ( tok == -1 ) {
			  cerr << getLocation() << ": Warning: Unrecognized token: " << lexer->YYText() << endl;
			} else if ( tok == 2 ) { //whitespace.
				//cout << "Whitespace detected: " << lexer->YYLeng() << endl;
			} else {
			  ctoken = lexer->YYText();
			  pvktrim(ctoken);

			  /*
			  cout << "------" << endl;
			  cout << relLyTokens.size() << endl;
			  cout << relLyTokens[0].size() << endl;
			  for (int nn=0; nn<relLyTokens[0].size(); nn++)
				  cout << relLyTokens[0][nn].getToken() << " ";
			  cout << endl;
			  cout << current_note_index << endl;
			  cout << "======" << endl;
			  */

			  //test whether there are too many text tokens
			  if ( current_note_index >= relLyTokens[0].size() ) {
				  cerr << getLocation() << ": Error: too much text: " << ctoken << endl;
				  exit(1);
			  }

			  //if not a (note or chord) in corresponding music, add empty tokens first.
			  while ( (relLyTokens[0][current_note_index].getIdentity() != RelLyToken::NOTE &&
					  relLyTokens[0][current_note_index].getIdentity() != RelLyToken::CHORD) ||
					  relLyTokens[0][current_note_index].isRest() ) {
				(relLyTokens.back()).push_back(RelLyToken("", getLocation(), WCELineNumber+line_offset, pos_in_line, RelLyToken::TEXT, is_music));
			  	current_note_index++;
				if ( current_note_index >= relLyTokens[0].size() ) {
					cerr << getLocation() << ": Error: too much text: " << ctoken << endl;
					exit(1);
				}
			  }
			  //add token to list of tokens
			  (relLyTokens.back()).push_back(RelLyToken(ctoken, getLocation(), WCELineNumber+line_offset, pos_in_line, RelLyToken::TEXT, is_music));
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
		  	(relLyTokens.back()).push_back(RelLyToken("", getLocation(), WCELineNumber+line_offset, pos_in_line, RelLyToken::TEXT, is_music));
		  }

		}
		is_music = false;
		line_offset++;
	}

}


bool SongLine::hasGrace(string krntoken) const {
	if ( krntoken.find_first_of("q") != string::npos || krntoken.find_first_of("Q") != string::npos )
		return true;
	else
		return false;
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

	if ( tok.size() == 0 ) {
		if ( repr == KERN ) return "."; else return "";
	}
	if ( tok == "_" ) {
		if ( repr == KERN ) {
			if ( ts == RelLyToken::END_WORD || ts == RelLyToken::SINGLE_WORD || ts == RelLyToken::END_WORD_CONT || ts == RelLyToken::SINGLE_WORD_CONT ) return "."; // return "|";
			else if ( ts == RelLyToken::IN_WORD || ts == RelLyToken::BEGIN_WORD || ts == RelLyToken::IN_WORD_CONT || ts == RelLyToken::BEGIN_WORD_CONT ) return "."; // return "||";
			else return ".";
		}
		else return "";
	}
	if ( tok.find(" --") != string::npos) tok.erase(tok.find(" --"));

	//if only a dash, print a warning. Should be an underscore.
	if ( tok.find_first_not_of("-") == string::npos ) {
		cerr << getLocation() << ": Warning: only a \"-\" under a note. Should be an \"_\"?" << endl;
	}

	string hyphen="-";
	//string hyphen="";

	//string delimiterA="["; //to keep words that go with single note together
	//string delimiterB="]";
	string delimiterA="";
	string delimiterB="";

	tok = delimiterA + tok + delimiterB;

	switch (ts) {
		case RelLyToken::IN_WORD: {
			if ( repr == KERN ) tok = hyphen + tok + hyphen; //else ok
			if ( repr == TEXT ) tok = tok + hyphen;
			break;
		}

		case RelLyToken::BEGIN_WORD: {
			if ( repr == KERN ) tok = tok + hyphen;
			if ( repr == TEXT ) tok = tok + hyphen;
			break;
		}

		case RelLyToken::END_WORD: {
			if ( repr == KERN ) tok = hyphen + tok;
			else                tok = tok + " ";
			break;
		}

		case RelLyToken::SINGLE_WORD: {
			if ( repr == TEXT ) tok = tok + " ";
			break;
		}
		default: {}; //other possibilities handled above ("_")
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

		//TODO: fix inheritFirstLynoteDuration

		if (lines) {
			cerr << getLocation() << " Warning: inheritFirstLynoteDuration() broken. Don't use single line output." << endl;
		}

		//if (!inheritFirstLynoteDuration( res[0], initialDuration)) {
		//	cerr << getLocation() << " Error: Problem checking duration of first note/rest." << endl;
		//	exit(1);
		//}


		if ( lines ) {
			if ( initialTripletStatus ) {
				if ( res.size() > 0 ) res[0] = "\\times 2/3 { " + res[0];
			}
			if ( finalTripletStatus ) {
				if ( res.size() > 0 ) res[0] = res[0] + "}";
			}
		}

		// add break at end
		// if there is a barline, don't add the empty bar.

		// if not barline is provided at the end of the line, print an appropriate one.
		string bartype = "\"|\"";
		if (!endsWithBarLine() ) {
			if ( phraseNo == numPhrases ) {
				if ( res.size() >0 ) res[0] = res[0] + " \\bar \"|.\"";
			} else {
				if ( finalUpbeat.getNumerator() == 0 ) {
					if ( meterInvisible ) bartype = "\"\"";
					if ( res.size() >0 ) res[0] = res[0] + " \\mBreak \\bar " + bartype;
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
	//string mode;
	//guess most probable key (not optimal)
	switch( initialKeySignature ) {
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
		case 143: key = "fes \\lydian"; break;
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
	else if ( ly_ver == 16 ) res.push_back("\\version\"2.16\"");
	else res.push_back("\\version\"2.18.2\"");

	if ( weblily ) {
		res.push_back("#(append! paper-alist '((\"long\" . (cons (* 210 mm) (* 2000 mm)))))");
		res.push_back("#(set-default-paper-size \"long\")");
		res.push_back("sb = {\\breathe}");
	}
	else
		res.push_back("sb = {\\breathe}");

	if ( weblily ) {
		if (instrumental) {
			res.push_back("mBreak = {\\breathe }");
			res.push_back("bBreak = {\\breathe }"); //see getLyLine()
		} else {
			res.push_back("mBreak = { \\bar \"\" \\break }");
			res.push_back("bBreak = { \\break }"); //see getLyLine()
		}
	} else if ( eachPhraseNewStaff ) {
		res.push_back("mBreak = { \\bar \"\" \\break }");
		res.push_back("bBreak = { \\break }");
	} else {
		res.push_back("mBreak = { }");
		res.push_back("bBreak = { }");
	}

	res.push_back("x = {\\once\\override NoteHead #'style = #'cross }");
	res.push_back("gl=\\glissando");
	res.push_back("app = #(define-music-function (parser location notes) (ly:music?) #{ \\appoggiatura $notes #} )");
	res.push_back("kvs = #(define-music-function (parser location notes) (ly:music?) #{ \\slashedGrace $notes #} )");
	res.push_back("itime = #(define-music-function (parser location timesig) (fraction?) #{ \\once\\override Staff.TimeSignature #'stencil = ##f \\time $timesig #} )");
	//** for instrumental music
	res.push_back("ficta = {\\once\\set suggestAccidentals = ##t}");
	res.push_back("fine = {\\once\\override Score.RehearsalMark #'self-alignment-X = #1 \\mark \\markup {\\italic{Fine}}}");
	res.push_back("dc = {\\once\\override Score.RehearsalMark #'self-alignment-X = #1 \\mark \\markup {\\italic{D.C.}}}");
	res.push_back("dcf = {\\once\\override Score.RehearsalMark #'self-alignment-X = #1 \\mark \\markup {\\italic{D.C. al Fine}}}");
	res.push_back("dcc = {\\once\\override Score.RehearsalMark #'self-alignment-X = #1 \\mark \\markup {\\italic{D.C. al Coda}}}");
	res.push_back("ds = {\\once\\override Score.RehearsalMark #'self-alignment-X = #1 \\mark \\markup {\\italic{D.S.}}}");
	res.push_back("dsf = {\\once\\override Score.RehearsalMark #'self-alignment-X = #1 \\mark \\markup {\\italic{D.S. al Fine}}}");
	res.push_back("dsc = {\\once\\override Score.RehearsalMark #'self-alignment-X = #1 \\mark \\markup {\\italic{D.S. al Coda}}}");
	res.push_back("pv = {\\set Score.repeatCommands = #'((volta \"1\"))}");
	res.push_back("sv = {\\set Score.repeatCommands = #'((volta #f) (volta \"2\"))}");
	res.push_back("tv = {\\set Score.repeatCommands = #'((volta #f) (volta \"3\"))}");
	res.push_back("qv = {\\set Score.repeatCommands = #'((volta #f) (volta \"4\"))}");
	res.push_back("xv = {\\set Score.repeatCommands = #'((volta #f))}");
	//** end for instrumental music
	res.push_back("\\header{ tagline = \"\"");
	string songtitle = "";
	string piece = "";
	//cout << "title: " << title << endl;
	//if ( weblily ) {
	//	if ( !lines ) {
	//		if ( title.size() != 0 && title.compare(0,3,"NLB") ) { //only produce given title if it is not an NLB number. do not construct one
	//			//cout << title << endl;
	//			songtitle = "title = \"" + title + "\"";
	//		}
	//	}
	//}
	//else
	{   if (!weblily) piece = "piece = \"Record " + record + " - Strophe " + strophe;
		if ( lines ) {
			stringstream s;
			s << phraseNo;
			string str_phraseNo = "";
			s >> str_phraseNo;
			if (!weblily) piece = piece + " - Phrase " + str_phraseNo + "\"";
		} else {
			if (!weblily) piece = piece + "\"";
		}
		if ( title.size() != 0 ) { //use provided title if given for 'title' field.
			songtitle = "title = \"" + title + "\"";
		} else {
			if (weblily)
				songtitle = "";
			else
				songtitle = "title = \"[" + this->getNLBIdentifier() + "]\"";
		}
	}
	if ( songtitle.size() != 0 ) {
		res.push_back(songtitle);
	}
	if ( piece.size() != 0 ) {
		res.push_back(piece);
	}
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

	res.push_back("\\clef " + initialClef);

	//tempo
	res.push_back("\\tempo " + lyTempo);
	res.push_back("\\override Score.MetronomeMark #'transparent = ##t");

	//** for instrumental music
	res.push_back("\\override Score.RehearsalMark #'break-visibility = #(vector #t #t #f)");

	return res;
}

vector<string> SongLine::getLyEndSignature(int ly_ver, bool lines, bool weblily) const {
	vector<string> res;

	res.push_back(" }}");
	res.push_back(" \\midi { }"); //tempo is in the score block. This works for ly 2.8, 2.10 and 2.11
	res.push_back(" \\layout {");
	res.push_back("            indent = 0.0\\cm");
	if (lines) {
		res.push_back("           ragged-right = ##f ");
		res.push_back("           ragged-last = ##f ");
	}
	if (meterInvisible) {
		res.push_back("           \\context { \\Staff \\remove \"Time_signature_engraver\" }");
		res.push_back("           \\context { \\Score \\remove \"Bar_number_engraver\" defaultBarType = #\"\" \\override PaperColumn #'keep-inside-line = ##t \\override NonMusicalPaperColumn #'keep-inside-line = ##t }");
	}
	res.push_back("}"); //layout
	res.push_back("}"); //score

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
	// if for web, add url

	if (weblily && !lines) {
		res.push_back("\\markup { \\vspace #0 } \\markup { \\with-color #grey \\fill-line { \\center-column { \\smaller \""+this->getNLBIdentifier()+" - http://www.liederenbank.nl/liedpresentatie.php?zoek="+record+"\" } } }");
	}

	// \markup { \with-color #grey \fill-line { \center-column { \smaller http://www.liederenbank.nl/image.php?recordid=167802 } } }

	return res;
}

vector<string> SongLine::getKernLine(bool lines) const {
	vector<string> res;
	string s;

	/*
	for(int i=0; i<kernTokens.size(); i++)
	{
		cout << "kernTokens["<<i<<"]:" << endl;
		for (int j=0; j<kernTokens[i].size(); j++) {
			cout << kernTokens[i][j] << " ";
		}
		cout << endl;
	}
	*/

	if ( kernTokens.size() == 0 ) return res;

	for(int j=0; j < kernTokens[0].size(); j++) {
		s = "";
		if ( isGlobalComment(kernTokens[0][j]) ) {
			s = kernTokens[0][j];
		} else {
			for (int i=0; i < kernTokens.size(); i = i+2 ) {
				s = s + kernTokens[i][j] + "\t";
			}
			//remove last tab.
			s = s.substr(0,s.size()-1);
		}
		res.push_back(s);
	}

	//add always closing barline to final line
	//otherwise only add barline at end a line of a meterless song.
	//but not for grace notes

	if (graceType == RelLyToken::NOGRACE) {

		if ( phraseNo == numPhrases ) {

			//create it
			s = "";
			for( int i=0; i < kernTokens.size(); i = i+2) s = s + "==|!" + "\t";
			s = s.substr(0,s.size()-1); //remove last tab

			//If there is no barline yet, add the closing barline, else remove the barnumbers from the last barline
			if ( res.back().find("=") == string::npos )
						res.push_back(s);
			else {
				res.back().erase(remove_if(res.back().begin(), res.back().end(), &::isdigit), res.back().end());
			}
		} else {
			s = "";
			if ( ( meterInvisible && lines ) || lines) {
				for( int i=0; i < kernTokens.size(); i = i+2) s = s + "==|!" + "\t";
				s = s.substr(0,s.size()-1); //remove last tab
				res.push_back(s);
			}
		}
	}

	return res;
}

void SongLine::getKeyAndMode(int keysig, string& key, string& mode) const {
	key = "C";
	mode = ""; //major and minor do not have indication
	switch( keysig ) {
		//major 0
		case  7: key = "C#"; break;
		case  6: key = "F#"; break;
		case  5: key = "B" ; break;
		case  4: key = "E" ; break;
		case  3: key = "A" ; break;
		case  2: key = "D" ; break;
		case  1: key = "G" ; break;
		case  0: key = "C" ; break;
		case -1: key = "F" ; break;
		case -2: key = "B-"; break;
		case -3: key = "E-"; break;
		case -4: key = "A-"; break;
		case -5: key = "D-"; break;
		case -6: key = "G-"; break;
		case -7: key = "C-"; break;
		//minor 30
		case 37: key = "a#"; break;
		case 36: key = "d#"; break;
		case 35: key = "g#"; break;
		case 34: key = "c#"; break;
		case 33: key = "f#"; break;
		case 32: key = "b" ; break;
		case 31: key = "e" ; break;
		case 30: key = "a" ; break;
		case 29: key = "d" ; break;
		case 28: key = "g" ; break;
		case 27: key = "c" ; break;
		case 26: key = "f" ; break;
		case 25: key = "b-"; break;
		case 24: key = "e-"; break;
		case 23: key = "a-"; break;
		//ionian 60
		case 67: key = "C#"; mode = "ion"; break; //should
		case 66: key = "F#"; mode = "ion"; break;
		case 65: key = "B" ; mode = "ion"; break;
		case 64: key = "E" ; mode = "ion"; break;
		case 63: key = "A" ; mode = "ion"; break;
		case 62: key = "D" ; mode = "ion"; break;
		case 61: key = "G" ; mode = "ion"; break;
		case 60: key = "C" ; mode = "ion"; break;
		case 59: key = "F" ; mode = "ion"; break;
		case 58: key = "B-"; mode = "ion"; break;
		case 57: key = "E-"; mode = "ion"; break;
		case 56: key = "A-"; mode = "ion"; break;
		case 55: key = "D-"; mode = "ion"; break;
		case 54: key = "G-"; mode = "ion"; break;
		case 53: key = "C-"; mode = "ion"; break;
		//dorian 90
		case 97: key = "d#"; mode = "dor"; break;
		case 96: key = "g#"; mode = "dor"; break;
		case 95: key = "c#"; mode = "dor"; break;
		case 94: key = "f#"; mode = "dor"; break;
		case 93: key = "b" ; mode = "dor"; break;
		case 92: key = "e" ; mode = "dor"; break;
		case 91: key = "a" ; mode = "dor"; break;
		case 90: key = "d" ; mode = "dor"; break;
		case 89: key = "g" ; mode = "dor"; break;
		case 88: key = "c" ; mode = "dor"; break;
		case 87: key = "f" ; mode = "dor"; break;
		case 86: key = "b-"; mode = "dor"; break;
		case 85: key = "e-"; mode = "dor"; break;
		case 84: key = "a-"; mode = "dor"; break;
		case 83: key = "d-"; mode = "dor"; break;
		//phrygian 120
		case 127: key = "e#"; mode = "phr"; break;
		case 126: key = "a#"; mode = "phr"; break;
		case 125: key = "d#"; mode = "phr"; break;
		case 124: key = "g#"; mode = "phr"; break;
		case 123: key = "c#"; mode = "phr"; break;
		case 122: key = "f#"; mode = "phr"; break;
		case 121: key = "b" ; mode = "phr"; break;
		case 120: key = "e" ; mode = "phr"; break;
		case 119: key = "a" ; mode = "phr"; break;
		case 118: key = "d" ; mode = "phr"; break;
		case 117: key = "g" ; mode = "phr"; break;
		case 116: key = "c" ; mode = "phr"; break;
		case 115: key = "f" ; mode = "phr"; break;
		case 114: key = "b-"; mode = "phr"; break;
		case 113: key = "e-"; mode = "phr"; break;
		//lydian 150
		case 157: key = "F#"; mode = "lyd"; break;
		case 156: key = "B" ; mode = "lyd"; break;
		case 155: key = "E" ; mode = "lyd"; break;
		case 154: key = "A" ; mode = "lyd"; break;
		case 153: key = "D" ; mode = "lyd"; break;
		case 152: key = "G" ; mode = "lyd"; break;
		case 151: key = "C" ; mode = "lyd"; break;
		case 150: key = "F" ; mode = "lyd"; break;
		case 149: key = "B-"; mode = "lyd"; break;
		case 148: key = "E-"; mode = "lyd"; break;
		case 147: key = "A-"; mode = "lyd"; break;
		case 146: key = "D-"; mode = "lyd"; break;
		case 145: key = "G-"; mode = "lyd"; break;
		case 144: key = "C-"; mode = "lyd"; break;
		case 143: key = "F-"; mode = "lyd"; break;
		//mixolydian 180
		case 187: key = "G#"; mode = "mix"; break;
		case 186: key = "C#"; mode = "mix"; break;
		case 185: key = "F#"; mode = "mix"; break;
		case 184: key = "B" ; mode = "mix"; break;
		case 183: key = "E" ; mode = "mix"; break;
		case 182: key = "A" ; mode = "mix"; break;
		case 181: key = "D" ; mode = "mix"; break;
		case 180: key = "G" ; mode = "mix"; break;
		case 179: key = "C" ; mode = "mix"; break;
		case 178: key = "F" ; mode = "mix"; break;
		case 177: key = "B-"; mode = "mix"; break;
		case 176: key = "E-"; mode = "mix"; break;
		case 175: key = "A-"; mode = "mix"; break;
		case 174: key = "D-"; mode = "mix"; break;
		case 173: key = "G-"; mode = "mix"; break;
		//aeolian 210
		case 217: key = "a#"; mode = "aeo"; break;
		case 216: key = "d#"; mode = "aeo"; break;
		case 215: key = "g#"; mode = "aeo"; break;
		case 214: key = "c#"; mode = "aeo"; break;
		case 213: key = "f#"; mode = "aeo"; break;
		case 212: key = "b" ; mode = "aeo"; break;
		case 211: key = "e" ; mode = "aeo"; break;
		case 210: key = "a" ; mode = "aeo"; break;
		case 209: key = "d" ; mode = "aeo"; break;
		case 208: key = "g" ; mode = "aeo"; break;
		case 207: key = "c" ; mode = "aeo"; break;
		case 206: key = "f" ; mode = "aeo"; break;
		case 205: key = "b-"; mode = "aeo"; break;
		case 204: key = "e-"; mode = "aeo"; break;
		case 203: key = "a-"; mode = "aeo"; break;
		//locrian 240
		case 247: key = "b#"; mode = "loc"; break;
		case 246: key = "e#"; mode = "loc"; break;
		case 245: key = "a#"; mode = "loc"; break;
		case 244: key = "d#"; mode = "loc"; break;
		case 243: key = "g#"; mode = "loc"; break;
		case 242: key = "c#"; mode = "loc"; break;
		case 241: key = "f#"; mode = "loc"; break;
		case 240: key = "b" ; mode = "loc"; break;
		case 239: key = "e" ; mode = "loc"; break;
		case 238: key = "a" ; mode = "loc"; break;
		case 237: key = "d" ; mode = "loc"; break;
		case 236: key = "g" ; mode = "loc"; break;
		case 235: key = "c" ; mode = "loc"; break;
		case 234: key = "f" ; mode = "loc"; break;
		case 233: key = "b-"; mode = "loc"; break;
	}
}

string SongLine::getKernKeySignature(int keysig) const {
	int tempkey = keysig;
	tempkey = ((tempkey+15)%30)-15;

	string keys = "";
	if (tempkey < 0) keys = "b-e-a-d-g-c-f-";
	if (tempkey > 0) keys = "f#c#g#d#a#e#b#";

	int ks = abs(tempkey);
	return "*k[" + keys.substr(0,2*ks) + "]";
}

string SongLine::getKernKey(int keysig) const {
	string key, mode;
	getKeyAndMode(keysig, key, mode);
	return "*" + key + ":" + mode;
}

vector<string> SongLine::getKernBeginSignature(bool lines, bool meterinvisible) const {

	vector<string> res;
	string s;
	if ( kernTokens.size() == 0 ) return res;

	//first print some comments
	res.push_back("!! Published by the Meertens Institute (http://www.meertens.knaw.nl)");
	res.push_back("!! Part of the Database of Dutch Songs (NLB) (http://www.liederenbank.nl)");
	res.push_back("!! Part of the Meertens Tune Collections (MTC) (http://www.liederenbank.nl/mtc)");
	res.push_back(string("!! NLB identifier: " + getNLBIdentifier()));
	res.push_back(string("!! NLB record number: " + record));
	res.push_back(string("!! NLB strophe/voice number: " + strophe));
	res.push_back(string("!!!OTL: "+title));


	//identify spine
	s = "";
	std::string staffids = "";
	for(int i=0; i < kernTokens.size() / 2; i++ ) {
		if (i == 0 )
			s = s + "**kern" + "\t";
		else
			s = s + "**text" + "\t";
		staffids = staffids + "*staff1" + "\t";
	}
	//remove last tab.
	s = s.substr(0,s.size()-1);
	staffids = staffids.substr(0,staffids.size()-1);
	res.push_back(s);
	res.push_back(staffids);

	//clef
	s = "*clefG2";
	if (initialClef == "bass") s = "*clefF4";
	for(int i=0; i < kernTokens.size() / 2; i++ ) {
		if ( i == 0 ) s = s + "\t"; else s = s + "*\t";
	}
	s = s.substr(0,s.size()-1);
	res.push_back(s);

	//time signature
	s = "";
	if (meterinvisible) s = "!!";
	for(int i=0; i < kernTokens.size() / 2; i++ ) {
		if ( i == 0 ) s = s + initialTimeSignature.getKernTimeSignature() + "\t"; else s = s + "*\t";
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
		if ( i == 0 ) s = s + "*MM" + midiTempo_str + "\t"; else s = s + "*\t";
	}
	s = s.substr(0,s.size()-1);
	res.push_back(s);

	//key signature
	s = "";
	for(int i=0; i < kernTokens.size() / 2; i++ ) {
		if ( i == 0 ) s = s + getKernKeySignature(initialKeySignature) + "\t"; else s = s + "*\t";
	}
	s = s.substr(0,s.size()-1);
		res.push_back(s);

	//key (if major or minor or mode)
	////!!!!!!!! FOR NOW: do not export mode. music21 cannot parse!
	//8 april 2014: do it anyway for MTC, anticipating music21 te be able to parse this
	//mode = "";
	s = "";
	for( int i=0; i< kernTokens.size() / 2; i++ ) {
		if ( i == 0) s = s + getKernKey(initialKeySignature) + "\t"; else s = s + "*\t";
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
		s = s + "*-" + "\t";
	}
	s = s.substr(0,s.size()-1);
	res.push_back(s);

	//do footer
	vector<string> footer = formatFooterField(footerField);
	for(int i=0; i<footer.size(); i++) {
		res.push_back("!! " + footer[i]);
	}
	res.push_back("!!");

	//comment
	res.push_back("!! generated by wce2krn " + version + " (released on " + releasedate + ")");

	return res;
}

string SongLine::upbeatToString(RationalTime t) const {
	string res;

	if (initialUpbeat == 0 ) return "";
	//if (meterInvisible) return ""; //better just put the upbeat in the ly

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

	cerr << "SongLine::inheritFirstLynoteDuration() is broken. Do not use." << endl;

	string::size_type pos = 0;
	stringstream ss;
	string strduration;

	int index = 0;
	if (relLyTokens.size() > 0) {

		while ( index < relLyTokens[0].size() )
			if ( relLyTokens[0][index].getIdentity() != RelLyToken::NOTE &&
				 relLyTokens[0][index].getIdentity() != RelLyToken::CHORD )
				index++;
			else
				break;
	}
	else {
		cerr << getLocation() << " Error: No notes in phrase." << endl;
		exit(1);
	}

	//index passed end?
	if ( index >= relLyTokens[0].size() ) {
		cerr << getLocation() << " Error: No first note in phrase: " << lyline << endl;
		exit(1);
	}

	//firstnote has duration?
	if ( relLyTokens[0][index].getDurationBase() != 0 ) return true;


	//find index of end of first note
	//while ( index > 0 ) {
	//	pos = lyline.find('\t');
	//	index--;
	//}

	// THIS IS DIFFERENT FOR CHORDS AND NOTES

	pos = relLyTokens[0][index].getWCE_Pos(); //initialisation of pos: start of lily line

	if ( relLyTokens[0][index].getIdentity() == RelLyToken::NOTE ) {

		//skip '\ficta', \gl, \bar ETC

		//TODO

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

		RelLyToken::Accidental ac = relLyTokens[0][index].getAccidental();

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
			default: {};
		}

		while ( pos < lyline.size() )
			if ( lyline.at(pos) == ',' || lyline.at(pos) == '\'' )
				pos++;
			else
				break;

		//here we should insert the duration

	} else { //chords: easier. right after closing >
		if ( (pos = lyline.find_first_of(">", pos)) == string::npos) return false;
		pos++;
	}

	//create the duration
	ss << initialDuration;
	for(int i=0; i<initialDots; i++) ss << '.';
	ss >> strduration;

	lyline.insert(pos, strduration);

	//cout << lyline << endl;

	return true;
}

string SongLine::getLyricsLine( int line ) const {
	if (lyricsLines.size() > line )
		return lyricsLines[line];
	else {
		cerr << getLocation() << ": Warning: No lyrics available for verse " << line << endl;
		return "";
	}
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
			if ( relLyTokens[0][i].getIdentity() == RelLyToken::NOTE || relLyTokens[0][i].getIdentity() == RelLyToken::CHORD ) {
				if ( ( slurs_ann[i] != RelLyToken::IN_SLUR ) &&
					 ( slurs_ann[i] != RelLyToken::END_SLUR && slurs_ann[i] != RelLyToken::ENDSTART_SLUR) &&
					 ( ties_ann[i] != RelLyToken::CONTINUE_TIE ) &&
					 ( ties_ann[i] != RelLyToken::END_TIE ) ) {
							cerr << getLocation() << ": Warning: slur or tie missing at melisma: " << relLyTokens[0][lastNoteIndex].getToken()
							     << " - " << relLyTokens[0][i].getToken() << endl;
						res = false;
				}
			}
		}
		if ( i < relLyTokens[0].size() )
			if (relLyTokens[0][i].getIdentity() == RelLyToken::NOTE ) {
				if(!relLyTokens[0][i].isRest()) {
					lastPC = relLyTokens[0][i].getPitchClass(); //remember current pitch.
					lastNoteIndex = i; //also remember index of last note
				}
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
		//part = (relLyTokens[0][i].getToken() + "*");
		part = (relLyTokens[0][i].getToken());
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

bool SongLine::isLocalComment(string krntoken) const {
	if (krntoken.size() >= 2) {
		if (krntoken[0] == '!' && krntoken[1] != '!' )
			return true;
	}
	return false;
}

bool SongLine::isGlobalComment(string krntoken) const {
	if (krntoken.size() >= 2) {
		if (krntoken[0] == '!' && krntoken[1] == '!' )
			return true;
	}
	return false;
}

string SongLine::getNLBIdentifier(bool escape_underscore) const {
	string recid = record;
	string strid = strophe;
	while ( recid.size() < 6 ) recid = "0"+recid;
	while ( strid.size() < 2 ) strid = "0"+strid;
	string underscore = "_";
	if (escape_underscore) underscore = "\\_";
	return string("NLB"+recid+underscore+strid);
}

bool SongLine::checkTies() const {
	bool res = true;

	if ( relLyTokens[0].size() < 2 ) return true; //apparently one note on the line

	//walk trough note sequence. If slur end and previous note has slur start XXor in slurXX, and pitches are the same: should be a tie. Unless a slur and a tie end at the same note
	//cout << 0 << " : " << relLyTokens[0][0].getToken() << endl;
	//cout << 0 << " : " << RelLyToken::printSlurStatus(slurs_ann[0]) << endl;
	for( int i=1; i<relLyTokens[0].size(); i++ ) { //starting from the second token
		//cout << i << " : " << relLyTokens[0][i].getToken() << endl;
		//cout << i << " : " << RelLyToken::printSlurStatus(slurs_ann[i]) << endl;
		if ( relLyTokens[0][i].getIdentity() != RelLyToken::NOTE && relLyTokens[0][i].getIdentity() != RelLyToken::CHORD ) continue; //only notes
		if ( slurs_ann[i] == RelLyToken::END_SLUR || slurs_ann[i] == RelLyToken::ENDSTART_SLUR ) {

			//cout << getLocation() << ": Info: slur ends at (" << i << "): " << relLyTokens[0][i].getToken() << endl;

			//search previous NOTE

			//check whether in slur:
			if ( slurs_ann[i-1] == RelLyToken::NO_SLUR || slurs_ann[i-1] == RelLyToken::END_SLUR) {
				cerr << getLocation() << ": Error: slur ends at note: " << relLyTokens[0][i].getToken() << ", but no start" << endl;
				continue; //no previous note
			}

			int p = i-1;

			while ( p>=0 && (slurs_ann[p] != RelLyToken::START_SLUR && slurs_ann[p] != RelLyToken::ENDSTART_SLUR) && slurs_ann[p] != RelLyToken::IN_SLUR ) {
				p--;
				//cout << i << " " << p << endl;
			}

			if ( p == -1 ) {
				cerr << getLocation() << ": Error: slur ends at note: " << relLyTokens[0][i].getToken() << ", but no start" << endl;
				continue; //no previous note
			}
			//now p has index of previous note
			//
			if ( (slurs_ann[p] == RelLyToken::START_SLUR || slurs_ann[p] == RelLyToken::ENDSTART_SLUR) )
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
