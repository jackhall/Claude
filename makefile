CC = g++
CFLAGS = -std=c++11 -g 

demo/libfern.so : src/Fern.h src/Fern.cpp src/fernpy.cpp test/test_claude
	cd src; \
	$(CC) $(CFLAGS) -fPIC -I/usr/include/python2.7 -c fernpy.cpp
	$(CC) -shared -g -lpython2.7 -lboost_python -o demo/libfern.so src/fernpy.o

test/test_claude : src/Fern.h src/Fern.cpp test/test_claude.cpp
	cd test; \
	$(CC) $(CFLAGS) -I../src -lgtest -lpthread test_claude.cpp -o test_claude

clean :
	rm src/fernpy.o demo/libfern.so test/test_claude
	
#libClaude.so: $(OBJECTS)
#	gcc -g -shared -Wl,-soname,libClaude.so.1 -o libClaude.so.1.0 $(OBJECTS); \
#	ln -s libClaude.so.1.0 libClaude.so.1; \
#	ln -s libClaude.so.1 libClaude.so

#Node.o : Node.cpp Node.h
#	$(CC) $(CFLAGS) -c Node.cpp

#Fork.o : Fork.cpp Fork.h Node.h
#	$(CC) $(CFLAGS) -c Fork.cpp

#Leaf.o : Leaf.cpp Leaf.h Node.h Fork.h
#	$(CC) $(CFLAGS) -c Leaf.cpp

#Fern.o : Fern.cpp Node.h Fork.h Leaf.h Fern.h
#	$(CC) $(CFLAGS) -c Fern.cpp

#clean :
#	rm $(OBJECTS); \
#	rm libClaude.*
	
#install :
#	sudo mv libClaude.so /usr/lib64/.; \
#	sudo mv libClaude.so.1.0 /usr/lib64/.; \
#	sudo ldconfig

#remove : 
#	rm $(OBJECTS); \
#	rm libClaude.*; \
#	sudo rm /usr/lib64/libClaude.*; \
#	sudo ldconfig
	
