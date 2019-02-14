all:ws

ws:websk.o base64.o sha1.o network.o
	g++ -o $@ $^ -lwsock32 -Wl,-rpath=.
	-make clean
%.o:%.cpp
	-g++ -c $< -I . -std=c++11
	
.PHONY : clean
clean:
	-rm *.o