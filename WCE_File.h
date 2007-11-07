/*
 *  WCE_File.h
 *  wce2krn
 *
 *  Created by Peter Van Kranenburg on 10/27/06.
 *  Copyright 2006 Peter Van Kranenburg. All rights reserved.
 *
 */

#ifndef WCE_FILE_H
#define WCE_FILE_H

#include<iostream>
#include<fstream>
#include<string>
#include<vector>
using namespace std;

class WCE_File {
public:
	WCE_File(string inputfilename);
	
	vector<string> getContents() const;
	vector<string> getBeginSignature() const;
	vector<string> getEndSignature() const;
	string getFilename() const;
	string getEncoder() const;
	string getTimeSignature() const;
	string getKey() const;
	string getMidiTempo() const;
	string getUpbeat() const;
	string getWCEVersion() const;
	bool getMeterInvisible() const;
	string getRecord() const { return record; }
	string getStrophe() const { return strophe; }
	int getFirstNoteRelativeToOctave() const;
	char getFirstNoteRelativeToPitchClass() const;
	string getFirstNoteRelativeTo() const { return firstNoteRelativeTo; }

private:
	ifstream infile;
	bool stdinput;
	vector<string> contents;
	vector<string> beginSignature;
	vector<string> endSignature;
	const string filename;
	string encoder;
	string timeSignature;
	string key;
	string midiTempo;
	string upbeat;
	string WCEVersion;
	string firstNoteRelativeTo;
	bool meterInvisible;
	string record;
	string strophe;
	
	string extractStringFromLine(string s) const;
	vector<string> extractStringFromMultiLine(string s); //not const, modifies infile.
 };

#endif