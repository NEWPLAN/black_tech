all: time_test

time_test_objs := time_test.o 
include_header := time_event.h time_record.h
CXXFLAGS += -std=c++2a -Os
LINKFLAGS += -lpthread 

time_test: $(time_test_objs)
	g++ $^ -o $@ -g $(LINKFLAGS)



%.o:%.cpp $(include_header)
	g++ -c $< -o $@ $(CXXFLAGS)
clean:
	rm -rf $(time_test_objs) time_test
