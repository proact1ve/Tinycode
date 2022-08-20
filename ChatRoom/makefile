CXX ?= g++

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS += -g
else
	CXXFLAGS += -O2
endif

server : server.cpp
		$(CXX) -o server $^ $(CXXFLAGS)

client : client.cpp
		$(CXX) -o client $^ $(CXXFLAGS)

clean:
	rm -r server client