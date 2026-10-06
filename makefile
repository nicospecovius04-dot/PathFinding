CXX = clang++

CXXFLAGS = -Wall -Wextra -std=c++23 -IC:\msys64\ucrt64\include -Iinclude

LDFLAGS = -LC:\msys64\ucrt64\lib -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp,build/%.o,$(SRC))

TARGET = build/main.exe

.PHONY: all clean run format

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./build/main.exe

format:
	@clang-format -i src/*.cpp include/*.hpp
	@git diff -- src/

clean:
	rm -f build/*.o build/*.exe