%option c++
%option prefix="ChordNote"
%option noyywrap

digit			[0-9]
pitchbase		(as)|(es)|[a-grs]
alteration		[(is)|(es)]!?
octave			[',]
ws				[| \t\n]+
time_command	(\\itime)?{ws}*\\time
times_command	\\times
glis			\\gl
cross			\\x
duration		{digit}+\.*
note			{pitchbase}{alteration}*{octave}*{duration}?{ws}*[\~()\[\]]*
chordnote		{pitchbase}{alteration}*{octave}*
chord			\<({ws}*{chordnote}{ws}*)+\>{ws}*{duration}?{ws}*[\~()\[\]]*
braced			\{[^\}]*\}
key				major|minor|ionian|dorian|phrygian|lydian|mixolydian|aeolian|locrian

%%
{chordnote}											{ return 2; }
.													return -1; /*cerr << "Unrecognized character: "<< YYText() << endl;*/
%%

int ChordNotewrap(void) {
return 1;
}

