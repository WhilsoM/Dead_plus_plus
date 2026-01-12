CXX = clang++
CXXFLAGS = -I/opt/homebrew/include -std=c++17
LDFLAGS = -L/opt/homebrew/lib -lsfml-graphics -lsfml-window -lsfml-system
TARGET = MyHorrorGame
SRC = main.cpp

all:
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS) $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
