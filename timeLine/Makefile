all: timeline_test

time_test_objs := main.o  timeline.o
include_header := timeline.h spsc_queue.h
CXXFLAGS += -std=c++14 -O0 -g
LINKFLAGS += -lpthread -lglog

timeline_test: $(time_test_objs)
	g++ $^ -o $@ -g $(LINKFLAGS)



%.o:%.cpp $(include_header)
	g++ -c $< -o $@ $(CXXFLAGS)
clean:
	rm -rf $(time_test_objs) timeline_test