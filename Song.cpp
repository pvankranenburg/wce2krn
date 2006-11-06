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
using namespace std;

Song::Song(string inputfilename) : wcefile(inputfilename) {

	int i; //index
	
	//extract the lilypond lines
	vector<string> wcelines = wcefile.getContents();

	//find out number of datalines (spines)
	//find out number of lines in song
	vector<bool> lineprofile;
	vector<string>::iterator str_it;
	vector<string> singleline;
	SongLine sl;
	for( str_it=wcelines.begin(); str_it!=wcelines.end(); str_it++)
		if ( (*str_it).empty() ) lineprofile.push_back(false); else lineprofile.push_back(true);
	//nu staan in lineprofile de dataregels op true
	for( i = 0; i <= lineprofile.size(); i++ ) { //doorloop het profile tot size() om ook laatste songline toe te voegen als er geen lege regel volgt
		if (lineprofile[i] && i<lineprofile.size()) { // dataline
			singleline.push_back(wcelines[i]); // is cleared after adding to songLines
		} else { //empty line
			if( !singleline.empty() ) { //don't add empty lines to songLines
				if( songLines.empty() ) { //first line of song
					songLines.push_back(SongLine(singleline,
							      translateUpbeat(wcefile.getUpbeat()),
								  translateTimeSignature(wcefile.getTimeSignature()),
								  0,
								  false,
								  4,
								  'g',
								  0));
					singleline.clear();
					//songLines.push_back(sl);
				} else { //not first line of song
					songLines.push_back(SongLine(singleline,
							      (songLines.back()).getFinalUpbeat(), //from previous line
								  (songLines.back()).getFinalTimeSignature(),
								  (songLines.back()).getFinalDuration(),
								  (songLines.back()).getFinalDotted(),
								  (songLines.back()).getFinalOctave(),
								  (songLines.back()).getFinalLastPitchClass(),
								  (songLines.back()).getFinalBarnumber()));
					//songLines.push_back(sl);
					singleline.clear();
				}
			}
		}
	}
	
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
			cerr << "Error: all songlines should have the same number of textlines." << endl;
			exit(1);
		}
	}
}


TimeSignature Song::translateTimeSignature(string lyTimeSignature) const {
	return TimeSignature(lyTimeSignature);
}

RationalTime Song::translateUpbeat(string lyUpbeat) const {
	int duration = 0;
	int amount = 1;
	pvktrim(lyUpbeat);
	string::size_type pos;
	if( (pos = lyUpbeat.find("*")) != string::npos ) { //number of notes
		amount = atoi( (lyUpbeat.substr(pos+1)).c_str() );
		lyUpbeat.erase(pos);
	}
	duration = atoi( lyUpbeat.c_str() );
	return RationalTime(amount,duration);
}

void Song::writeToDisk(string basename, SongLine::representation repr, bool lines, bool absoulte, bool rhythm) const {
	string outname = basename + "krn";
	ofstream out(outname.c_str());
	
	vector<string> part;
	vector<string>::iterator part_it;
	
	if (songLines.size() > 0) {
		vector<SongLine>::const_iterator si;
		//songlines
		for ( si = songLines.begin(); si != songLines.end(); si++ ) {
			//preamble
			if (lines || si == songLines.begin()) {
				switch(repr) {
					case SongLine::KERN: part = songLines[0].getKernBeginSignature(); break;
					case SongLine::ABSLY: part = songLines[0].getLyBeginSignature(true); break;
					case SongLine::RELLY: part = songLines[0].getLyBeginSignature(false); break;
				}
				for ( part_it = part.begin(); part_it != part.end(); part_it++ )
					out << *part_it << endl;
			}
			switch (repr) {
				case SongLine::KERN: part = si->getKernLine(); break;
				case SongLine::ABSLY: part = si->getLyLine(true); break;
				case SongLine::RELLY: part = si->getLyLine(false); break;
			}
			for ( part_it = part.begin(); part_it != part.end(); part_it++ )
				out << *part_it << endl;
			//postamble
			if (lines || si == (songLines.end()-1) ) {
				switch(repr) {
					case SongLine::KERN: part = songLines[0].getKernEndSignature(); break;
					case SongLine::ABSLY:
					case SongLine::RELLY: part = songLines[0].getLyEndSignature(); break;
				}
				for ( part_it = part.begin(); part_it != part.end(); part_it++ )
					out << *part_it << endl;
			}
		}
		//ending
	}
	//close
	
	out.close();
	
}
