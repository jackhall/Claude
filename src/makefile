CC = g++
CFLAGS = -std=c++11 -fPIC -g -I /usr/include/python2.7

libfern.so: Fern.h Fern.cpp fernpy.cpp
	$(CC) $(CFLAGS) -c fernpy.cpp
	$(CC) -g -shared -lpython2.7 -lboost_python -o fernpy.so fernpy.o

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
	
