all: links overflow_tester

overflow_tester: overflow_tester.o overflow_checker.o
	g++ overflow_tester.o overflow_checker.o -o overflow_tester

overflow_tester.o: overflow_tester.cpp overflow_checker.cpp overflow_checker.h
	g++ -c overflow_tester.cpp overflow_checker.cpp overflow_checker.h

millis_overflow.o: overflow_checker.cpp overflow_checker.h
	g++ -c overflow_checker.cpp overflow_checker.h

links:
	ln -fs ../../overflow_checker.cpp overflow_checker.cpp
	ln -fs ../../overflow_checker.h overflow_checker.h

clean: 
	rm -rf *o overflow_tester
