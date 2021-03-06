.PHONY : all test clean tags deps

CXX=g++
CXXFLAGS += -g -I. -I deps/simple_log/output/include -I src
LDFLAGS += -pthread

TEST_DEPS_LIBS = bin/lib/libtestudo.a deps/simple_log/output/lib/libsimplelog.a 

objects := $(patsubst %.cc,%.o,$(wildcard src/*.cc))

all: prepare deps $(objects) package

deps:
	make -C deps/simple_log
	
test: data_storage_test	format_data_test

prepare: 
	mkdir -p bin/include bin/lib

%.o: %.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@
	
data_storage_test: package test/data_storage_test.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) test/data_storage_test.o $(TEST_DEPS_LIBS) -o bin/$@

format_data_test: package test/format_data_test.o
	$(CXX) $(CXXFLAGS) test/format_data_test.o $(TEST_DEPS_LIBS) -o bin/$@

package: $(objects)
	cp src/*.o bin/
	ar -rcs libtestudo.a bin/*.o
	
	cp src/*.h bin/include/
	mv libtestudo.a bin/lib/
	rm -rf bin/*.o

tags:
	ctags -R src

clean:
	rm -rf src/*.o
	rm -rf test/*.o
	rm -rf bin/*

	
