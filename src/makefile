CC=gcc

INCLUDE = -I. -I./timer/ -I./mqtt/ -I./buffer/ -I./tcp/ -I./util/

CFLAGS = $(INCLUDE) -Wall -Wextra -lpthread -g

OBJ = main.o mqtt_fc.o tcp.o logger.o mqtt_fc_send_receive.o \
mqtt_fc_fsm.o mqtt_fc_build.o mqtt_fc_pack.o mqtt_fc_unpack.o \
timer.o circular_buffer.o mqtt_fc_tcp_connect.o

OBJ_FILES = $(addprefix $(BUILDDIR)/,$(OBJ))

RM := rm -rf

VPATH = tcp util mqtt timer buffer

BUILDDIR := ../build

all: mqtt_fc

mqtt_fc: $(OBJ_FILES)
	$(CC) -o $(BUILDDIR)/$@ $^ $(CFLAGS)

$(BUILDDIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

run: mqtt_fc
	./$(BUILDDIR)/mqtt_fc #broker.hivemq.com 1883

clean:
	-$(RM) $(BUILDDIR)/*.o $(BUILDDIR)/mqtt_fc

again: clean mqtt_fc

run_again: clean run
