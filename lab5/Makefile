processScheduling :  main.cpp  header.h process.o scheduler.o event.o eventManager.o clock.o 
	g++ main.cpp asgn5.cpp scheduler.o clock.o process.o event.o eventManager.o -o processScheduling  -g

scheduler.o: scheduler.cpp scheduler.h header.h eventManager.h clock.h process.h	
		g++ -c scheduler.cpp  -g

eventManager.o :eventManager.h eventManager.cpp header.h process.h  scheduler.h clock.h
		g++ -c eventManager.cpp -g
event.o: event.h event.cpp header.h clock.h process.h
		g++ -c event.cpp -g
clock.o: clock.h clock.cpp
		g++ -c clock.cpp -g
process.o: process.h process.cpp eventManager.h header.h scheduler.h clock.h 
	g++ -c process.cpp -g

clean:
	rm -rf *.o processScheduling
