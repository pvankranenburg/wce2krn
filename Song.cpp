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
								  4));
					singleline.clear();
					//songLines.push_back(sl);
				} else { //not first line of song
					songLines.push_back(SongLine(singleline,
							      (songLines.back()).getUpbeat(), //from previous line
								  (songLines.back()).getFinalTimeSignature(),
								  (songLines.back()).getFinalDuration(),
								  (songLines.back()).getFinalOctave()));
					//songLines.push_back(sl);
					singleline.clear();
				}
			}
		}
	}
	
	
	//test
	/*vector<SongLine>::iterator it_sl;
	for(it_sl=songLines.begin(); it_sl != songLines.end(); it_sl++) {
		cout << "songline: " << endl;
		(*it_sl).writeToStdout();
	}*/
	
	//test if each songline has same number of lines (spines)
	vector<SongLine>::iterator it_sl;
	for(it_sl=songLines.begin()+1; it_sl != songLines.end(); it_sl++) {
		if ( (*it_sl).getWceLines().size() != (*(it_sl-1)).getWceLines().size() ) {
			cerr << "Error: all songlines should have the same number of textlines." << endl;
			exit(1);
		}
	}
}

vector<string> Song::getAbsoluteLilypondFile() const {
	vector<string> res;
	return res;
}

vector< vector<string> > Song::getAbsoluteLilypondLineFiles() const {
	vector< vector<string> > res;
	return res;
}

vector<Spine> Song::getKernFile() const {
	vector<Spine> res;
	return res;
}

TimeSignature Song::translateTimeSignature(string lyTimeSignature) const {
	return TimeSignature(lyTimeSignature);
}

RationalTime Song::translateUpbeat(string lyUpbeat) const {
	RationalTime res;
	return res;
}

void Song::writeKern(string basename, bool lines, bool rhythm) const {
}

void Song::writeLy(string basename, bool lines, bool absoulte, bool rhythm) const {
}