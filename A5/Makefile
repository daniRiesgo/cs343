# the compiler: gcc for C program, define as g++ for C++
CC = u++
KIND = NPAD

# compiler flags:
CFLAGS  = -O2 -multi -nodebug -D$(KIND)

# the build target executable:
TARGET = q1
EXEC = c$(KIND)

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)
