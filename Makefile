all:
	/bin/sh ./build.sh
clean:
	/bin/sh ./build.sh clean
install:
	/bin/sh ./build.sh install
release: 
	/bin/sh ./build.sh release 

