FROM gcc:9.3.0
RUN mkdir /Server
WORKDIR /Server
COPY StunServer.cpp /Server/
ENV UDPPORT 80
RUN g++ -o StunServer StunServer.cpp
CMD ["./StunServer"]
EXPOSE ${UDPPORT}
EXPOSE ${UDPPORT}/udp