CC = gcc
OBJFILES1 = setacl.o helper.o
OBJFILES2 = getacl.o helper.o
OBJFILES3 = ls.o helper.o
OBJFILES4 = fget.o helper.o
OBJFILES5 = fput.o helper.o


TARGET1 = setacl
TARGET2 = getacl
TARGET3 = ls
TARGET4 = fget
TARGET5 = fput


all: $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5)

$(TARGET1): $(OBJFILES1)
	$(CC) -o $(TARGET1) $(OBJFILES1)

$(TARGET2): $(OBJFILES2)
	$(CC) -o $(TARGET2) $(OBJFILES2)

$(TARGET3): $(OBJFILES3)
	$(CC) -o $(TARGET3) $(OBJFILES3)

$(TARGET4): $(OBJFILES4)
	$(CC) -o $(TARGET4) $(OBJFILES4)

$(TARGET5): $(OBJFILES5)
	$(CC) -o $(TARGET5) $(OBJFILES5)

clean:
	rm -f $(OBJFILES1) $(TARGET1) *~
	rm -f $(OBJFILES2) $(TARGET2) *~
	rm -f $(OBJFILES3) $(TARGET3) *~
	rm -f $(OBJFILES4) $(TARGET4) *~
	rm -f $(OBJFILES5) $(TARGET5) *~

