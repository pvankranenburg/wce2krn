##########################################
#
# UNIVERSAL EXECUTABLE
#

FLAGS=-g

wce2krn :  RationalTime_i386.o \
           RelLyToken_i386.o \
		   Song_i386.o \
		   SongLine_i386.o \
		   TimeSignature_i386.o \
		   WCE_File_i386.o \
		   lex.Lily_i386.o \
		   lex.Text_i386.o \
		   pvkutilities_i386.o \
		   wce2krn_i386.o
	g++ $(FLAGS) -o wce2krn RationalTime_i386.o \
	               RelLyToken_i386.o \
				   Song_i386.o \
				   SongLine_i386.o \
				   TimeSignature_i386.o \
				   WCE_File_i386.o \
				   lex.Lily_i386.o \
				   lex.Text_i386.o \
				   pvkutilities_i386.o \
				   wce2krn_i386.o

##########################################
#
# i386 OBJECT FILES
#

wce2krn_i386.o : wce2krn.cpp wce2krn.h
	g++ $(FLAGS) -c -o wce2krn_i386.o wce2krn.cpp 

RationalTime_i386.o : RationalTime.cpp RationalTime.h
	g++ $(FLAGS) -c -o RationalTime_i386.o RationalTime.cpp 

RelLyToken_i386.o : RelLyToken.cpp RelLyToken.h
	g++ $(FLAGS) -c -o RelLyToken_i386.o RelLyToken.cpp 

Song_i386.o : Song.cpp Song.h
	g++ $(FLAGS) -c -o Song_i386.o Song.cpp 

SongLine_i386.o : SongLine.cpp SongLine.h
	g++ $(FLAGS) -Wno-deprecated -c -o SongLine_i386.o SongLine.cpp 

TimeSignature_i386.o : TimeSignature.cpp TimeSignature.h
	g++ $(FLAGS) -c -o TimeSignature_i386.o TimeSignature.cpp 

WCE_File_i386.o : WCE_File.cpp WCE_File.h
	g++ $(FLAGS) -c -o WCE_File_i386.o WCE_File.cpp 

pvkutilities_i386.o : pvkutilities.cpp pvkutilities.h
	g++ $(FLAGS) -c -o pvkutilities_i386.o pvkutilities.cpp 

lex.Lily_i386.o : lex.Lily.cc
	g++ $(FLAGS) -Wno-deprecated -c -o lex.Lily_i386.o lex.Lily.cc 

lex.Text_i386.o : lex.Text.cc
	g++ $(FLAGS) -Wno-deprecated -c -o lex.Text_i386.o lex.Text.cc 


##########################################
#
# FLEX
#

lex.Lily.cc : lilylexer.ll
	flex lilylexer.ll

lex.Text.cc : textlexer.ll
	flex textlexer.ll

##########################################
#
# INSTALL 
#

install: wce2krn
	strip wce2krn
	cd ~/SVN/music/binary/mac/universal/bin; svn update wce2krn
	cp wce2krn ~/SVN/music/binary/mac/universal/bin
	cd ~/SVN/music/binary/mac/universal/bin; svn commit -m "wce2krn mac binary" wce2krn

##########################################
#
# RELEASE 
#

release: install
	cd ../wce2krn; svn update
	cp *.cpp ../wce2krn
	cp *.h ../wce2krn
	cp *.ll ../wce2krn
	cp Makefile ../wce2krn
	cp CHANGELOG TODO ../wce2krn
	cd ../wce2krn; svn commit -m "wce2krn update"

##########################################
#
# CLEAN
#

clean :
	rm *.o \
	lex.Lily.cc \
	lex.Text.cc \
	wce2krn

