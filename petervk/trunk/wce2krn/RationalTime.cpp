/*
 *  RationalTime.cpp
 *  wce2krn
 *
 *  Created by Peter Van Kranenburg on 10/27/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include<iostream>
using namespace std;

#include "RationalTime.h"

RationalTime::RationalTime() {
	numerator = 0;
	denominator = 1;
}

RationalTime::RationalTime(int num, int den) : numerator(num), denominator(den) {
	if ( denominator == 0 )
	{ cerr << "Denominator is zero!" << endl; exit(1); }
	simplify();
}

int RationalTime::getNumerator() const {
	return numerator;
}

int RationalTime::getNumerator(int den) const {
	if( testDenominator(den) )
		return den*numerator/denominator;
	else
		throw(1);
}

int RationalTime::getDenominator() const {
	return denominator;
}

int RationalTime::getDenominator(int num) const {
	if( testNumerator(num) )
		return num*denominator/numerator;
	else
		throw(1);
}

float RationalTime::getFloat() const {
	return (float) numerator / (float) denominator;
}

int RationalTime::getGCD() const {
	int res = 1;
	int kleinste;
	if(denominator>numerator) kleinste = numerator; else kleinste = denominator;

	int k = 1;
	while (k<=kleinste) {
		if ( (int(numerator/k)*k == numerator) && (int(denominator/k)*k == denominator) ) res = k;
		k++;
	}
	
	return res;
}

bool RationalTime::testDenominator(int i) const {
	return ((i*numerator) % denominator) == 0;
}

bool RationalTime::testNumerator(int i) const {
	return ((i*denominator) % numerator) == 0;
}

bool RationalTime::isMultipleOf(int i) const {
	//must be integer:	
	if ( (numerator%denominator)!=0 ) return false;
	//now is integer;
	if ( (numerator/denominator)%i == 0 ) return true;
	//sure not dividable by i
	return false;
}
	
bool RationalTime::isMultipleOf(RationalTime r) const {
	int numr;
	if ( testDenominator(r.getDenominator()) )
		numr = getNumerator(r.getDenominator());
	else
		return false;
	if ( (numerator%numr) == 0 )
		return true;
	
	return false;
}

	
void RationalTime::simplify() {
	int gcd = getGCD();
	numerator = numerator / gcd;
	denominator = denominator / gcd;
}

RationalTime RationalTime::operator+( RationalTime r) const {
	int newdenominator = denominator*r.getDenominator();
	int newnumerator = r.getNumerator()*denominator + r.getDenominator()*numerator;
	RationalTime res(newnumerator, newdenominator);
	return res;
}

RationalTime RationalTime::operator-( RationalTime r) const {
	int newdenominator = denominator*r.getDenominator();
	int newnumerator = r.getDenominator()*numerator - r.getNumerator()*denominator;
	RationalTime res(newnumerator, newdenominator);
	return res;
}

RationalTime RationalTime::operator-() const {
	RationalTime res(-numerator, denominator);
	return res;
}

bool RationalTime::operator==( RationalTime r) const {
	return (numerator*r.getDenominator()) == (denominator*r.getNumerator());
}

bool RationalTime::operator==( int i ) const {
	return numerator == (denominator*i);
}

RationalTime& RationalTime::operator=(const RationalTime& r) {
	if( &r != this ) {
		numerator = r.getNumerator();
		denominator = r.getDenominator();
	}
	return *this;
}