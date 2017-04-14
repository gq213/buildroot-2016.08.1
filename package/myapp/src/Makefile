

SRC_AGENT = bluez/agent.c
OBJ_AGENT = $(patsubst %.c, %.o, $(SRC_AGENT))

CC = ${CROSS_COMPILE}gcc
CFLAGS = -Wall

PKG ?= pkg-config
CPPFLAGS = $(shell ${PKG} --cflags dbus-1)
LDFLAGS = $(shell ${PKG} --libs dbus-1)


TARGET = agent


all:$(TARGET)

agent:$(OBJ_AGENT)
	$(CC) -o $@ $(OBJ_AGENT) $(LDFLAGS)

%.o:%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $^ -o $@

.PHONY:clean
clean:
	-rm -rf $(TARGET) $(OBJ_AGENT)
