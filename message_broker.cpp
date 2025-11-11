#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <memory>
#include <algorithm>

using namespace std;

// Message structure
struct Message {
    string topic;
    string content;
    long long timestamp;
    
    Message(string t, string c) : topic(t), content(c) {
        timestamp = chrono::system_clock::now().time_since_epoch().count();
    }
};

// Subscriber interface
class Subscriber {
private:
    string id;
    queue<Message> messageQueue;
    mutex queueMutex;
    
public:
    Subscriber(string subscriberId) : id(subscriberId) {}
    
    void receiveMessage(const Message& msg) {
        lock_guard<mutex> lock(queueMutex);
        messageQueue.push(msg);
        cout << "[Subscriber " << id << "] Received message on topic '" 
             << msg.topic << "': " << msg.content << endl;
    }
    
    string getId() const { return id; }
    
    int getQueueSize() {
        lock_guard<mutex> lock(queueMutex);
        return messageQueue.size();
    }
};

// The actual Message Broker
class MessageBroker {
private:
    // topic -> list of subscribers
    map<string, vector<shared_ptr<Subscriber>>> subscriptions;
    mutex brokerMutex;
    
    // Message history for persistence (simple in-memory)
    map<string, vector<Message>> messageHistory;
    
public:
    // Publisher publishes a message to a topic
    void publish(const string& topic, const string& content) {
        lock_guard<mutex> lock(brokerMutex);
        
        Message msg(topic, content);
        
        // Store in history
        messageHistory[topic].push_back(msg);
        
        // Send to all subscribers of this topic
        if (subscriptions.find(topic) != subscriptions.end()) {
            for (auto& subscriber : subscriptions[topic]) {
                subscriber->receiveMessage(msg);
            }
            cout << "[Broker] Published message to topic '" << topic 
                 << "' -> " << subscriptions[topic].size() << " subscribers notified\n";
        } else {
            cout << "[Broker] Published to topic '" << topic 
                 << "' but no subscribers found\n";
        }
    }
    
    // Subscribe to a topic
    void subscribe(const string& topic, shared_ptr<Subscriber> subscriber) {
        lock_guard<mutex> lock(brokerMutex);
        subscriptions[topic].push_back(subscriber);
        cout << "[Broker] Subscriber " << subscriber->getId() 
             << " subscribed to topic '" << topic << "'\n";
    }
    
    // Unsubscribe from a topic
    void unsubscribe(const string& topic, const string& subscriberId) {
        lock_guard<mutex> lock(brokerMutex);
        
        if (subscriptions.find(topic) != subscriptions.end()) {
            auto& subs = subscriptions[topic];
            subs.erase(
                remove_if(subs.begin(), subs.end(),
                    [&subscriberId](const shared_ptr<Subscriber>& sub) {
                        return sub->getId() == subscriberId;
                    }),
                subs.end()
            );
            cout << "[Broker] Subscriber " << subscriberId 
                 << " unsubscribed from topic '" << topic << "'\n";
        }
    }
    
    // Get message history for a topic
    vector<Message> getHistory(const string& topic, int limit = 10) {
        lock_guard<mutex> lock(brokerMutex);
        
        if (messageHistory.find(topic) == messageHistory.end()) {
            return {};
        }
        
        auto& history = messageHistory[topic];
        int start = max(0, (int)history.size() - limit);
        return vector<Message>(history.begin() + start, history.end());
    }
    
    void printStats() {
        lock_guard<mutex> lock(brokerMutex);
        cout << "\n=== Broker Statistics ===" << endl;
        cout << "Total topics: " << subscriptions.size() << endl;
        for (const auto& [topic, subs] : subscriptions) {
            cout << "  Topic '" << topic << "': " << subs.size() << " subscribers" << endl;
        }
        cout << "========================\n" << endl;
    }
};

// Demo/Testing
int main() {
    cout << "=== Simple Message Broker (Final Year Project) ===" << endl;
    cout << "Demonstrating pub-sub pattern with multiple topics\n" << endl;
    
    MessageBroker broker;
    
    // Create subscribers
    auto sub1 = make_shared<Subscriber>("user1");
    auto sub2 = make_shared<Subscriber>("user2");
    auto sub3 = make_shared<Subscriber>("user3");
    
    // Subscribe to topics
    broker.subscribe("sports", sub1);
    broker.subscribe("sports", sub2);
    broker.subscribe("tech", sub2);
    broker.subscribe("tech", sub3);
    broker.subscribe("news", sub1);
    
    cout << endl;
    broker.printStats();
    
    // Publish messages
    broker.publish("sports", "India wins World Cup!");
    this_thread::sleep_for(chrono::milliseconds(100));
    
    broker.publish("tech", "New C++ standard released");
    this_thread::sleep_for(chrono::milliseconds(100));
    
    broker.publish("news", "Breaking: Important announcement");
    this_thread::sleep_for(chrono::milliseconds(100));
    
    broker.publish("weather", "Sunny day expected");  // No subscribers
    
    cout << "\n--- Unsubscribing user1 from sports ---\n" << endl;
    broker.unsubscribe("sports", "user1");
    
    broker.publish("sports", "Another sports update!");
    
    cout << endl;
    broker.printStats();
    
    // Get message history
    cout << "\n--- Message History for 'tech' topic ---" << endl;
    auto history = broker.getHistory("tech");
    for (const auto& msg : history) {
        cout << "  [" << msg.timestamp << "] " << msg.content << endl;
    }
    
    return 0;
}