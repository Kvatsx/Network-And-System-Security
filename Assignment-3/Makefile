CC = gcc
OBJFILES1 = setacl.o helper.o
OBJFILES2 = getacl.o helper.o
OBJFILES3 = ls.o helper.o
OBJFILES4 = fget.o helper.o
OBJFILES5 = fput.o helper.o
OBJFILES6 = create_dir.o helper.o
OBJFILES7 = do_exec.o helper.o



TARGET1 = setacl
TARGET2 = getacl
TARGET3 = ls
TARGET4 = fget
TARGET5 = fput
TARGET6 = create_dir
TARGET7 = do_exec


all: $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5) $(TARGET6) $(TARGET7)

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

$(TARGET6): $(OBJFILES6)
	$(CC) -o $(TARGET6) $(OBJFILES6)

$(TARGET7): $(OBJFILES7)
	$(CC) -o $(TARGET7) $(OBJFILES7)

clean:
	rm -f $(OBJFILES1) $(TARGET1) *~
	rm -f $(OBJFILES2) $(TARGET2) *~
	rm -f $(OBJFILES3) $(TARGET3) *~
	rm -f $(OBJFILES4) $(TARGET4) *~
	rm -f $(OBJFILES5) $(TARGET5) *~
	rm -f $(OBJFILES6) $(TARGET6) *~
	rm -f $(OBJFILES7) $(TARGET7) *~

