LIB_SRC_FILES = workthreadpool.cpp lckfree.cpp
TEST_SRC_FILES = main.cpp
INCLUDE_DIR = ./
STD_FLAG = -std=c++0x 
 
all: main.o libs
	g++ $(STD_FLAG) -o test_workthreadpool main.o libworkthreadpool.so -lpthread -L . -Wl,-rpath .

main.o: $(TEST_SRC_FILES)
	g++ $(STD_FLAG) -c $(TEST_SRC_FILES) -I$(INCLUDE_DIR)

libs: $(LIB_SRC_FILES)
	g++ $(STD_FLAG) -o libworkthreadpool.so -fPIC -O2 -shared -Wl,--no-as-needed -I$(INCLUDE_DIR) $(LIB_SRC_FILES) -lpthread

.PHONY : clean
clean:
	rm -f test_workthreadpool main.o libworkthreadpool.so
