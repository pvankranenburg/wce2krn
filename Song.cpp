/*
 *  ContentSong.cpp
 *  wce2krn
 *
 *  Created by Peter Van Kranenburg on 10/27/06.
 *  Copyright 2006 Peter Van Kranenburg. All rights reserved.
 *
 */

#include "Song.h"
#include "SongLine.h"
#include "pvkutilities.h"
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
using namespace std;

Song::Song(string inputfilename, bool weblilypond, string filename_titles) : wcefile(inputfilename, filename_titles), weblily(weblilypond) {

	int i; //index

	//extract the lilypond lines
	vector<string> wcelines = wcefile.getContents();

	//footnotes?
	//vector<string> footnotelines = wcefile.getFooterField();
	//for (int i=0; i<footnotelines.size(); i++) {
	//	cout << footnotelines[i] << endl;
	//}

	//find out number of datalines (spines)
	//find out number of lines in song
	vector<bool> lineprofile;
	vector<string>::iterator str_it;
	vector<string> singleline;
	bool emptyline = false;
	int phraseno = 1;
	int numberOfPhrases = 0;
	SongLine sl;

	for( str_it=wcelines.begin(); str_it!=wcelines.end(); str_it++) {
		emptyline = false;
		if ((*str_it).empty()) emptyline = true;
		if ((*str_it).find_first_not_of(" \t") == string::npos ) emptyline = true;
		if ( emptyline ) lineprofile.push_back(false); else lineprofile.push_back(true);
	}
	//nu staan in lineprofile de dataregels op true

	//compute number of lines
	//is there at least 1 phrase?
	for( int i = 0; i < lineprofile.size(); i++ ) {
		if ( lineprofile[i] ) numberOfPhrases = 1;
	}
	//but if the first line is empty, the counter should be set to 0

	if ( lineprofile.size() == 0 ) { cout << inputfilename << ": Error: No lines in score field. Is this a WCE file?" << endl; exit(1); }

	if ( !lineprofile[0] ) numberOfPhrases = 0;

	// now count the number of false -> true
	for( int i = 0; i < lineprofile.size(); i++ ) {
		if ( i < lineprofile.size()-1 ) {
			if ( !lineprofile[i] && lineprofile[i+1] )
				numberOfPhrases++;
		}
	}

	if ( lineprofile.size() == 0 ) {
		cerr << "Error: empty file." << endl;
		exit (1);
	}

	//instrumental?
	bool instr = true; //by default assume instr
	//if anywhere in the profile there are two adjacent true, then not instrumental
	bool prev = false;
	for (i = 0; i < lineprofile.size(); i++) {
		//cout << i << " - " << lineprofile[i] << endl;
		if ( prev && lineprofile[i] ) instr = false;
		prev = lineprofile[i];
	}
	//cout << "Instrumental: " << instr << endl;

	//determine maximum number of input lines per phrase
	int lines_per_phrase = 0;
	int max_lines_per_phrase = 0;
	for (i=0; i<lineprofile.size(); i++) {
		if ( lineprofile[i] ) {
			lines_per_phrase++;
			if ( lines_per_phrase > max_lines_per_phrase ) max_lines_per_phrase = lines_per_phrase;
		} else {
			lines_per_phrase = 0;
		}
	}
	//last one (if no empty phrases follow:
	if ( lines_per_phrase > max_lines_per_phrase ) max_lines_per_phrase = lines_per_phrase;


	for( i = 0; i <= lineprofile.size(); i++ ) { //doorloop het profile tot size() om ook laatste songline toe te voegen als er geen lege regel volgt
		//cout << "i: " << i << endl;
		//cout << wcelines[i] << endl;
		if (lineprofile[i] && i<lineprofile.size()) { // dataline
			singleline.push_back(wcelines[i]);
			//cout << "SINGLELINE: " << singleline[singleline.size()-1] << endl; // is cleared after adding to songLines
		} else { //empty line -> this means that the line that has just been passed can be added to songLines
			if( !singleline.empty() ) { //don't add empty lines to songLines
				int diff = max_lines_per_phrase - singleline.size();
				while ( diff > 0 ) { //probalby Not enough text lines. Add
					singleline.push_back("");
					diff = max_lines_per_phrase - singleline.size();
					cerr << getLocation() << ": Warning: probably a lyric line missing. Adding an empty line." << endl;
				}
				if( songLines.empty() ) { //first line of song
					int initialBarnumber = 0;
					if ( translateUpbeat(wcefile.getUpbeat()) == RationalTime(0,1) ) initialBarnumber = 1;
					//cout << wcefile.getFirstNoteRelativeTo() << " " << wcefile.getFirstNoteRelativeToPitchClass() << endl;
					//cout << "INITIAL CLEF: " << wcefile.getInitialClef() << endl;
					//if (singleline.size() >0 ) cout << singleline[0] << endl;
					songLines.push_back(SongLine(singleline,
							      translateUpbeat(wcefile.getUpbeat()),
								  translateTimeSignature(wcefile.getTimeSignature()),
								  wcefile.getInitialClef(),
								  4,
								  0,
								  wcefile.getFirstNoteRelativeToOctave(),
								  wcefile.getFirstNoteRelativeToPitchClass(),
								  false,
								  RelLyToken::NO_TIE,
								  RelLyToken::NO_SLUR,
								  translateKeySignature(wcefile.getKey()),
								  translateMidiTempo(wcefile.getMidiTempo()),
								  wcefile.getMidiTempo(),
								  initialBarnumber,
								  wcefile.getMeterInvisible(),
								  wcefile.getEachPhraseNewStaff(),
								  wcefile.getFilename(),
								  phraseno,
								  numberOfPhrases,
								  wcefile.getRecord(),
								  wcefile.getStrophe(),
								  wcefile.getTitle(),
								  i-singleline.size(),
								  instr,
								  RelLyToken::NOGRACE, //not grace
								  wcefile.getFooterField()));
					singleline.clear();
					phraseno++;
					//songLines.push_back(sl);
				} else { //not first line of song
					songLines.push_back(SongLine(singleline,
							      (songLines.back()).getFinalUpbeat(), //from previous line
								  (songLines.back()).getFinalTimeSignature(),
								  (songLines.back()).getFinalClef(),
								  (songLines.back()).getFinalDuration(),
								  (songLines.back()).getFinalDots(),
								  (songLines.back()).getFinalOctave(),
								  (songLines.back()).getFinalLastPitchClass(),
								  (songLines.back()).getFinalTripletStatus(),
								  (songLines.back()).getFinalTieStatus(),
								  (songLines.back()).getFinalSlurStatus(),
								  (songLines.back()).getFinalKeySignature(),
								  (songLines.back()).getMidiTempo(),
								  (songLines.back()).getLyTempo(),
								  (songLines.back()).getFinalBarnumber(),
								  (songLines.back()).getMeterInvisible(),
								  (songLines.back()).getEachPhraseNewStaff(),
								  wcefile.getFilename(),
								  phraseno,
								  numberOfPhrases,
								  wcefile.getRecord(),
								  wcefile.getStrophe(),
								  wcefile.getTitle(),
								  i-singleline.size(),
								  instr,
								  RelLyToken::NOGRACE, //not grace
								  wcefile.getFooterField()));
					//songLines.push_back(sl);
					singleline.clear();
					phraseno++;
				}
			}
		}
	}

	//check
	RelLyToken::TieStatus ts = (songLines.back()).getFinalTieStatus();
	RelLyToken::SlurStatus ss = (songLines.back()).getFinalSlurStatus();

	if (ss == RelLyToken::START_SLUR ||
		ss == RelLyToken::ENDSTART_SLUR ||
		ss == RelLyToken::IN_SLUR ||
		ss == RelLyToken::START_NEW_SLUR  )
		cerr << getLocation() << ": Warning: unterminated slur." << endl;

	if (ts == RelLyToken::START_TIE ||
		ts == RelLyToken::CONTINUE_TIE )
		cerr << getLocation() << ": Warning: unterminated tie." << endl;

	//cout << "HIER" << endl;

	vector<SongLine>::iterator it_sl;
	//test
	//for(it_sl=songLines.begin(); it_sl != songLines.end(); it_sl++) {
	//	cout << "songline: " << endl;
	//	(*it_sl).writeToStdout();
	//}

	//test if each songline has same number of lines (spines)
	//vector<SongLine>::iterator it_sl;
	for(it_sl=songLines.begin()+1; it_sl != songLines.end(); it_sl++) {
		if ( (*it_sl).getWceLines().size() != (*(it_sl-1)).getWceLines().size() ) {
			cerr << getLocation() << ": Error: all songlines should have the same number of textlines." << endl;

			//exit(1);
		}
	}

	//cout << wcefile.getRecord() << " " << wcefile.getStrophe() << " " << songLines.size() << " LINES" << endl;

}


