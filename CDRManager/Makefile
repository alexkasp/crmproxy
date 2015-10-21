	PROXYHEADERS = ./crmproxy
	RECALLHEADERS = ./recall
	MONITORHEADERS = ./Monitor
	BUILDHEADERS = -I ../include -I /usr/include/mysql++ -I /usr/include/mysql
default:
	export CFLAGS="$CFLAGS -O0 -fbuiltin -g"
	export CXXFLAGS="$CXXFLAGS -O0 -fbuiltin -g"
	
	g++ -c CDRManager.cpp -std=c++11 ${BUILDHEADERS} -L /usr/local/boost/lib/ -L /usr/lib64/ -L /usr/lib64/mysql/ \
	-Wl,-Bstatic -lboost_system -lboost_regex  -lboost_thread -lboost_date_time -lboost_log -lboost_log_setup -lboost_filesystem -Wl,-Bdynamic -lm -lcurl -lpthread -lrt -lmysqlpp -lmysqlclient
test:
	g++ test.cpp corefunc.cpp sipheader.cpp sipmanager.cpp callbox.cpp -o test  -I ../include -I /usr/local/boost/include/ -L /usr/local/boost/lib/ -Wl,-Bstatic  -lboost_system -lboost_regex -Wl,-Bdynamic -lm
	
thread:
	g++ thread.cpp -o thread -std=c++0x -I /usr/local/boost/include/ -L /usr/local/boost/lib/ -Wl,-Bstatic  -lboost_system -lboost_regex -lboost_thread -Wl,-Bdynamic -lm -lpthread -lrt
	
client:
	g++ simple_client.cpp -o simple -std=c++0x -I /usr/local/boost/include/ -L /usr/local/boost/lib/ -Wl,-Bstatic  -lboost_system -lboost_regex -lboost_thread -Wl,-Bdynamic -lm -lpthread -lrt
	
iptable:
	g++ iplist.cpp iptest.cpp -o iplist -std=c++0x -I /usr/local/boost/include/ -L /usr/local/boost/lib/ -Wl,-Bstatic  -lboost_system -lboost_regex -Wl,-Bdynamic -lm -lpthread -lrt
	
logs:
	g++ messagelog.cpp corefunc.cpp makelogs.cpp filemanager.cpp storagemanager.cpp sipheader.cpp sipmanager.cpp callbox.cpp iplist.cpp  -o logs -lpthread -std=c++0x -I /usr/local/boost/include/ -L /usr/local/boost/lib/ -Wl,-Bstatic  -lboost_system -lboost_regex -lboost_date_time -Wl,-Bdynamic -lm

serv:
	g++ simpleserv.cpp -o serv -std=c++0x -I /usr/local/boost/include/ -L /usr/local/boost/lib/ -Wl,-Bstatic -lboost_thread -lboost_system -lboost_regex -Wl,-Bdynamic -lm -lpthread -lrt

all:
	export CFLAGS="$CFLAGS -O0 -fbuiltin -g"
	export CXXFLAGS="$CXXFLAGS -O0 -fbuiltin -g"
	
	${MAKE} -C ${PROXYHEADERS}
	${MAKE} -C ${RECALLHEADERS}
	${MAKE} -C ${MONITORHEADERS}
clean:
	rm -f ${PROXYHEADERS}/*.o ${RECALLHEADERS}/*.o ${MONITORHEADERS}/*.o ./main