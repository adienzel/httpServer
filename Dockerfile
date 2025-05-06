# Use Alpine Linux as the base image
FROM alpine:latest

# Install necessary packages
RUN apk add --no-cache g++ boost-dev boost-system boost-thread boost-random

# Set the working directory
WORKDIR /usr/src/app

# Copy the source code into the container
COPY . .

# Compile the HTTP server
RUN g++ -o server main.cpp -lboost_system -lboost_thread -lpthread

ENV HTTP_SERVER_PORT_PORT="8080"

# Command to run the server
CMD ["./server"]
