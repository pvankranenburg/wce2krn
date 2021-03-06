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

#include <string>

class RationalTime {
public:
	RationalTime();
	RationalTime(int num, int den);
	void simplify();

	int getNumerator() const;
	int getNumerator(int den) const;
	int getDenominator() const;
	int getDenominator(int num) const;
	int getGCD() const;
	float getFloat() const;
	
	bool testDenominator(int i) const;
	bool testNumerator(int i) const;
	
	bool isMultipleOf(int i) const;
	bool isMultipleOf(RationalTime r) const;
	
	RationalTime operator+( RationalTime r ) const;
	RationalTime operator-( RationalTime r ) const;
	RationalTime operator/( RationalTime r ) const;
	RationalTime operator-() const; //negation
	RationalTime& operator=(const RationalTime& r_);
	bool operator==( RationalTime r ) const;
	bool operator!=( RationalTime r ) const;
	bool operator==( int i ) const;
	bool operator!=( int i ) const;	
	bool operator>( RationalTime r ) const;
	bool operator>=( RationalTime r ) const;

	std::string toString() const;

private:
	int numerator;
	int denominator;
};
	

#endif