TimeSignature Song::translateTimeSignature(string lyTimeSignature) const {
	return TimeSignature(lyTimeSignature);
}

RationalTime Song::translateUpbeat(string lyUpbeat) const {
	pvktrim(lyUpbeat);
	if ( lyUpbeat.size() == 0 ) return RationalTime(0,1);
	int duration = 0;
	int amount = 1;
	bool dot = false;
	string::size_type pos;
	if( (pos = lyUpbeat.find("*")) != string::npos ) { //number of notes
		amount = atoi( (lyUpbeat.substr(pos+1)).c_str() );
		lyUpbeat.erase(pos);
	}
	if ( (pos = lyUpbeat.find("."))  != string::npos) {
		dot = true;
		lyUpbeat.erase(pos);
	}
	duration = atoi( lyUpbeat.c_str() );
	if (dot) {
		amount = amount * 3;
		duration = duration * 2;
	}
	return RationalTime(amount,duration);
}

void Song::writeToDisk(string basename_full, SongLine::Representation repr, bool lines, int ly_ver) const {
	vector<string> part;
	vector<string>::iterator part_it;
	vector<SongLine>::const_iterator si;

	//to remember the textlines for lilypond output
	vector<string> textlines;
	vector<string>::iterator text_it;

	int stanzas = 0;

	int line = 0;
	stringstream ss;
	string s, basename, path;
	string ext = ".ly";
	string::size_type pos;

	bool stdoutput = false;
	if ( basename_full == "-" ) stdoutput = true;

	if ( repr == SongLine::KERN ) ext = ".krn";

	if( ( pos = basename_full.find_last_of("/") ) != string::npos ) {
		basename = basename_full;
		basename = basename.erase(0,pos+1);
		path = basename_full.erase(pos+1);
	} else {
		basename = basename_full;
		path = "";
	}

	string outname = path + basename + ext;
	ofstream out;

	if (!lines && repr != SongLine::TEXT && !stdoutput) {
		out.open(outname.c_str());
		clog << "Writing " << outname << endl;
	}


	if (songLines.size() > 0 && repr != SongLine::TEXT) {
		//songlines
		for ( si = songLines.begin(); si != songLines.end(); si++ ) {
			//preamble

			//cout << "processing " << (*si).getLocation() << endl;



			if (lines || si == songLines.begin()) {
				if ( lines && !stdoutput) {
					if (out.is_open()) out.close();
					ss.clear();
					ss << path << line << "-" << basename << ext;
					ss >> s;
					out.open(s.c_str());
					clog << "Writing " << s << endl;
				}
				switch(repr) {
					case SongLine::KERN: part = si->getKernBeginSignature(lines, wcefile.getMeterInvisible()); break;
					case SongLine::ABSLY: part = si->getLyBeginSignature(true, lines, weblily, ly_ver); break;
					case SongLine::RELLY: part = si->getLyBeginSignature(false, lines, weblily, ly_ver); break;
					case SongLine::TEXT: {}
				}
				for ( part_it = part.begin(); part_it != part.end(); part_it++ )
					if (stdoutput) cout << *part_it << endl; else out << *part_it << endl;
			}

			switch (repr) {
				case SongLine::KERN:

					part = si->getKernLine(lines);

					if (stdoutput) {
						if (!lines) cout << "!! segment " << line << ".0" << endl;
					}
					else {
						if (!lines) out << "!! segment " << line << ".0" << endl;
					}

					for ( part_it = part.begin(); part_it != part.end(); part_it++ )
						if (stdoutput) cout << *part_it << endl;
						else out << *part_it << endl;

					break;

				case SongLine::RELLY:

					part = si->getLyLine(false, lines, ly_ver);

					if ( part.size() == 0 ) { cerr << "Empty line!" << endl; exit(0); }

					//output lilypond line. first line is music, other lines are txt.
					if (stdoutput) cout << part[0] << endl; else out << part[0] << endl;

					//save text into textlines
					stanzas = songLines.begin()->getNumberOfLines()-1; //number of text lines. Assume melody is in line 0, rest is text
					for ( int l = 1; l <= stanzas; l++ ) {
						if ( l <= part.size() ) textlines.push_back(part[l]); else textlines.push_back("");
					}

					//if lines or at end of song, output textblocks: output everything in textlines
					if (lines || si == (songLines.end()-1)) {

						for ( int s = 0; s < stanzas; s++) {

							if (stdoutput) cout << "} \\addlyrics {" << endl; else out << "} \\addlyrics {" << endl;

							int ix = s;
							while ( ix < textlines.size() ) {

								if (stdoutput) cout << textlines[ix] << endl; else out << textlines[ix] << endl;
								ix = ix + stanzas;
							}
						}

						textlines.clear();

					}

					break;

				case SongLine::ABSLY:

					part = si->getLyLine(true, lines, ly_ver);

					break;

				case SongLine::TEXT:
					{};

			}


			//in case of lilypond: output text lines.
			if ( repr == SongLine::ABSLY || repr == SongLine::RELLY ) {

			}

			//postamble
			if (lines || si == (songLines.end()-1) ) {
				switch(repr) {
					case SongLine::KERN: part = si->getKernEndSignature(); break;
					case SongLine::ABSLY:
					case SongLine::RELLY: part = si->getLyEndSignature(ly_ver, lines, weblily); break;
					case SongLine::TEXT: {};
				}
				for ( part_it = part.begin(); part_it != part.end(); part_it++ )
					if (stdoutput) cout << *part_it << endl; else out << *part_it << endl;
			}
		line++;
		}

	} else { //output lyrics

		if (out.is_open()) out.close();
		ss.clear();
		ss << path << basename << ".txt";
		ss >> s;
		if (!stdoutput) {
			out.open(s.c_str());
			clog << "Writing " << s << endl;
		}

		int stanzas = songLines.begin()->getNumberOfLines()-1; //number of text lines. Assume melody is in line 0, rest is text
		for ( int s = 1; s < stanzas+1; s++ ) {
			for ( si = songLines.begin(); si != songLines.end(); si++ ) {
				if (stdoutput) cout << si->getLyricsLine(s-1) << endl; else out << si->getLyricsLine(s-1) << endl;
			}
			if (stdoutput) cout << endl; else out << endl;
		}
		if (out.is_open()) out.close();
	}


	//close

	if ( out.is_open() ) out.close();

}



