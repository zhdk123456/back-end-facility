
# Where to find user code.
SRC_DIR = .
LIBNAME = libserver.a

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra

HEADER = ./include 
MISCHEADER = ./misc


all : $(LIBNAME)

thread.o : $(SRC_DIR)/src/thread.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(SRC_DIR)/src/thread.cc -I$(HEADER) -I$(MISCHEADER)

ITask.o : $(SRC_DIR)/src/ITask.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(SRC_DIR)/src/ITask.cc -I$(HEADER) -I$(MISCHEADER)
	
worker.o : $(SRC_DIR)/src/worker.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(SRC_DIR)/src/worker.cc -I$(HEADER) -I$(MISCHEADER)

daemon.o : $(SRC_DIR)/src/daemon.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(SRC_DIR)/src/daemon.cc -I$(HEADER) -I$(MISCHEADER)

OBJECTS = thread.o daemon.o worker.o ITask.o 

$(LIBNAME) : $(OBJECTS)
	ar r $(LIBNAME) $(OBJECTS)

#server : $(objects) 
	#$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $(objects) -o server


clean :
	rm -f $(LIBNAME) *.o
