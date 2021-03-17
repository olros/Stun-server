FROM ubuntu:20.04

RUN apt-get update

RUN apt-get install -y g++
COPY . .
ENV UDPPORT 80
RUN g++ -o main main.cpp -pthread
CMD ["./main"]
EXPOSE ${UDPPORT}
EXPOSE ${UDPPORT}/udp