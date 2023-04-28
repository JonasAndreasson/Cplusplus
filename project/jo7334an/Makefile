# Define the compiler and the linker. The linker must be defined since
# the implicit rule for linking uses CC as the linker. g++ can be
# changed to clang++.
CXX = g++
CC  = $(CXX)

# Generate dependencies in *.d files
DEPFLAGS = -MT $@ -MMD -MP -MF $*.d

# Define preprocessor, compiler, and linker flags. Uncomment the # lines
# if you use clang++ and wish to use libc++ instead of GNU's libstdc++.
# -g is for debugging.
CPPFLAGS =  -Iinclude
CXXFLAGS =  -O2 -Wall -Wextra -pedantic-errors -Wold-style-cast 
CXXFLAGS += -std=c++11 
CXXFLAGS += -g
CXXFLAGS += $(DEPFLAGS)
LDFLAGS =   -g -Llib
#CPPFLAGS += -stdlib=libc++
#CXXFLAGS += -stdlib=libc++
#LDFLAGS +=  -stdlib=libc++

# Targets

all: src/client src/memserver src/diskserver
install: bin/client bin/memserver bin/diskserver

# Create the library; ranlib is for Darwin (OS X) and maybe other systems.
# Doesn't seem to do any damage on other systems.

src/client: src/client.o src/server.o src/connection.o
src/memserver: src/memserver.o src/connection.o src/server.o
src/diskserver: src/diskserver.o src/connection.o src/server.o
bin/client: src/client 
	-cp src/client bin/client
bin/memserver: src/memserver
	-cp src/memserver bin/memserver
bin/diskserver: src/diskserver
	-cp src/diskserver bin/diskserver
# Phony targets
.PHONY: all clean distclean

SRC = $(wildcard src/*.cc)

# Standard clean
clean:
	-rm $(SRC:.cc=.o) $(PROGS)

distclean: clean
	-rm lib/libclientserver.a
	-rmdir lib
	-rm $(SRC:.cc=.d) 
	make -C example distclean


# Include the *.d files
-include $(SRC:.cc=.d)
