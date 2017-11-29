/*
 *  TimeSignature.h
 *  wce2krn
 *
 *  Created by Peter Van Kranenburg on 10/27/06.
 *  Copyright 2006 Peter van Kranenburg. All rights reserved.
 *
 */

#ifndef TIMESIGNATURE_H
#define TIMESIGNATURE_H

#include "RationalTime.h"
#include<string>
using namespace std;

class TimeSignature {
public:
	TimeSignature();
	TimeSignature(string ts); //input is lilypond time signature (with or without "\time " or "\itime ").
	                          //no whitespace at begin or end allowed!
	TimeSignature(int num, int den) : numerator(num), denominator(den) {};
	
	RationalTime getRationalTime() const;
	int getNumerator() const;
	int getDenominator() const;
	
	string getKernTimeSignature() const;

private:
	int numerator;
	int denominator;
};

#endif
