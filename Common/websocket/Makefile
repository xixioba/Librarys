OBJS=websocket.o base64.o sha1.o network.o
all:$(OBJS)
%.o:%.cpp
	-$(CXX) -c $^ -o ../$(OBJ_DIR)/$@ -I . -std=c++11 