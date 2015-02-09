CXX=g++
CXXFLAGS += -g -I. -I dependency/simple_log/include -I src

.PHONY : all test clean

TEST_DEPS_LIBS = bin/lib/libtestudo.a dependency/simple_log/lib/libsimplelog.a 

objects := $(patsubst %.cc,%.o,$(wildcard src/*.cc))

all: prepare $(objects) package
	
test: data_storage_test	

prepare: 
	mkdir -p bin/include bin/lib
	
package:
	mv src/*.o bin/
	ar -rcs libtestudo.a bin/*.o
	
	cp src/*.h bin/include/
	mv libtestudo.a bin/lib/
	rm -rf bin/*.o

%.o: %.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@
	
data_storage_test: test/data_storage_test.cc
	$(CXX) $(CXXFLAGS) $< $(TEST_DEPS_LIBS) -o bin/$@

clean:
	rm -rf bin/*

	