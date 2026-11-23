# Optional: build with "make" (or "mingw32-make" on Windows).

CC      = gcc
CFLAGS  = -Wall -Wextra -Isrc
SRC     = $(wildcard src/*.c)

ifeq ($(OS),Windows_NT)
    TARGET = BusDepot.exe
    LIBS   = -lraylib -lopengl32 -lgdi32 -lwinmm
else
    TARGET = BusDepot
    LIBS   = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

$(TARGET): $(SRC) $(wildcard src/*.h)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.dat

.PHONY: run clean
