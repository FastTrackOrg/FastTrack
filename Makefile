# Standard
CC = g++
CFLAGS = -std=c++11 -Wall -Wextra -fdiagnostics-color=always -O0
IDIR= ./
SRC= $(IDIR)functions.cpp Hungarian.cpp

# compile
all: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o build/main -I /usr/include/opencv -L /usr/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -lopencv_video

