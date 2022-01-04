CC:=gcc
BIN:=datasender
SRC:=$(wildcard *.c)
OBJ:=$(SRC:.c=.o)
LDFLAGS:= -lpthread -lcrypto -W -liotp-as-device -lblobmsg_json -lubox -lubus
CPPFLAGS:=  

PREFIX?=/usr

.PHONY: all clean install uninstall

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	echo $(OBJ)

clean:
	$(RM) $(BIN) $(OBJ)

