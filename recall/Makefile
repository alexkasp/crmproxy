default:
	export CFLAGS="$CFLAGS -O0 -fbuiltin -g"
	export CXXFLAGS="$CXXFLAGS -O0 -fbuiltin -g"
	
	g++ -g -c astmanager.cpp RecallManager.cpp RecallParser.cpp -lpthread -std=c++0x -I ../include -I /usr/local/boost/include
clean:
	rm -f ./*.o