
CC=gcc
TARGET=ddt
SRCFILES= ddt.c teste.c teste1.c
INCFILES= teste.h teste1.h struct_no.h
OBJFILES= ddt.o teste.o teste1.o

default: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) -o $(TARGET) $(OBJFILES) -lm

clean:
	rm -f $(TARGET) $(OBJFILES) core.* #*

ddt.o: ddt.c

teste.o: teste.c teste.h

teste1.o: teste1.c teste1.h

