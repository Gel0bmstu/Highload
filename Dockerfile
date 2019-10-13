FROM ubuntu:latest
WORKDIR /bin/
COPY . .
RUN apt-get update && yes | \
    apt-get install "libevent-dev" "zlib1g-dev" "cmake"
RUN ./run.sh
EXPOSE 80
ENTRYPOINT ["./bin/HighloadServer", "/etc/httpd.conf", "2>&1"]