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

#include"prekern.h" 
using namespace std;

string getDuur(string kernwoord)
// als geen duur: uitkomst is ""
{
	string result;
	int i=0;
	while(kernwoord[i] == '&' || kernwoord[i] == '{' || kernwoord[i] == '[' || kernwoord[i] == '(')
		i++;
	
	//nu wijst i naar eerste cijfer, als er een is.
	// even controleren
	if(!isdigit(kernwoord[i]))
		return "";
		
	while(isdigit(kernwoord[i]))
	{
		result += kernwoord[i];
		i++;
	}
	
	//nog kijken of er een . volgt
	if(kernwoord[i] == '.') result += kernwoord[i];
	
	//klaar
	return result;
}

int getPitchclass(string kernwoord)
// c=1, d=2 etc
// rust = 0
// -1 als mislukt
{
	int result = -1;
	int i=0;
	char pitch;
	
	//voorlooptroep
	while(kernwoord[i] == '&' || kernwoord[i] == '{' || kernwoord[i] == '[' || kernwoord[i] == '(')
	{
		i++;
	}
	
	//als duurinfo, dan verwijderen
	while(isdigit(kernwoord[i]))
		i++;
	//nog even . verwijderen
	if(kernwoord[i] == '.')
		i++;

	// er kan nog een @ komen
	if(kernwoord[i] == '@')
		i++;
		
	//als het goed is komt nu de toonhoogte
	pitch=tolower(kernwoord[i]);
	
	// als pitch geen rust is
	// in interval c-b brengen (c = 99)
	if( pitch != 'r')
	{
		pitch = pitch - 98;
		if(pitch<=0) pitch = pitch+7;
	}
	else
	{
		pitch = 0;
	}
	
	return pitch;
}

bool isNoot(char c)
{
	bool result = false;
	c = tolower(c);
	if(c>=97 && c<=103) result = true;
	return result;
}

int getOktaaf(string kernwoord)
// als geen oktaaf: uitkomst is -1
{
	int oktaaf = -1;
	int i=0;
	
	//toonhoogte zoeken
	while(!isalpha(kernwoord[i]))
		i++;
	
	// i staat nu aan begin toonhoogte

	// alleen verder als voor toonhoogte @ staat
	if((i!=0 && kernwoord[i-1]!='@') || i==0)
		return -1;
	
	if(isupper(kernwoord[i]))
	{
		oktaaf = 3;
		i++;
		while(isNoot(kernwoord[i]) && i<kernwoord.length())
		{
			oktaaf--;
			i++;
		}
	}
	else
	if(islower(kernwoord[i]))
	{
		oktaaf = 4;
		i++;
		while(isNoot(kernwoord[i]) && i<kernwoord.length())
		{
			oktaaf++;
			i++;
		}
	}
	
	//cout << "kernwoord: " << kernwoord <<  " oktaaf: " << oktaaf << endl;
	
	//klaar
	return oktaaf;
}

int getAantalnoten(string regel)
{
	int aantal = 1;
	for(int i=0; i<regel.length();i++)
	{
		//noot als na spatie een niet-spatie volgt
		if(regel[i] == ' ' && i+1<regel.length() && regel[i+1] != ' ') aantal++;
	}
	return aantal;
}

string getKernwoord(string regel, int positie)
// eerste noot heeft positie 0
{
	string result;
	int aantal = 0;
	int i = 0;
	while(aantal != positie)
	{
		if(regel[i] == ' ') aantal++;
		i++;
	}
	
	// nu wijst i naar het begin van de gewenste noot
	
	while(regel[i] != ' ' && i<regel.length())
	{
		result += regel[i];
		i++;
	}
	return result;
}

string stripInvoer(string invoer)
//verwijdert duur en oktaafindicatie < en > : alle pitches worden 1 kleine letter.
{
	string result;
	int i=0;
	int positie;
	
	//voorlooptroep behouden
	while(invoer[i] == '&' || invoer[i] == '{' || invoer[i] == '[' || invoer[i] == '(')
	{
		result += invoer[i];
		i++;
	}
	
	//als duurinfo, dan verwijderen
	while(isdigit(invoer[i]))
		i++;
	//nog even . verwijderen
	if(invoer[i] == '.')
		i++;
	
	// @ weghalen
	if(invoer[i] == '@')
		i++;
		
	//als het goed is komt nu de toonhoogte
	result += tolower(invoer[i]);
	i++;
	
	//nog rest toonhoogte wegwerken
	while((tolower(invoer[i]) == tolower(invoer[i-1])) && i<invoer.length())
		i++;
		
	//de rest toevoegen (als die er is)
	while(i<invoer.length())
	{
		result += invoer[i];
		i++;
	}
	
	//< en > weghalen
	if((positie = result.find('<')) != string::npos)
		result.erase(positie,1);
	if((positie = result.find('>')) != string::npos)
		result.erase(positie,1);
	
	//klaar
	return result;
}

