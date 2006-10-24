/*
    This file is part of prekern.

    prekern is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    prekern is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with prekern; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Copyright 2004 Peter van Kranenburg
    Email: peter@musical-style-recognition.net
*/

#include<iostream>
#include<fstream>
#include<string>
#include<cctype>
#include<cmath>
using namespace std;

//wat constanten
//maximaal aantal noten in een timeslice
const int MAXNOTES = 10;

string getDuur(string kernwoord);
int getPitchclass(string kernwoord);
bool isNoot(char c);
int getOktaaf(string kernwoord);
int getAantalnoten(string regel);
string getKernwoord(string regel, int positie);
string stripInvoer(string invoer);
string formatNoot(string stripinvoer, string duur, int oktaaf);
