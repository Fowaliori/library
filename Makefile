CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -fPIC -pthread


all: lib app

lib: logger/Logger.cpp
	$(CXX) $(CXXFLAGS) -shared -o liblogger.so $<

app: app/main.cpp lib
	$(CXX) $(CXXFLAGS) -o logger_app app/main.cpp -L. -llogger -pthread

run: app
	LD_LIBRARY_PATH=. ./logger_app mylog.txt INFO

clean:
	rm -f liblogger.so logger_app mylog.txt
