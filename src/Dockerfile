FROM ubuntu:latest

RUN apt-get update && apt-get install -y libcpprest-dev libpq-dev libfmt-dev build-essential

WORKDIR /app/src
ADD . /app/src

RUN g++ -o server $(find . -name "*.cpp") --std=c++17 \
    -I /usr/include/postgresql/ -Iheaders/ -lcpprest -pthread -lcrypto -lfmt -lpq

ENTRYPOINT [ "/bin/bash", "-c", "sleep 5 && ./server 0.0.0.0 8000"]