# Simple Chat Client-Server with Load Balancer

## Project Overview
This project implements a simple client-server system with a load balancer that distributes client requests between two computation servers. The load balancer periodically checks the load on the servers and directs client requests to the server with the least load. The service provided in this implementation is fetching the current date and time from the servers.

## Components
The system consists of three main components:
1. **Computation Servers (S1, S2)**: These servers receive requests from the load balancer and provide either their current load or the requested service (date and time).
2. **Load Balancer (L)**: Acts as an intermediary between clients and computation servers. It maintains load information for the servers and forwards client requests accordingly.
3. **Client (C)**: Connects to the load balancer and requests the current date and time.

## Functionalities
### **Computation Servers (S1, S2)**
- Wait for incoming requests from the load balancer.
- On receiving a "Send Load" request, generate a random integer between 1 and 100 as a dummy load and send it back to the load balancer.
- On receiving a "Send Time" request, return the current date and time to the load balancer.
- Log messages when sending load or time.

### **Load Balancer (L)**
- Periodically (every 5 seconds) queries both servers for their load and stores the last known load values.
- Listens for incoming client requests.
- Selects the server with the least reported load and forwards the client's request for time.
- Receives the response from the selected server and sends it back to the client.
- Logs messages for server load requests, client requests, and forwarding decisions.

### **Client (C)**
- Connects to the load balancer and requests the current date and time.
- Waits for a response and displays the received date and time before closing the connection.

## How to Run
### **Step 1: Start Computation Servers**
Run the computation servers (S1 and S2) with the required port numbers:
```bash
./server <port1>
./server <port2>
```

### **Step 2: Start Load Balancer**
Run the load balancer, specifying its port number:
```bash
./loadbalancer <port>
```

### **Step 3: Start Client**
Run the client program to request the date and time:
```bash
./client <load_balancer_ip> <load_balancer_port>
```

## Design Considerations
- **Concurrency**: The load balancer is designed as a concurrent TCP server, allowing multiple clients to connect.
- **Dynamic Load Balancing**: The load balancer continuously updates the load information and makes real-time decisions.
- **Connection Management**: Each interaction (load request or client request) is handled with a fresh connection for simplicity and isolation.

## Future Improvements
- Implement real load monitoring instead of using dummy loads.
- Add fault tolerance to handle server failures.
- Enhance logging and monitoring features.
- Support multiple types of services beyond date and time.


