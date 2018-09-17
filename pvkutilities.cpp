/*
 *  pvkutilities.cpp
 *  wce2krn
 *
 *  Created by Peter Van Kranenburg on 11/3/06.
 *  Copyright 2006 Peter van Kranenburg. All rights reserved.
 *
 */

#include "pvkutilities.h"

string& pvktrim(string& s, string drop) {
	s.erase(s.find_last_not_of(drop)+1);
    s.erase(0,s.find_first_not_of(drop));
	return s;
}

string& columnize(string& s, int length, char fill) {
	while ( s.size() < length ) s = s + fill;
	return s;
}

string& removeTextFromNote(string& external_token) {
	string::size_type pos1, pos2;
	//locate the texts (if any)
	while( (pos1 = external_token.find("^\"")) != string::npos || (pos1 = external_token.find("_\"")) != string::npos ) {
		//find end of text
		pos2 = external_token.find_first_of('\"', pos1+2); //skip the ^" or _"
		external_token.erase(pos1, pos2-pos1+1);
	}
	return external_token;
}

int translateKeySignature(string lykey) {
	int res = 0;
	pvktrim(lykey);

	//strip away "\key" if necessary
	string::size_type poskey = lykey.find("\\key");
	if (poskey != string::npos) {
		lykey.erase(poskey, 4);
	}
	pvktrim(lykey);

	if ( lykey.size() == 0 ) return res;

	bool major = ( lykey.find("\\major") != string::npos );
	bool minor = ( lykey.find("\\minor") != string::npos );
	bool ionian = ( lykey.find("\\ionian") != string::npos );
	bool locrian = ( lykey.find("\\locrian") != string::npos );
	bool aeolian = ( lykey.find("\\aeolian") != string::npos );
	bool mixolydian = ( lykey.find("\\mixolydian") != string::npos );
	bool lydian = ( lykey.find("\\lydian") != string::npos );
	bool phrygian = ( lykey.find("\\phrygian") != string::npos );
	bool dorian = ( lykey.find("\\dorian") != string::npos );

	if ( !major && !minor && !ionian && !locrian && !aeolian && !mixolydian && !lydian && !phrygian && !dorian ) {
		cerr << "Warning: Bad key signature. Assuming c major." << endl;
		return res;
	}

	string root = lykey.erase(lykey.find("\\"));
	pvktrim (root);

	if (root == "ces") { res = -7; }
	if (root == "c") { res = 0; }
	if (root == "cis") { res = 7; }
	if (root == "des") { res = -5; }
	if (root == "d") { res = 2; }
	if (root == "dis") { res = 9; }
	if (root == "es") { res = -3; }
	if (root == "ees") { res = -3; }
	if (root == "e") { res = 4;  }
	if (root == "f") { res = -1; }
	if (root == "fis") { res = 6; }
	if (root == "ges") { res = -6; }
	if (root == "g") { res = 1; }
	if (root == "gis") { res = 8; }
	if (root == "as") { res = -4; }
	if (root == "aes") { res = -4; }
	if (root == "a") { res = 3; }
	if (root == "ais") { res = 10; }
	if (root == "bes") { res = -2; }
	if (root == "b") { res = 5; }

	if (minor) res = res - 3 + 30;
	if (ionian) res = res + 60;
	if (dorian) res = res - 2 + 90;
	if (phrygian) res = res -4 + 120;
	if (lydian) res = res +1 + 150;
	if (mixolydian) res = res -1 + 180;
	if (aeolian) res = res -3 + 210;
	if (locrian) res = res -5 + 240;

	//cout << "ROOT " << root << " " << res << endl;

	return res;
}


vector<string> formatFooterField( vector<string> footerField ) {
	vector<string> res;

	if ( footerField.size() < 1) { //empty

	}

	string::size_type pos;

	//first line
	//remove lilypond markup command
	string firstline = footerField[0];
	if ( ( pos = firstline.find_first_of("\\markup { \\wordwrap-string #\"")) != string::npos ) {
		firstline.erase(pos, pos+29);
		pvktrim(firstline); //there seems always be a space after the markup command....
		if ( firstline.size() > 0 )
			res.push_back(firstline);
	}

	//middle lines
	for (int i=1; i<footerField.size()-1; i++) {
		string line = footerField[i];
		if (line.size() > 0) {
			//there might be only \r in the line
			if ( line.size() == 1 ) {
				if ( line[0] == '\r' ) {
					continue;
				}
			}
			res.push_back(line);
		}
	}

	//last line
	//remove "
	string lastline = footerField.back();
	if ( ( pos = lastline.find_last_of("\"}")) != string::npos ) {
		lastline.erase(pos, pos+2);
		if ( lastline.size() > 0)
			res.push_back(lastline);
	}

	return res;
}

