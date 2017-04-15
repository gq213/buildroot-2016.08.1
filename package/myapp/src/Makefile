

SRC_AGENT = bluez/agent.c
OBJ_AGENT = $(patsubst %.c, %.o, $(SRC_AGENT))

SRC_RADIO = cron/playradio.c
OBJ_RADIO = $(patsubst %.c, %.o, $(SRC_RADIO))

CC = ${CROSS_COMPILE}gcc
CFLAGS = -Wall

PKG ?= pkg-config
CPPFLAGS = $(shell ${PKG} --cflags dbus-1)
CPPFLAGS += $(shell ${PKG} --cflags json-c)
AGENT_LDFLAGS = $(shell ${PKG} --libs dbus-1)
RADIO_LDFLAGS = $(shell ${PKG} --libs json-c)


TARGET = agent radio


all:$(TARGET)

agent:$(OBJ_AGENT)
	$(CC) -o $@ $(OBJ_AGENT) $(AGENT_LDFLAGS)

radio:$(OBJ_RADIO)
	$(CC) -o $@ $(OBJ_RADIO) $(RADIO_LDFLAGS)

%.o:%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $^ -o $@

.PHONY:clean
clean:
	-rm -rf $(TARGET) $(OBJ_AGENT) $(OBJ_RADIO)
