CC=gcc
CFLAGS=-I.
OBJ = mqtt_fc.o

RM := rm -rf

all: mqtt_fc

#run: mqtt_fc
#	./mqtt_fc

mqtt_fc: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	$(RM) *.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) mqtt_fc
	-$(RM) *.o
