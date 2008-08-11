%option c++
%option prefix="Lily"
%option noyywrap

digit			[0-9]
pitchbase		[a-grs]|(as)|(es)
alteration		[(is)|(es)]
octave			[',]
ws				[| \t\n]+
time_command	\\time
times_command	\\times
glis			\\gl
cross			\\x
duration		{digit}\.*
note			{pitchbase}{alteration}*{octave}*{duration}*{ws}*[\~()\[\]]*

%%
[\^_]\"\*\"{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
[\^_]\"\+\"{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
[\^_]\"[^\"]*\"{ws}*								{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\ficta{ws}*										{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\grace{ws}*\{[^\}]*\}{ws}*							{/*clog << "GRACE  " << YYText() << endl; */ return 5; }
[\^_]?\\staccato{ws}*								{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
[\^_]?\\staccatissimo{ws}*							{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
[\^_]?\\trill{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
[\^_]?\\fermata{ws}*								{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
[\^_]?\\(prall)+{ws}*								{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
[\^_]\"\/\/\"{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\sb{ws}*											{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\bar{ws}*\"[^\"]*\"{ws}*							{/*clog << "INSTR  " << YYText() << endl; */ return 6; }
\\segno{ws}*										{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\coda{ws}*											{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\fine{ws}*											{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\dc[fc]?{ws}*										{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\ds[fc]?{ws}*										{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\~													return 7; /*clog << "TIE  " << YYText() << endl;*/
\}													return 8; /*clog << "TIE  " << YYText() << endl;*/
({glis}|{cross}|\{)*{ws}*{note}{ws}*\}*				return 1; /*clog << "NOTE  " << YYText() << endl;*/
{time_command}{ws}*{digit}+"/"{digit}+{ws}*			return 2; /*clog << "TIME  " << YYText() << endl;*/
{times_command}{ws}*{digit}+"/"{digit}+{ws}*			return 3; /*clog << "TIMES " << YYText() << endl;*/ 
{ws}												{/*clog << "WHITESPACE" << endl; */ return -2; }
.													return -1; /*cerr << "Unrecognized character: "<< YYText() << endl;*/
%%

int Lilywrap(void) {
return 1;
}

