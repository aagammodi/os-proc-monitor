CXX = g++
CXXFLAGS = -std=c++17 -Iinclude

TARGET = proc_monitor
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) -pthread

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

run: $(TARGET)
	./$(TARGET)

live: $(TARGET)
	./$(TARGET) -T 2
