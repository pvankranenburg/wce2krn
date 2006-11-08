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
	bool emptyline = false;
	SongLine sl;
	for( str_it=wcelines.begin(); str_it!=wcelines.end(); str_it++) {
		emptyline = false;
		if ((*str_it).empty()) emptyline = true;
		if ((*str_it).find_first_not_of(" \t") == string::npos ) emptyline = true;
		if ( emptyline ) lineprofile.push_back(false); else lineprofile.push_back(true);
	}
	//nu staan in lineprofile de dataregels op true
	for( i = 0; i <= lineprofile.size(); i++ ) { //doorloop het profile tot size() om ook laatste songline toe te voegen als er geen lege regel volgt
		if (lineprofile[i] && i<lineprofile.size()) { // dataline
			singleline.push_back(wcelines[i]); // is cleared after adding to songLines
		} else { //empty line
			if( !singleline.empty() ) { //don't add empty lines to songLines
				if( songLines.empty() ) { //first line of song
					int initialBarnumber = 0;
					if ( translateUpbeat(wcefile.getUpbeat()) == RationalTime(0,1) ) initialBarnumber = 1;
					songLines.push_back(SongLine(singleline,
							      translateUpbeat(wcefile.getUpbeat()),
								  translateTimeSignature(wcefile.getTimeSignature()),
								  0,
								  false,
								  4,
								  'g',
								  translateKeySignature(wcefile.getKey()),
								  initialBarnumber));
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
								  (songLines.back()).getKeySignature(),
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
	if ( lyUpbeat.size() == 0 ) return RationalTime(0,1);
	int duration = 0;
	int amount = 1;
	bool dot = false;
	pvktrim(lyUpbeat);
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

void Song::writeToDisk(string basename_full, SongLine::representation repr, bool lines, bool absoulte) const {
	vector<string> part;
	vector<string>::iterator part_it;
	
	int line = 0;
	stringstream ss;
	string s, basename, path;
	string ext = ".ly";
	string::size_type pos;

	if ( repr == SongLine::KERN ) ext = ".krn";

	if( ( pos = basename_full.find_last_of("/") ) != string::npos ) {
		basename = basename_full;
		basename = basename.erase(0,pos+1);
		path = basename_full.erase(pos+1);
	} else {
		basename = basename_full;
		path = "";
	}

	string outname = path + "all-" + basename + ext;
	ofstream out;
	
	if (!lines) {
		out.open(outname.c_str());
		cout << "Writing " << outname << endl;
	}
	

	if (songLines.size() > 0) {
		vector<SongLine>::const_iterator si;
		//songlines
		for ( si = songLines.begin(); si != songLines.end(); si++ ) {
			//preamble
			if (lines || si == songLines.begin()) {
				if ( lines ) {
					if (out.is_open()) out.close();
					ss.clear();
					ss << path << line << "-" << basename << ext;
					ss >> s;
					out.open(s.c_str());
					cout << "Writing " << s << endl;
				}
				switch(repr) {
					case SongLine::KERN: part = si->getKernBeginSignature(); break;
					case SongLine::ABSLY: part = si->getLyBeginSignature(true); break;
					case SongLine::RELLY: part = si->getLyBeginSignature(false); break;
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
					case SongLine::KERN: part = si->getKernEndSignature(); break;
					case SongLine::ABSLY:
					case SongLine::RELLY: part = si->getLyEndSignature(); break;
				}
				for ( part_it = part.begin(); part_it != part.end(); part_it++ )
					out << *part_it << endl;
			}
		line++;
		}
		
	}
	//close
	
	if ( out.is_open() ) out.close();
	
}

int Song::translateKeySignature(string lykey) const {
	int res = 0;
	pvktrim(lykey);
	if ( lykey.size() == 0 ) return res;
	
	bool minor = ( lykey.find("minor") != string::npos );

	string root = lykey.erase(lykey.find("\\"));
	pvktrim (root);
	
	if (root == "c") { res = 0; }
	if (root == "cis") { res = 7; }
	if (root == "des") { res = -5; }
	if (root == "d") { res = 2; }
	if (root == "dis") { res = 9; }
	if (root == "es") { res = -3; }
	if (root == "e") { res = 4;  }
	if (root == "f") { res = -1; }
	if (root == "fis") { res = 6; }
	if (root == "ges") { res = 6; }
	if (root == "g") { res = 1; }
	if (root == "gis") { res = 8; }	
	if (root == "as") { res = -4; }	
	if (root == "a") { res = 3; }
	if (root == "ais") { res = 10; }
	if (root == "bes") { res = -2; }
	if (root == "b") { res = 5; }
		
	if (minor) res = res - 3;
	
	return res;
}
