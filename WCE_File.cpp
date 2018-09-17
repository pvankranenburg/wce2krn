/*
 *  WCE_File.cpp
 *  wce2krn
 *
 *  Created by Peter Van Kranenburg on 10/27/06.
 *  Copyright 2006 Peter Van Kranenburg. All rights reserved.
 *
 */

#include "WCE_File.h"
#include "pvkutilities.h"
#include <cstdlib>
using namespace std;

WCE_File::WCE_File(string inputfilename) : filename(inputfilename), meterInvisible(false), eachPhraseNewStaff(true), record("unknown"), initialClef("treble") {
	
	
	string line; //line from wce file
	string::size_type pos;
	
	if (inputfilename == "-") stdinput = true; else stdinput = false;
	
	if ( !stdinput ){
		infile.open(inputfilename.c_str());
		if ( !infile.good() ) {
			cerr << "Error: Unable to open file: " << inputfilename << endl;
			cerr.flush();
			exit(1);
		}
	}
	
	bool good = false;
	if (stdinput) good = getline(cin,line); else good = getline(infile,line);
	
	while(good) {
		
		//replace xml entities
		while ( (pos = line.find("&amp;")) != string::npos ) line.replace(pos,5,"&");
		while ( (pos = line.find("&quot;")) != string::npos ) line.replace(pos,6,"\"");
		while ( (pos = line.find("&apos;")) != string::npos ) line.replace(pos,6,"'");
		while ( (pos = line.find("&lt;")) != string::npos ) line.replace(pos,4,"<");
		while ( (pos = line.find("&gt;")) != string::npos ) line.replace(pos,4,">");
		
		if( (pos = line.find("encoderNameTextField")) != string::npos ) {
			if (stdinput) getline(cin,line); else getline(infile,line);
			encoder = extractStringFromLine(line);
			continue;
		}
		if( (pos = line.find("keyTextField")) != string::npos ) {
			if (stdinput) getline(cin,line); else getline(infile,line);
			key = extractStringFromLine(line);
			continue;
		}
		if( (pos = line.find("tempoTextField")) != string::npos ) {
			if (stdinput) getline(cin,line); else getline(infile,line);
			midiTempo = extractStringFromLine(line);
			continue;
		}
		if( (pos = line.find("partialTextField")) != string::npos ) {
			if (stdinput) getline(cin,line); else getline(infile,line);
			upbeat = extractStringFromLine(line);
			continue;
		}
		if( (pos = line.find("timeTextField")) != string::npos ) {
			if (stdinput) getline(cin,line); else getline(infile,line);
			timeSignature = extractStringFromLine(line);
			continue;
		}
		if( (pos = line.find("<key>version</key>")) != string::npos ) {
			if (stdinput) getline(cin,line); else getline(infile,line);
			WCEVersion = extractStringFromLine(line);
			continue;
		}
		if( (pos = line.find("inputTextView")) != string::npos ) {
			if (stdinput) getline(cin,line); else getline(infile,line);
			contents = extractStringFromMultiLine(line);
			continue;
		}
		if( (pos = line.find("footerTextView")) != string::npos ) {
			if (stdinput) getline(cin,line); else getline(infile,line);
			footerField = extractStringFromMultiLine(line);
			continue;
		}
		if( (pos = line.find("endSignatureTextView")) != string::npos ) {
			if (stdinput) getline(cin,line); else getline(infile,line);
			endSignature = extractStringFromMultiLine(line);
			continue;
		}
		if( (pos = line.find("beginSignatureTextView")) != string::npos ) {
			if (stdinput) getline(cin,line); else getline(infile,line);
			beginSignature = extractStringFromMultiLine(line);
			continue;
		}
		if( (pos = line.find("NLBController-recordIDTextField")) != string::npos ) {
			if (stdinput) getline(cin,line); else getline(infile,line);
			record = extractStringFromLine(line);
			continue;
		}
		if( (pos = line.find("SignatureController-stropheNumberTextField")) != string::npos ) {
			if (stdinput) getline(cin,line); else getline(infile,line);
			strophe = extractStringFromLine(line);
			continue;
		}
		if( (pos = line.find("SignatureController-clefTextField")) != string::npos ) {
			if (stdinput) getline(cin,line); else getline(infile,line);
			initialClef = extractStringFromLine(line);
			continue;
		}
		if( (pos = line.find("SignatureController-titleTextField")) != string::npos ) {
			if (stdinput) getline(cin,line); else getline(infile,line);
			title = extractStringFromLine(line);
			continue;
		}
		if( (pos = line.find("SignatureController-isMeterInvisibleSwitch")) != string::npos ) {
			if (stdinput) getline(cin,line); else getline(infile,line);
			if ( line.find("true") != string::npos ) meterInvisible = true;
			if ( line.find("false") != string::npos ) meterInvisible = false;
			continue;
		}
		if( (pos = line.find("SignatureController-isEachPhraseNewStaffSwitch")) != string::npos ) {
			if (stdinput) getline(cin,line); else getline(infile,line);
			if ( line.find("true") != string::npos ) eachPhraseNewStaff = true;
			if ( line.find("false") != string::npos ) eachPhraseNewStaff = false;
			continue;
		}
		if( (pos = line.find("SignatureController-relativeTextField")) != string::npos ) {
			if (stdinput) getline(cin,line); else getline(infile,line);
			firstNoteRelativeTo = extractStringFromLine(line);
			continue;
		}
		// read next line
		if (stdinput) good = getline(cin,line); else good = getline(infile,line);
	}
	if (!stdinput) infile.close();
	
	//record?
	pvktrim(record);
	if ( record == "unknown" || record.size() == 0 ) {
		//see if we can get it from the file name
		string fn = filename;
		if ( (pos = fn.find_last_of("/")) != string::npos ) fn = fn.substr(pos+1);
		//cout << fn << endl;
		if ( fn.size() == 16 ) //do some tests
			if ( fn.substr(0,3) == "NLB" )
				if ( fn.substr(12,4) == ".wce" ) {
					record = fn.substr(3,6);
					//remove leading zeros
					while ( record.at(0) == '0' )
						record = record.substr(1);
				}
	}
	
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

bool WCE_File::getMeterInvisible() const {
	return meterInvisible;
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
	
	//replace xml entities
	string::size_type pos;
	while ( (pos = res.find("&amp;")) != string::npos ) res.replace(pos,5,"&");
	while ( (pos = res.find("&quot;")) != string::npos ) res.replace(pos,6,"\"");
	while ( (pos = res.find("&apos;")) != string::npos ) res.replace(pos,6,"'");
	while ( (pos = res.find("&lt;")) != string::npos ) res.replace(pos,4,"<");
	while ( (pos = res.find("&gt;")) != string::npos ) res.replace(pos,4,">");	
		
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
		res.push_back(s.substr(pos1+8, s.length()-pos1-8));
	} else {
		res.push_back(s.substr(pos1+8, pos2-pos1-8));
		foundLast = true; //string geheel op 1 regel
	}

	//vanaf nu met current
	while( !foundLast ) {
		if (stdinput) getline(cin,current); else getline(infile,current);
		if( (pos2 = current.find("</string>")) != string::npos ) {
			res.push_back(current.substr(0, pos2));
			foundLast = true;
		} else {
			res.push_back(current);
		}
	}
	
	//translate xml entities
	string::size_type pos;
	for ( int i = 0; i < res.size(); i++ ) {
		while ( (pos = res[i].find("&amp;")) != string::npos ) res[i].replace(pos,5,"&");
		while ( (pos = res[i].find("&quot;")) != string::npos ) res[i].replace(pos,6,"\"");
		while ( (pos = res[i].find("&apos;")) != string::npos ) res[i].replace(pos,6,"'");
		while ( (pos = res[i].find("&lt;")) != string::npos ) res[i].replace(pos,4,"<");
		while ( (pos = res[i].find("&gt;")) != string::npos ) res[i].replace(pos,4,">");
		//cout << "RES[" << i << "]: " << res[i] << endl;
	}
	
	return res;
}

int WCE_File::getFirstNoteRelativeToOctave() const {
	string tmp = firstNoteRelativeTo;
	int res = 3; //without ' or , it is octave 3
	string::size_type pos;
	while( (pos = tmp.find("'")) != string::npos ) { res++; tmp.erase(pos,1); }
	while( (pos = tmp.find(",")) != string::npos ) { res--; tmp.erase(pos,1); }
	
	//in case not given, assume g'
	if ( tmp.size() == 0 ) res = 4;

	return res;
}

char WCE_File::getFirstNoteRelativeToPitchClass() const {

	string tmp = firstNoteRelativeTo;
	pvktrim(tmp);
	
	if ( tmp.size() == 0 ) cerr << getLocation() << ": Warning: Initial 'relative to' pitch not given. Assuming g'" << endl;
	
	string::size_type pos;
	
	if ( (pos = tmp.find_first_of("abcdefg")) == string::npos) return 'g';
	
	return tmp[pos];

}

string WCE_File::getLocation() const {
	string fn = getFilename();
	string::size_type pos;
	if ( (pos = fn.find_last_of("/")) != string::npos ) fn = fn.substr(pos+1);
	return fn + ": Record " + getRecord() + " - Strophe " + getStrophe();
}




