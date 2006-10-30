/*
 *  LilyLine.cpp
 *  wce2krn
 *
 *  Created by Peter Van Kranenburg on 10/27/06.
 *  Copyright 2006 Peter van Kranenburg. All rights reserved.
 *
 */

#include "SongLine.h"
#include<string>
#include<iostream>
using namespace std;

SongLine::SongLine(vector<string> lines, RationalTime upb, TimeSignature timesig, int duration, int octave) :
																   wcelines(lines),
																   upbeat(upb),
																   initialTimeSignature(timesig),
																   initialDuration(duration),
																   initialOctave(octave),
																   finalTimeSignature(TimeSignature()),
																   finalOctave(4),
																   finalDuration(0) {
}

SongLine::SongLine() : wcelines(vector<string>()),
					   upbeat(RationalTime(0,1)),
					   initialTimeSignature(TimeSignature()),
					   initialDuration(0),
					   initialOctave(4) {
}

SongLine::SongLine(const SongLine& sl) : wcelines(sl.getWceLines()),
									     upbeat(sl.getUpbeat()),
										 initialTimeSignature(sl.getInitialTimeSignature()),
										 initialDuration(sl.getInitialDuration()),
										 initialOctave(sl.getInitialOctave()),
										 finalTimeSignature(sl.getFinalTimeSignature()),
										 finalOctave(sl.getFinalOctave()),
										 finalDuration(sl.getFinalDuration()) {
}

SongLine& SongLine::operator=(const SongLine& sl) {
	if ( &sl != this ) {
	}
	cerr << "Waarschuwing: SongLine& SongLine::operator=(const SongLine& sl) gebruikt!!!!" << endl;
	return *this;
}

void SongLine::writeToStdout() const {
	cout << wcelines.size() << " lines" << endl;
	vector<string>::const_iterator it;
	for( it = wcelines.begin(); it != wcelines.end(); it++)
		cout << "line: " << *it << endl;
}