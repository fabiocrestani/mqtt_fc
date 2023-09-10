# Build environment
FROM alpine as build-env
RUN apk add --no-cache build-base
RUN apk add --no-cache -u musl
RUN apk add --no-cache linux-headers

COPY . mqtt_fc
WORKDIR /mqtt_fc/examples/temperature_producer
RUN make

# Run environment
FROM alpine
COPY --from=build-env /mqtt_fc/build/temperature-producer/mqtt_fc_producer /mqtt_fc/mqtt_fc_producer
WORKDIR /mqtt_fc
CMD ["./mqtt_fc_producer"]