string formatNoot(string stripinvoer, string duur, int oktaaf)
//verwacht gestripte invoer!!!
{
	string result;
	int i=0;
	//voorlooptroep
	while(stripinvoer[i] == '&' || stripinvoer[i] == '{' || stripinvoer[i] == '[' || stripinvoer[i] == '(')
	{
		result += stripinvoer[i];
		i++;
	}
	
	//nu toonduur toevoegen
	result += duur;
	
	//nu pitchclass opvragen
	string pitchclass;
	pitchclass += stripinvoer[i];
	i++;
	
	//pitchclass uitbreiden naar gewenst oktaaf
	//behalve als het een rust betreft!
	if( pitchclass != "r")
	{
		switch(oktaaf)
		{
		case 0:
			pitchclass[0] = toupper(pitchclass[0]);
			pitchclass += pitchclass[0];
			pitchclass += pitchclass[0];
			pitchclass += pitchclass[0];
			break;
		case 1:
			pitchclass[0] = toupper(pitchclass[0]);
			pitchclass += pitchclass[0];
			pitchclass += pitchclass[0];
			break;
		case 2:
			pitchclass[0] = toupper(pitchclass[0]);
			pitchclass += pitchclass[0];
			break;
		case 3:
			pitchclass[0] = toupper(pitchclass[0]);
			break;
		case 4:
			break;
		case 5:
			pitchclass += pitchclass[0];
			break;
		case 6:
			pitchclass += pitchclass[0];
			pitchclass += pitchclass[0];
			break;
		case 7:
			pitchclass += pitchclass[0];
			pitchclass += pitchclass[0];
			pitchclass += pitchclass[0];
			break;
		default:
			cerr << "vreemd oktaaf:" << stripinvoer << " " << oktaaf << endl;
			break;
		}
	}
	
	//toevoegen
	result += pitchclass;
	
	//rest toevoegen
	while(i<stripinvoer.length())
	{
		result += stripinvoer[i];
		i++;
	}
	
	//klaar
	return result;
	
}

