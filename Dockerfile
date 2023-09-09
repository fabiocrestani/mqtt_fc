FROM python:3

RUN ./configure

WORKDIR examples/temperature_producer
RUN make
