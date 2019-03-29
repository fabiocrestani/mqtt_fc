CC=gcc
CFLAGS=-I. -Wall -Wextra
OBJ = mqtt_fc.o tcp.o logger.o

RM := rm -rf

all: mqtt_fc

#run: mqtt_fc
#	./mqtt_fc

mqtt_fc: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	$(RM) *.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

run: mqtt_fc
	./mqtt_fc

clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) mqtt_fc
	-$(RM) *.o
