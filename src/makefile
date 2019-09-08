CC=gcc

INCLUDE = -I. -I./timer/ -I./mqtt/ -I./buffer/ -I./tcp/ -I./util/ -I./sensor/

CFLAGS = $(INCLUDE) -Wall -Wextra -lpthread -g -lm

OBJ = main.o mqtt_fc.o tcp.o logger.o mqtt_fc_send_receive.o \
mqtt_fc_fsm.o mqtt_fc_build.o mqtt_fc_pack.o mqtt_fc_unpack.o \
timer.o circular_buffer.o mqtt_fc_tcp_connect.o mqtt_fc_response_handler.o\
sensor_dummy.o

OBJ_FILES = $(addprefix $(BUILDDIR)/,$(OBJ))

RM := rm -rf

VPATH = tcp util mqtt timer buffer sensor

BUILDDIR := ../build

all: mqtt_fc

mqtt_fc: $(OBJ_FILES)
	$(CC) -o $(BUILDDIR)/$@ $^ $(CFLAGS)

$(BUILDDIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

run: mqtt_fc
	./$(BUILDDIR)/mqtt_fc mqtt.eclipse.org 1883 #broker.hivemq.com 1883

clean:
	-$(RM) $(BUILDDIR)/*.o $(BUILDDIR)/mqtt_fc

again: clean mqtt_fc

run_again: clean run