int main(int argc, char** argv)
{
	// controleer argumenten
	if(argc != 2)
	{
		cerr << "Incorrect number of parameters" << endl;
		exit(1);
	}

	// probeer invoerbestand te openen
	ifstream bron(argv[1]);
	if(!bron.is_open())
	{
		cerr << "Kan " << argv[1] << " niet openen" << endl;
		exit(1);
	}

	// open uitvoerbestand
	// bestandsnaam: extensie pk verwijderen, extensie krn toevoegen
	string naamIn(argv[1]);
	string naamUit = naamIn;
	int positie;
	if( (positie = naamIn.find(".pk")) != string::npos)
	{
		naamUit.erase(positie);
	}
	naamUit.append(".krn");

	// bestand openen om te schrijven
	ofstream uit(naamUit.c_str());
	if(!uit)
	{
		cerr << "Kan " << naamUit << " niet openen" << endl;
		exit(1);
	}
	
	// hierin komen de regels
	string line;
	int linenr=0;
	
	// bij te houden variabelen
	int pitchclass[MAXNOTES]; // max tien noten in een spine
	int oktaaf[MAXNOTES]; 
	string duur; // toonduur
	
	// kan er een regel gelezen worden?
	if(!getline(bron,line)) exit(0);
	linenr++;
	// eerste dataregel opzoeken:
	while(line[0]=='=' || line[0]=='!' || line[0]=='*' || line[0]=='%')
	{
		if(line[0]=='%')
		{
			uit << line.substr(1) << endl;
		}
		else
		{
			if(line == "**prekern") line = "**kern";
			uit << line << endl;
		}
		if(!getline(bron,line)) exit(0);
		linenr++;
	}

	// nu staat in line de eerste regel met data
	
	//bepaal duur
	string kernwoord;
	kernwoord = getKernwoord(line,0); //kernwoord: eerste noot op de regel
	duur = getDuur(kernwoord);
	if (duur == "") duur = "4";
	
	//bepaal oktaven
	for(int i = 0; i<getAantalnoten(line) | i>=MAXNOTES; i++)
	{
		kernwoord = getKernwoord(line,i);
		oktaaf[i] = getOktaaf(kernwoord); //-1 als rust
		pitchclass[i] = getPitchclass(kernwoord); // 0 als rust
	}
	
	// nu is bekend in welke oktaven begonnen moet worden
	// en met welke duur begonnen moet worden
	
	// eerste regel wegschrijven
	for(int i = 0; i<getAantalnoten(line) || i>=MAXNOTES; i++)
	{
		if(i!=0) uit << " ";
		uit << formatNoot(stripInvoer(getKernwoord(line,i)),duur,oktaaf[i]);
	}
	uit << endl;

	// overige regels
	while(getline(bron, line))
	{
		//regelnummer
		linenr++;
		
		//debug:
		//cout << linenr << endl;
				
		// commentaar etc overslaan
		// regels met % ongewijzigd overnemen (zonder %)
		if(line[0] == '!' || line[0] == '*' || line[0] == '=' || line[0]=='%')
		{
			if(line[0]=='%')
			{
				uit << line.substr(1) << endl;
			}
			else
			{
				uit << line << endl;
			}
			continue;
		}
		
		//lege regels overslaan
		if(line.length() == 0)
		{
			uit << line << endl;
			continue;
		}
				
		// bepaal duur
		string kernwoord; // hier komt de heel uitdrukking voor de noot in
		string nieuweduur; // nodig omdat duur van vorige noot ook nog nodig is
		int nieuwoktaaf; // nodig omdat vorig oktaaf ook nog nodig is
		kernwoord = getKernwoord(line,0);
		nieuweduur = getDuur(kernwoord);
		if(nieuweduur != "") duur = nieuweduur;
		
		//bepaal pitchclasses en oktaven op grond van voorgaande noot.
		//voor elk van de noten in de timeslice
		for(int i = 0; i<getAantalnoten(line) | i>=MAXNOTES; i++) // voor alle noten
		{
			kernwoord = getKernwoord(line,i); // kernwoord is i-de noot
			
			//cout << "kernwoord: " << kernwoord << endl;
			
			//controleren of expliciet oktaaf aagegeven is
			int nieuwoktaaf = getOktaaf(kernwoord);
			int oudeoktaaf = oktaaf[i]; //sowieso bewaren
			
			// als niet expliciet: oktaaf blijft hetzelfde
			if( nieuwoktaaf != -1 ) oktaaf[i] = nieuwoktaaf;

			//cout << linenr << kernwoord << ' ' << oktaaf[i] <<endl;
			
			// vorige pitchclass onthouden			
			int oudepitchclass = pitchclass[i];
			// alleen aanpassen als geen rust
			if(getPitchclass(kernwoord)!=0) //geen rust
			{
				pitchclass[i] = getPitchclass(kernwoord);
			}
			
			// in welk oktaaf komt nieuwe toon terecht?
			// als het verschil tussen de oude toon en de nieuwe in hetzelfde
			// oktaaf >= 4, dan komt de nieuwe toon in een ander oktaaf
			// terecht (bijv: a = 5, e = 2, verschil = 3 -> zelfde oktaaf)
			// behalve als het een rust betreft! (pitchclass=0)
			bool geenrust = pitchclass[i] != 0;
			// of als het de eerst klinkende noot is. (oudeoktaaf=-1)
			bool nieteerstenoot = oudeoktaaf != -1;
			// of als de oktaafhooge expliciet gegeven is (met @)
			bool oktaafnietexpliciet = (kernwoord.find('@')==string::npos);

			if(geenrust && nieteerstenoot && oktaafnietexpliciet)
			{
				int verschil = oudepitchclass-pitchclass[i];
				if(abs(verschil) >=4)
				{
					if(verschil < 0) oktaaf[i]--;
					if(verschil > 0) oktaaf[i]++;
				}
			}
			
			//cout << kernwoord << ' ' << oktaaf[i] <<endl;

			// nu nog verschuiving controleren.
			if((positie = kernwoord.find('<')) != string::npos)
			{
				oktaaf[i]--;
			}
			if((positie = kernwoord.find('>')) != string::npos)
			{
				oktaaf[i]++;
			}
			
		}
		
		//wegschrijven
		for(int i=0; i<getAantalnoten(line) || i>=MAXNOTES; i++)
		{
			if(i!=0) uit << " ";
			uit << formatNoot(stripInvoer(getKernwoord(line,i)),duur,oktaaf[i]);
		}
		uit << endl;

		line.clear();
	}
	
	// opruimen
	bron.close();
	uit.close();
	return 0;
}
