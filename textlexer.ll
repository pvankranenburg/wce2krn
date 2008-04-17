%option c++
%option prefix="Text"
%option noyywrap

ext		--
ws		[ \t\n]+
word	[^ \t\n]+|(\"[^\"]*\")
syl		{word}[ ]*{ext}*

%%
{syl}					{ /* clog << "WORD: " << YYText() << endl;         */ return 1; }
{ws} 					{ /* clog << "WHITESPACE" << YYText() << endl;     */ return 2; }
.						{ /* clog << "Unrecognized: " << YYText() << endl; */ return -1; }
%%

int Textwrap(void) {
return 1;
}


