TARGET = MaquinaDeEscrever3D

SRC = MaquinaDeEscrever3D.cpp

SHADER_DIR = shaders

LIBS = -lGL -lGLEW -lGLU -lglut -lglfw -lglm

CXX = g++

CXXFLAGS = -std=c++11 -Wall -g

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: clean run