int Song::translateMidiTempo(string lymtempo) const {
	int res = 120;

	pvktrim(lymtempo);
	string::size_type is_pos;
	if ( ( is_pos = lymtempo.find("=") ) == string::npos ) {
		cerr << getLocation() << ": Warning: Bad midiTempo; '=' missing: " << lymtempo << endl;
		return 120;
	}

	//find number of dots before the '='
	int dots = 0;
	string::size_type dpos;
	string lt = lymtempo;
	while( (dpos = lt.find(".")) != string::npos && dpos + dots <= is_pos ) { dots++; lt.erase(dpos,1); }

	int duration = convertToInt(lymtempo.substr(0,is_pos - dots));
	int tempo = convertToInt(lymtempo.substr(is_pos+1));

	if ( dots > 1 ) {
		cerr << getLocation() << ": Warning: only one dot allowed in tempo: " << lymtempo << endl;
		dots = 1;
	}

	float factor = duration / 4.0;
	if ( dots == 1 ) factor = factor * 2 / 3;
	res = (int)(tempo / factor);

	return res;
}

string Song::getLocation() const {
	string fn = wcefile.getFilename();
	string::size_type pos;
	if ( (pos = fn.find_last_of("/")) != string::npos ) fn = fn.substr(pos+1);
	return fn + ": Record " + wcefile.getRecord() + " - Strophe " + wcefile.getStrophe();
}

void Song::printContents() const {
	vector<SongLine>::const_iterator si;
	clog << "----- Annotations -----" << endl;
	for ( si = songLines.begin(); si != songLines.end(); si++ ) {
		si->printAnnotations();
	}
	clog << "-----------------------" << endl;

}
