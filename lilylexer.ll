%option c++
%option prefix="Lily"
%option noyywrap
%x INMARKUP
%{
int brace_count = 0;
%}

digit			[0-9]
pitchbase		(as)|(es)|[a-grs]
alteration		((is)|(es))(\?|!)?
octave			[',]
ws				[| \t\n]+
time_command	\\time|\\itime
times_command	\\times
glis			\\gl
cross			\\x
ficta			\\ficta
duration		{digit}+\.*
note			{pitchbase}{alteration}*{octave}*{duration}?{ws}*[\~()\[\]]*
chordnote		{pitchbase}{alteration}*{octave}*
chord			\<({ws}*{chordnote}{ws}*)+\>{ws}*{duration}?{ws}*[\~()\[\]]*
braced			\{[^\}]*\}
mode			major|minor|ionian|dorian|phrygian|lydian|mixolydian|aeolian|locrian
clef_command	\\clef
clef_type		treble|bass
tekst			[\^_]\"[^\"]*\"{ws}*

%%
[\^_]\"\+\"{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 31; }
[\^_]\"-\"{ws}*										{/*clog << "INSTR  " << YYText() << endl; */ return 32; }
[\^_]\\markup{ws}*\{								{ BEGIN(INMARKUP); brace_count=1; /*clog << "INSTR  " << YYText() << endl; */ return 4; }
<INMARKUP>\{										{ brace_count++; return 4; }
<INMARKUP>\}										{ brace_count--; if (brace_count==0) BEGIN(INITIAL); return 4; }
<INMARKUP>[^\{\}]									{/* skip */ return 4; }
\(													{ return 13; }
\)													{ return 14; }
\[													{/* skip */ return 4; }
\]													{/* skip */ return 4; }
\\grace{ws}*\{[^\}]*\}{ws}*							{/*clog << "GRACE  " << YYText() << endl; */ return 5; }
\\app{ws}*\{[^\}]*\}{ws}*							{/*clog << "GRACE  " << YYText() << endl; */ return 5; }
\\kvs{ws}*\{[^\}]*\}{ws}*							{/*clog << "GRACE  " << YYText() << endl; */ return 5; }
\\afterGrace										{ return 16; }
{braced}											{ return 18; }
[\^_]?\\staccato{ws}*								{/*clog << "INSTR  " << YYText() << endl; */ return 27; }
[\^_]?\\staccatissimo{ws}*							{/*clog << "INSTR  " << YYText() << endl; */ return 28; }
-\.{ws}*											{/*clog << "INSTR  " << YYText() << endl; */ return 27; }
[\^_]?\\trill{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 21; }
[\^_]?\\fermata{ws}*								{/*clog << "INSTR  " << YYText() << endl; */ return 29; }
[\^_]?\\(prall){2}{ws}*								{/*clog << "INSTR  " << YYText() << endl; */ return 23; }
[\^_]?\\(prall){1}{ws}*								{/*clog << "INSTR  " << YYText() << endl; */ return 22; }
[\^_]?\\mordent{ws}*								{/*clog << "INSTR  " << YYText() << endl; */ return 24; }
[\^_]?\\turn{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 25; }
[\^_]?\\accent{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 30; }
[\^_]\"\/\/\"{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 26; }
\\key{ws}+{pitchbase}{alteration}*{ws}*\\{mode}{ws}*	{ return 19; }
[\^_]?\\segno{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 34; }
[\^_]?\\coda{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 35; }
[\^_]?\\fine{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 36; }
[\^_]?\\dc{ws}*										{/*clog << "INSTR  " << YYText() << endl; */ return 37; }
[\^_]?\\dcf{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 38; }
[\^_]?\\dcc{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 39; }
[\^_]?\\ds{ws}*										{/*clog << "INSTR  " << YYText() << endl; */ return 40; }
[\^_]?\\dsf{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 41; }
[\^_]?\\dsc{ws}*									{/*clog << "INSTR  " << YYText() << endl; */ return 42; }
\\pv{ws}*											return 43;
\\sv{ws}*											return 44;
\\tv{ws}*											return 45;
\\qv{ws}*											return 46;
\\xv{ws}*											return 47;
\\p+{ws}*											{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\f+{ws}*											{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\mf{ws}*											{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\mp{ws}*											{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\sfz{ws}*											{/*clog << "INSTR  " << YYText() << endl; */ return 4; }
\\\<{ws}*											return 4; /*clog << "BEGINCRESC  " << YYText() << endl;*/
\\\>{ws}*											return 4; /*clog << "BEGINDECRESC  " << YYText() << endl;*/
\\!{ws}*											return 4; /*clog << "ENDCRESC  " << YYText() << endl;*/
\\sb{ws}*											return 11;
\\bar{ws}*\"[^\"]*\"{ws}*							return 6;
~													return 7; /*clog << "TIE  " << YYText() << endl;*/
\}													return 8; /*clog << "CLOSINGBRACE " << YYText() << endl;*/
\{													return 9; /*clog << "OPENINGBRACE " << YYText() << endl;*/
    /*This captures any text. Make sure it is AFTER rules for ornaments and other elements coded as text */
[\^_]\"[^\"]*\"{ws}*								{/*clog << "INSTR  " << YYText() << endl; */ return 33; }
({ficta}{ws}*|{glis}{ws}*|{cross}{ws}*|\{{ws}*)*{note}{ws}*\}*		return 1; /*clog << "NOTE  " << YYText() << endl;*/
({ficta}{ws}*|{glis}{ws}*|{cross}{ws}*|\{{ws}*)*{chord}{ws}*\}*	return 12; /*clog << "CHORD " << YYText() << endl;*/
{time_command}{ws}*{digit}+"/"{digit}+{ws}*			return 2; /*clog << "TIME  " << YYText() << endl;*/
{times_command}{ws}*{digit}+"/"{digit}+{ws}*\{		return 3; /*clog << "TIMES " << YYText() << endl;*/
{clef_command}{ws}*\"?{clef_type}\"?{ws}*			return 15; /*clog << "CLEF " << YYText() << endl;*/
{ws}												{/*clog << "WHITESPACE" << endl; */ return -2; }
.													return -1; /*cerr << "Unrecognized character: "<< YYText() << endl;*/
%%

int Lilywrap(void) {
return 1;
}

