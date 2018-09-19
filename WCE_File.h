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
#include<map>
using namespace std;

class WCE_File {
public:
	WCE_File(string inputfilename, string filename_titles);
	
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
	string getTitle() const { return title; }
	bool getMeterInvisible() const;
	string getInitialClef() const;
	string getRecord() const { return record; }
	string getStrophe() const { return strophe; }
	int getFirstNoteRelativeToOctave() const;
	char getFirstNoteRelativeToPitchClass() const;
	string getFirstNoteRelativeTo() const { return firstNoteRelativeTo; }
	string getLocation() const;
	vector<string> getFooterField() const { return footerField; }
	bool getEachPhraseNewStaff() const { return eachPhraseNewStaff; }

private:
	void readTitles(string filename);

	ifstream infile;
	map<string,string> titles;
	bool stdinput;
	string fn_titles;
	vector<string> contents;
	vector<string> beginSignature;
	vector<string> endSignature;
	vector<string> footerField;
	const string filename;
	string encoder;
	string timeSignature;
	string key;
	string midiTempo;
	string upbeat;
	string WCEVersion;
	string title;
	string firstNoteRelativeTo;
	string initialClef;
	bool meterInvisible;
	bool eachPhraseNewStaff;
	string record;
	string strophe;
	
	string extractStringFromLine(string s) const;
	vector<string> extractStringFromMultiLine(string s); //not const, modifies infile.
 };

#endif
