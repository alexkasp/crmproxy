default:
	export CFLAGS="$CFLAGS -O0 -fbuiltin -g"
	export CXXFLAGS="$CXXFLAGS -O0 -fbuiltin -g"
	
	g++ -c RecallManager.cpp RecallParser.cpp -lpthread -std=c++0x -I ../include	
clean:
	rm -f ./*.o