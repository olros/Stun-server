FROM gcc:9.3.0
COPY . .
ENV UDPPORT 80
RUN g++ -o main main.cpp -pthread
CMD ["./main"]
EXPOSE ${UDPPORT}
EXPOSE ${UDPPORT}/udp