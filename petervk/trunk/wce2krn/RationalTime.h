/*
 *  RationalTime.h
 *  wce2krn
 *
 *  Created by Peter Van Kranenburg on 10/27/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef RATIONALTIME_H
#define RATIONALTIME_H

class RationalTime {
public:
	RationalTime();
	RationalTime(int num, int den);
	
	int getNumerator() const;
	int getNumerator(int den) const;
	int getDenominator() const;
	int getDenominator(int num) const;
	int getGCD() const;
	float getFloat() const;
	
	bool testDenominator(int i) const;
	bool testNumerator(int i) const;
	
	RationalTime operator+( RationalTime r );
	bool operator==( RationalTime r );

private:
	int numerator;
	int denominator;
	void simplify();
};
	

#endif