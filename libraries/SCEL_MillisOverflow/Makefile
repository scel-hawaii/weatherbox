all: overflow_tester

overflow_tester: tester.o overflow_checker.o
	g++ tester.o overflow_checker.o -o overflow_tester

overflow_tester.o: tester.cpp overflow_checker.cpp overflow_checker.h
	g++ -c tester.cpp overflow_checker.cpp overflow_checker.h

millis_overflow.o: overflow_checker.cpp overflow_checker.h
	g++ -c overflow_checker.cpp overflow_checker.h

clean: 
	rm -rf *o overflow_tester
