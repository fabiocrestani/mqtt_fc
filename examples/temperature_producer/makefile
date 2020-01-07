CC=gcc

RM := rm -rf

BUILDDIR := ../../build

SRC = ../../src

INCLUDE = -I. -I$(SRC)/timer/ -I$(SRC)/mqtt/ -I$(SRC)/buffer/ -I$(SRC)/tcp/ \
-I$(SRC)/util/ -I$(SRC)/sensor/

CFLAGS = $(INCLUDE) -Wall -Wextra -lpthread -g -lm

OBJ = main.o mqtt_fc.o tcp.o logger.o mqtt_fc_send_receive.o \
mqtt_fc_fsm.o mqtt_fc_build.o mqtt_fc_pack.o mqtt_fc_unpack.o \
timer.o circular_buffer.o circular_message_buffer.o mqtt_fc_tcp_connect.o \
mqtt_fc_response_handler.o mqtt_fc_publish_handler.o date.o sensor_dummy.o \
mqtt_fc_topics_loader.o string_utils.o

OBJ_FILES = $(addprefix $(BUILDDIR)/,$(OBJ))

define cc-command
	$(CC) -c -o $@ $< $(CFLAGS)
endef

all: mqtt_fc_producer

mqtt_fc_producer: $(OBJ_FILES)
	$(CC) -o $(BUILDDIR)/$@ $^ $(CFLAGS)

$(BUILDDIR)/%.o: $(SRC)/mqtt/%.c
	$(cc-command)

$(BUILDDIR)/%.o: $(SRC)/tcp/%.c
	$(cc-command)

$(BUILDDIR)/%.o: $(SRC)/util/%.c
	$(cc-command)

$(BUILDDIR)/%.o: $(SRC)/timer/%.c
	$(cc-command)

$(BUILDDIR)/%.o: $(SRC)/buffer/%.c
	$(cc-command)

$(BUILDDIR)/%.o: $(SRC)/sensor/%.c
	$(cc-command)

$(BUILDDIR)/main.o: main.c
	$(CC) -c -o $(BUILDDIR)/main.o main.c $(CFLAGS)

run: mqtt_fc_producer
	./$(BUILDDIR)/mqtt_fc_producer #broker.hivemq.com 1883

clean:
	-$(RM) $(BUILDDIR)/*.o $(BUILDDIR)/mqtt_fc_producer

again: clean mqtt_fc_producer

run_again: clean run
