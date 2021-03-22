FROM ubuntu:20.04

RUN apt-get update
RUN apt-get install g++-10 -y
COPY . .
ENV UDPPORT 3478
RUN g++-10 -std=c++20 -o main main.cpp -pthread 
CMD ["./main"]
EXPOSE ${UDPPORT}
EXPOSE ${UDPPORT}/udp