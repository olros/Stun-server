FROM ubuntu:20.04

RUN apt-get update

RUN apt-get install g++-10 -y
COPY . .
RUN g++-10 -std=c++20 ./tests/runTests.cpp -o ./tests/runTests -pthread
CMD ["./tests/runTests"]