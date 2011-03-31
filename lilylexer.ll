%option c++
%option prefix="Lily"
%option noyywrap
%x INMARKUP
%{
int brace_count = 0;
%}

digit			[0-9]
pitchbase		(as)|(es)|[a-grs]
alteration		[(is)|(es)]
octave			[',]
ws				[| \t\n]+
time_command	(\\itime)?{ws}*\\time
times_command	\\times
glis			\\gl
cross			\\x
duration		{digit}\.*
note			{pitchbase}{alteration}*{octave}*{duration}*{ws}*[\~()\[\]]*
braced			\{[^\}]*\}
key				major|minor|ionian|dorian|phrygian|lydian|mixolydian|aeolian|locrian

%%
[\^_]\"\*\"{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
[\^_]\"\+\"{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
[\^_]\"[^\"]*\"{ws}*								{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
[\^_]\\markup{ws}*\{								{ BEGIN(INMARKUP); brace_count=1; /*clog << "INSTR  " << YYText() << endl; */ return 4; }
<INMARKUP>\{										{ brace_count++; return 4; }
<INMARKUP>\}										{ brace_count--; if (brace_count==0) BEGIN(INITIAL); return 4; }
<INMARKUP>[^\{\}]									{/* skip */ return 4; }
\\ficta{ws}*										{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\grace{ws}*\{[^\}]*\}{ws}*							{/*clog << "GRACE  " << YYText() << endl; */ return 5; }
[\^_]?\\staccato{ws}*								{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
[\^_]?\\staccatissimo{ws}*							{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
[\^_]?\\trill{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
[\^_]?\\fermata{ws}*								{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
[\^_]?\\upline{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
[\^_]?\\uplines{ws}*								{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
[\^_]?\\(prall)+{ws}*								{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
[\^_]?\\mordent{ws}*								{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
[\^_]\"\/\/\"{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\key{ws}+{pitchbase}{ws}*\\{key}{ws}*				{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\sb{ws}*											{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\bar{ws}*\"[^\"]*\"{ws}*							{/*clog << "INSTR  " << YYText() << endl; */ return 6; }
\\segno{ws}*										{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\coda{ws}*											{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\fine{ws}*											{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\dc[fc]?{ws}*										{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\ds[fc]?{ws}*										{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\~													return 7; /*clog << "TIE  " << YYText() << endl;*/
\}													return 8; /*clog << "CLOSINGBRACE " << YYText() << endl;*/
\{													return 9; /*clog << "OPENINGBRACE " << YYText() << endl;*/
({glis}|{cross}|\{)*{ws}*{note}{ws}*\}*				return 1; /*clog << "NOTE  " << YYText() << endl;*/
\\stopbar{ws}*										return 10; /*clog << "STOPBAR  " << YYText() << endl;*/
{time_command}{ws}*{digit}+"/"{digit}+{ws}*			return 2; /*clog << "TIME  " << YYText() << endl;*/
{times_command}{ws}*{digit}+"/"{digit}+{ws}*		return 3; /*clog << "TIMES " << YYText() << endl;*/ 
{ws}												{/*clog << "WHITESPACE" << endl; */ return -2; }
.													return -1; /*cerr << "Unrecognized character: "<< YYText() << endl;*/
%%

int Lilywrap(void) {
return 1;
}

