# Networks

## Part A

### Basic

#### TCP

Directory: `./partA/basic/tcp`

Run the server code:

```bash
gcc -o server server.c
./server
```

Run the client code:

```bash
gcc -o client client.c
./client
```

#### UDP

Directory: `./partA/basic/udp`

Run the server code:

```bash
gcc -o server server.c
./server <port number>
```

Run the client code:

```bash
gcc -o client client.c
./client <port number>
```

### Rock-Paper-Scissors

Directory: `./partA/rps`

Run the server code:

```bash
gcc -o server server.c
./server
```

Run the client codes:

```bash
gcc -o p1 player1.c
./p1
```

```bash
gcc -o p2 player2.c
./p2

```

The running instructions mentioned above are same for both TCP and UDP implementations and should be run in their respective directories.

#### TCP

Directory: `./partA/rps/tcp`

#### UDP

Directory: `./partA/rps/udp`

## Part B

Directory: `./partB`

`client-server.c`: Sends message first and then receives.
`server-client.c`: Receives message first and then sends.

### Instructions for Running

```bash
gcc -o server server-client.c
./server
```

```bash
gcc -o client client-server.c
./client
```

### Intuition

- Divide the data into chunks of size 256 bytes.
- Send the chunks to the server.
- Two threads are created
  - One is used to receive acknowledgements from the server.
  - The other is used to resend the chunks to the server if the acknowledgement is not received within a specified time.
- We keep doing this until all the chunks are sent and acknowledged.

## Report

### Question 1

How is your implementation of data sequencing and retransmission different from traditional TCP?

<b>Answer</b>
In a traditional implementation of TCP, the rate at which data is sent is dynamically adjusted based on networks and congestion. It uses a sliding window mechanism to control the unacknowledged data to optimise the usage of resources. Also, data can be sent and received simultaneously.
On the other hand, in my implementation, while acknowledgement, retransmission, and sequencing have been taken care of, other features mentioned above like flow control, network congestion, etc have not been taken care of and sliding window approach has not been used.

### Question 2

How can you extend your implementation to account for flow control? You may ignore deadlocks.

<b>Answer</b>
TCP uses end-to-end flow control to avoid the the sender sending data too quickly for the receiver to be able to receive. This makes the process more reliable.
To implement this, we can use the sliding window technique to control the data flow. The window size represents the maximum number of unacknowledged chunks that can be in transit at any given time. When acknowledgments are received, we can slide the window forward. New chunks will only be sent if there is space available in the sliding window. In this case, the receiver can specify how much space they are allocating for receiving data and the sender can send accordingly.