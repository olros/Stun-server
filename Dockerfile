FROM ubuntu:20.04

RUN apt-get update

RUN apt-get install g++-10 -y
COPY . .
ENV UDPPORT 80
RUN g++-10 -std=c++20 stunserver/main.cpp -o stunserver/main -pthread
RUN g++-10 -std=c++20 tests/runTests.cpp -o tests/runTests -pthread
CMD ["./stunserver/main"]
EXPOSE ${UDPPORT}
EXPOSE ${UDPPORT}/udp