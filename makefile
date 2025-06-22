CXX = g++
CXXFLAGS = -O2 -std=c++17

SRC = src/main.cpp
OUT = main.exe

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(OUT)