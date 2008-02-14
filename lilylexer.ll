
%option c++
%option prefix="Lily"

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
note			{pitchbase}{alteration}*{octave}*{duration}*{ws}*[\~()]*

%%
({glis}|{ws}|{cross}|\{)*{note}{ws}*\}*{ws}			return 1; /*clog << "NOTE  " << YYText() << endl;*/
{time_command}{ws}*{digit}+"/"{digit}+{ws}			return 2; /*clog << "TIME  " << YYText() << endl;*/
{times_command}{ws}*{digit}+"/"{digit}+{ws}			return 3; /*clog << "TIMES " << YYText() << endl;*/ 
.													return -1; /*cerr << "Unrecognized character: "<< YYText() << endl;*/
%%

int Lilywrap(void) {
return 1;
}

