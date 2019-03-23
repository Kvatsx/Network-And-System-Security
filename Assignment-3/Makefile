# CC = gcc
# OBJFILES1 = setacl.o helper.o
# OBJFILES2 = getacl.o helper.o
# OBJFILES3 = ls.o helper.o
# OBJFILES4 = fget.o helper.o
# OBJFILES5 = fput.o helper.o
# OBJFILES6 = create_dir.o helper.o
# OBJFILES7 = do_exec.o helper.o



# TARGET1 = setacl
# TARGET2 = getacl
# TARGET3 = ls
# TARGET4 = fget
# TARGET5 = fput
# TARGET6 = create_dir
# TARGET7 = do_exec



# all: $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5) $(TARGET6) $(TARGET7)

# $(TARGET1): $(OBJFILES1)
# 	$(CC) -o $(TARGET1) $(OBJFILES1)

# $(TARGET2): $(OBJFILES2)
# 	$(CC) -o $(TARGET2) $(OBJFILES2)

# $(TARGET3): $(OBJFILES3)
# 	$(CC) -o $(TARGET3) $(OBJFILES3)

# $(TARGET4): $(OBJFILES4)
# 	$(CC) -o $(TARGET4) $(OBJFILES4)

# $(TARGET5): $(OBJFILES5)
# 	$(CC) -o $(TARGET5) $(OBJFILES5)

# $(TARGET6): $(OBJFILES6)
# 	$(CC) -o $(TARGET6) $(OBJFILES6)

# $(TARGET7): $(OBJFILES7)
# 	$(CC) -o $(TARGET7) $(OBJFILES7)

# test:
# 	gcc check.c -g -lcrypto -Wall
# 	sudo chown root:root a.out
# 	sudo chmod u+s a.out
# 	./a.out

# clean:
# 	rm -f $(OBJFILES1) $(TARGET1) *~
# 	rm -f $(OBJFILES2) $(TARGET2) *~
# 	rm -f $(OBJFILES3) $(TARGET3) *~
# 	rm -f $(OBJFILES4) $(TARGET4) *~
# 	rm -f $(OBJFILES5) $(TARGET5) *~
# 	rm -f $(OBJFILES6) $(TARGET6) *~
# 	rm -f $(OBJFILES7) $(TARGET7) *~


all: setacl getacl ls fget fput create_dir do_exec fput_encrypt fget_decrypt

setacl: setacl.o helper.o
	gcc -o setacl setacl.o helper.o -g -lcrypto -Wall

getacl: getacl.o helper.o
	gcc -o getacl getacl.o helper.o -g -lcrypto -Wall

ls: ls.o helper.o
	gcc -o ls ls.o helper.o -g -lcrypto -Wall

fget: fget.o helper.o
	gcc -o fget fget.o helper.o -g -lcrypto -Wall

fput: fput.o helper.o
	gcc -o fput fput.o helper.o -g -lcrypto -Wall

create_dir: create_dir.o helper.o
	gcc -o create_dir create_dir.o helper.o -g -lcrypto -Wall

do_exec: do_exec.o helper.o
	gcc -o do_exec do_exec.o helper.o -g -lcrypto -Wall

fput_encrypt: fput_encrypt.o helper.o
	gcc -o fput_encrypt fput_encrypt.o helper.o -g -lcrypto -Wall

fget_decrypt: fget_decrypt.o helper.o
	gcc -o fget_decrypt fget_decrypt.o helper.o -g -lcrypto -Wall

test:
	gcc check.c -g -lcrypto -Wall
	sudo chown root:root a.out
	sudo chmod u+s a.out
	./a.out

clean:
	rm -f setacl.o helper.o setacl *~
	rm -f getacl.o helper.o getacl *~
	rm -f ls.o helper.o ls *~
	rm -f fget.o helper.o fget *~
	rm -f fput.o helper.o fput *~
	rm -f create_dir.o helper.o create_dir *~
	rm -f do_exec.o helper.o do_exec *~
	rm -f fput_encrypt.o helper.o fput_encrypt *~
	rm -f fget_decrypt.o helper.o fget_decrypt *~


