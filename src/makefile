CC=gcc
CFLAGS=-I. -Wall -Wextra -lpthread -g
OBJ = main.o mqtt_fc.o tcp.o logger.o mqtt_fc_send_receive.o \
mqtt_fc_fsm.o mqtt_fc_build.o mqtt_fc_pack.o mqtt_fc_unpack.o \
timer.o circular_buffer.o mqtt_fc_tcp_connect.o

RM := rm -rf

all: mqtt_fc

mqtt_fc: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

run: mqtt_fc
	./mqtt_fc broker.hivemq.com 1883

clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) mqtt_fc
	-$(RM) *.o
