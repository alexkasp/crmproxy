	PROXYHEADERS = ./crmproxy
	RECALLHEADERS = ./recall
	MONITORHEADERS = ./Monitor
	LOGGER = ./logger
	ICMSERVER = ./icmserver
	CDRMANAGER = ./CDRManager
	FSPROXY = ./fsproxy
	TESTHEADERS = ./test
	BUILDHEADERS = -I ./include -I /usr/include/mysql++ -I /usr/include/mysql -I /usr/local/include/hiredis
default:
	export CFLAGS="$CFLAGS -O0 -fbuiltin -g"
	export CXXFLAGS="$CXXFLAGS -O0 -fbuiltin -g"
	
	echo ${MAKE} -C ${PROXYHEADERS}
	echo ${MAKE} -C ${RECALLHEADERS}
	echo ${MAKE} -C ${MONITORHEADERS}
	echo ${MAKE} -C ${LOGGER}
	echo ${MAKE} -C ${ICMSERVER}
	echo ${MAKE} -C ${CDRMANAGER}
	g++ -g ${PROXYHEADERS}/*.o ${ICMSERVER}/*.o ${CDRMANAGER}/*.o ${LOGGER}/*.o ${RECALLHEADERS}/*.o ${BUILDHEADERS} ${MONITORHEADERS}/*.o  main.cpp -o main iexecuter.cpp  -std=c++11 -L /usr/local/boost/lib/ -L /usr/lib64/ -L /usr/lib64/mysql/ \
	-Wl,-Bstatic -lhiredis -lboost_system -lboost_random -lboost_regex  -lboost_thread -lboost_date_time -lboost_log -lboost_log_setup -lboost_filesystem -lboost_timer -lboost_chrono -Wl,-Bdynamic -lpthread -lm -lcurl -lrt -lmysqlpp -lmysqlclient
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
	${MAKE} -C ${LOGGER}
	${MAKE} -C ${ICMSERVER}
	${MAKE} -C ${CDRMANAGER}
	${MAKE} -C ${FSPROXY}
clean:
	rm -f ${CDRMANAGER}/*.o ${ICMSERVER}/*.o ${PROXYHEADERS}/*.o ${RECALLHEADERS}/*.o ${MONITORHEADERS}/*.o ${LOGGER}/*.o ${FSPROXY}/*.o ./fsmain ./main
cleanfs:
	rm -f ${FSPROXY}/*.o ./fsmain
fsproxy:
	export CFLAGS="$CFLAGS -O0 -fbuiltin -g"
	export CXXFLAGS="$CXXFLAGS -O0 -fbuiltin -g"
	
	g++ -g ${FSPROXY}/*.o ${PROXYHEADERS}/*.o ${ICMSERVER}/*.o ${CDRMANAGER}/*.o ${LOGGER}/*.o ${RECALLHEADERS}/*.o ${BUILDHEADERS} ${MONITORHEADERS}/*.o  fsmain.cpp -o fsmain iexecuter.cpp  -std=c++11 -L /usr/local/boost/lib/ -L /usr/lib64/ -L /usr/lib64/mysql/ \
	-Wl,-Bstatic -lboost_system -lboost_random -lboost_regex  -lboost_thread -lboost_date_time -lboost_log -lboost_log_setup -lboost_filesystem -Wl,-Bdynamic -lpthread -lm -lcurl -lrt -lmysqlpp -lmysqlclient
fsproxytest:
	export CFLAGS="$CFLAGS -O0 -fbuiltin -g"
	export CXXFLAGS="$CXXFLAGS -O0 -fbuiltin -g"
	
	g++ -g ${FSPROXY}/*.o ${PROXYHEADERS}/*.o ${ICMSERVER}/*.o ${CDRMANAGER}/*.o ${LOGGER}/*.o ${RECALLHEADERS}/*.o ${BUILDHEADERS} ${MONITORHEADERS}/*.o fsmain.cpp -o fsmain iexecuter.cpp  -std=c++11 -L ./fsproxy/ -L /usr/local/boost/lib/ -L /usr/lib64/ -L /usr/lib64/mysql/ \
	-Wl,-Bstatic -l esl -lhiredis -lboost_system -lboost_random -lboost_regex  -lboost_thread -lboost_date_time -lboost_log -lboost_log_setup -lboost_filesystem  -Wl,-Bdynamic -lpthread -lmysqlpp -lm -lcurl -lrt -lmysqlclient
	
wssmonitor:
	export CFLAGS="$CFLAGS -O0 -fbuiltin -g"
	export CXXFLAGS="$CXXFLAGS -O0 -fbuiltin -g"
	
	g++ -g ${FSPROXY}/*.o ${PROXYHEADERS}/*.o ${ICMSERVER}/*.o ${CDRMANAGER}/*.o ${LOGGER}/*.o ${RECALLHEADERS}/*.o ${BUILDHEADERS} ${MONITORHEADERS}/*.o wsmain.cpp -o wsmain iexecuter.cpp  -std=c++11 -L ./fsproxy/ -L /usr/local/boost/lib/ -L /usr/lib64/ -L /usr/lib64/mysql/ \
	-Wl,-Bstatic -l esl -lboost_system -lboost_random -lboost_regex  -lboost_thread -lboost_date_time -lboost_log -lboost_log_setup -lboost_filesystem -Wl,-Bdynamic -lpthread -lm -lcurl -lrt -lmysqlpp -lmysqlclient
	
teststarter:
	export CFLAGS="$CFLAGS -O0 -fbuiltin -g"
	export CXXFLAGS="$CXXFLAGS -O0 -fbuiltin -g"
	
	g++ -g ${RECALLHEADERS}/*.o ${BUILDHEADERS} ${PROXYHEADERS}/*.o ${LOGGER}/*.o ${ICMSERVER}/*.o ${CDRMANAGER}/*.o  teststarter.cpp -o teststart iexecuter.cpp -std=c++11 -L ./fsproxy/ -L /usr/local/boost/lib/ -L /usr/lib64/ -L /usr/lib64/mysql/ \
	-Wl,-Bstatic -l esl -lboost_system -lboost_random -lboost_regex  -lboost_thread -lboost_date_time -lboost_log -lboost_log_setup -lboost_filesystem -Wl,-Bdynamic -lpthread -lm -lcurl -lrt -lmysqlpp -lmysqlclient
	
autotest:
	export CFLAGS="$CFLAGS -O0 -fbuiltin -g"
	export CXXFLAGS="$CXXFLAGS -O0 -fbuiltin -g"
	
	g++ -g ${TESTHEADERS}/*.o ${LOGGER}/*.o ${ICMSERVER}/*.o ${PROXYHEADERS}/*.o ${RECALLHEADERS}/*.o ${CDRMANAGER}/*.o ${BUILDHEADERS}  autotest.cpp -o autotest iexecuter.cpp -std=c++11 -L ./fsproxy/ -L /usr/local/boost/lib/ -L /usr/lib64/ -L /usr/lib64/mysql/ \
	-Wl,-Bstatic -l esl -lboost_system -lboost_random -lboost_regex  -lboost_thread -lboost_date_time -lboost_log -lboost_log_setup -lboost_filesystem -Wl,-Bdynamic -lpthread -lm -lcurl -lrt -lmysqlpp -lmysqlclient
