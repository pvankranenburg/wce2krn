%option c++
%option prefix="Text"

ext		--
ws		[ \t\n]+
word	[^ \t\n]+|(\".*\")
syl		{word}[ ]*{ext}*

%%
{syl}					clog << "WORD: " << YYText() << endl;
{ws} 					
.						clog << "Unrecognized: " << YYText() << endl;
%%

int Textwrap(void) {
return 1;
}


