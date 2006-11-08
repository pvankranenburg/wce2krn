/*
 *  WCE_File.cpp
 *  wce2krn
 *
 *  Created by Peter Van Kranenburg on 10/27/06.
 *  Copyright 2006 Peter Van Kranenburg. All rights reserved.
 *
 */

#include "WCE_File.h"
using namespace std;

WCE_File::WCE_File(string inputfilename) : filename(inputfilename) {
	
	string line; //line from wce file
	string::size_type pos;
	
	infile.open(inputfilename.c_str());
	if (!infile.good() ) {
		cerr << "Error: Unable to open file: " << inputfilename << endl;
		cerr.flush();
		exit(1);
	}
	
	while(getline(infile,line)) {
		if( pos = line.find("encoderNameTextField") != string::npos ) {
			getline(infile,line);
			encoder = extractStringFromLine(line);
			continue;
		}
		if( pos = line.find("keyTextField") != string::npos ) {
			getline(infile,line);
			key = extractStringFromLine(line);
			continue;
		}
		if( pos = line.find("tempoTextField") != string::npos ) {
			getline(infile,line);
			midiTempo = extractStringFromLine(line);
			continue;
		}
		if( pos = line.find("partialTextField") != string::npos ) {
			getline(infile,line);
			upbeat = extractStringFromLine(line);
			continue;
		}
		if( pos = line.find("timeTextField") != string::npos ) {
			getline(infile,line);
			timeSignature = extractStringFromLine(line);
			continue;
		}
		if( pos = line.find("<key>version</key>") != string::npos ) {
			getline(infile,line);
			WCEVersion = extractStringFromLine(line);
			continue;
		}
		if( pos = line.find("inputTextView") != string::npos ) {
			getline(infile,line);
			contents = extractStringFromMultiLine(line);
			continue;
		}
		if( pos = line.find("endSignatureTextView") != string::npos ) {
			getline(infile,line);
			endSignature = extractStringFromMultiLine(line);
			continue;
		}
		if( pos = line.find("beginSignatureTextView") != string::npos ) {
			getline(infile,line);
			beginSignature = extractStringFromMultiLine(line);
			continue;
		}
	}
	infile.close();
}

string WCE_File::getFilename() const {
	return filename;
}


vector<string> WCE_File::getContents() const {
	return contents;
}

vector<string> WCE_File::getBeginSignature() const {
	return beginSignature;
}

vector<string> WCE_File::getEndSignature() const {
	return endSignature;
}

string WCE_File::getEncoder() const {
	return encoder;
}

string WCE_File::getTimeSignature() const {
	return timeSignature;
}

string WCE_File::getKey() const {
	return key;
}

string WCE_File::getMidiTempo() const {
	return midiTempo;
}

string WCE_File::getUpbeat() const {
	return upbeat;
}

string WCE_File::getWCEVersion() const {
	return WCEVersion;
}

string WCE_File::extractStringFromLine(string s) const
{
	string::size_type pos1, pos2;
	string res="";

	if( (pos1 = s.find("<string>")) == string::npos )
		return res;
	if( (pos2 = s.find("</string>")) == string::npos )
		return res;
	res = s.substr( pos1+8, pos2-pos1-8 );
	
	return res;
}

vector<string> WCE_File::extractStringFromMultiLine(string s)
// not robust
{
	string::size_type pos1, pos2;
	vector<string> res;
	string current;
	bool foundLast=false;
	
	//werken met s
	if( (pos1 = s.find("<string>")) == string::npos )
		return res; // no <string> found. should be there.
	if( (pos2 = s.find("</string>")) == string::npos ) {
		res.push_back(s.substr(pos1+8, pos2-pos1-8));
	} else {
		res.push_back(s.substr(pos1+8, s.length()-pos1-8));
		return res; //string geheel op 1 regel
	}

	//vanaf nu met current
	while( !foundLast ) {
		getline(infile, current);
		if( (pos2 = current.find("</string>")) != string::npos ) {
			res.push_back(current.substr(0, pos2));
			foundLast = true;
		} else {
			res.push_back(current);
		}
	}
	
	return res;
}