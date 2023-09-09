FROM python:3

WORKDIR /usr/src/app

COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

COPY . .

EXPOSE 3002

CMD [ "./configure" ]
CMD [ "cd examples/temperature_producer" ]
CMD [ "make" ]
