# message-broker
# Message Broker - Pub/Sub System

message broker implementation in C++ demonstrating the publish-subscribe pattern.

## Overview

This project implements a message broker that allows publishers to send messages to topics and subscribers to receive messages from topics they're interested in. The broker acts as an intermediary, decoupling publishers from subscribers.

## Features

- **Topic-based messaging** - Publishers send messages to named topics
- **Multiple subscribers** - Any number of subscribers can listen to a topic
- **Thread-safe operations** - Uses mutex locks for concurrent access
- **Message history** - Stores recent messages for each topic
- **Dynamic subscription management** - Subscribe/unsubscribe at runtime

## Getting Started

### Compilation
```bash
g++ -std=c++11 -pthread message_broker.cpp -o message_broker
```

### Running
```bash
./message_broker
```

## Usage

```cpp
// Create broker
MessageBroker broker;

// Create subscribers
auto sub1 = make_shared<Subscriber>("user1");
auto sub2 = make_shared<Subscriber>("user2");

// Subscribe to topics
broker.subscribe("sports", sub1);
broker.subscribe("tech", sub2);

// Publish messages
broker.publish("sports", "Breaking news!");
broker.publish("tech", "New C++ features released");

// Unsubscribe
broker.unsubscribe("sports", "user1");

// Get message history
auto history = broker.getHistory("sports", 5);
```

## API Reference

- `publish(topic, content)` - Publishes a message to a topic
- `subscribe(topic, subscriber)` - Subscribes to a topic
- `unsubscribe(topic, subscriberId)` - Removes a subscriber from a topic
- `getHistory(topic, limit)` - Retrieves last N messages from a topic
- `printStats()` - Displays broker statistics

## Use Cases

- Real-time notifications (news feeds, alerts)
- Event-driven systems (microservices communication)
- IoT applications (sensor data distribution)
- Chat applications (room-based messaging)

## Thread Safety

All broker operations are thread-safe using `std::mutex`. Multiple publishers can publish simultaneously, and subscribers can subscribe/unsubscribe while messages are being published.

## Concepts Demonstrated

- **Publish-Subscribe Pattern** - Decoupled messaging architecture
- **Thread Safety** - Mutex locks and synchronization
- **Smart Pointers** - Memory management with `shared_ptr`
- **STL Containers** - `map`, `vector`, `queue`

## Limitations & Future Enhancements

### Current Limitations
- In-memory only (messages lost on restart)
- No persistent storage
- No delivery guarantees
- Single broker (not distributed)

### Possible Futre Enhancements
- Persist messages to disk/database
- Add message acknowledgment (ACK/NACK)
- Implement quality of service levels
- Distributed broker cluster
- REST/WebSocket API for network access
