FROM gcc:9.3.0
RUN mkdir /stunserver
WORKDIR /stunserver
COPY server.cpp /stunserver/
ENV UDPPORT 80
RUN g++ -o server server.cpp
CMD ["./server"]
EXPOSE ${UDPPORT}
EXPOSE ${UDPPORT}/udp