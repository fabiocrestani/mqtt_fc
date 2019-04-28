CC=gcc
CFLAGS=-I. -Wall -Wextra -lpthread
OBJ = main.o mqtt_fc.o tcp.o logger.o mqtt_fc_send_receive.o \
mqtt_fc_pack_unpack.o timer.o

RM := rm -rf

all: mqtt_fc

mqtt_fc: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	$(RM) *.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

run: mqtt_fc
	./mqtt_fc broker.hivemq.com 1883

clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) mqtt_fc
	-$(RM) *.o
