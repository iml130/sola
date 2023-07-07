# Getting Started


## Creating a SOLA instance

First, you need to include the SOLA API header from `sola/include/SOLA/sola.h`.
Then, to create a SOLA instance, you can call `SOLA(...)` with the parameters

* `config` map containing config values,
* `receive_fct` function to be called when a new message is received
* `topic_recv` function to be called when a new message on a topic arrives,
* `storage_logger` vector containing MINHTON-specific loggers, and
* `communication_logger` vector containing natter-specific loggers.

Example usage:

```cpp
std::unordered_map<std::string, std::string> config;
config.insert({"/storage/minhton/config_file", "configurations/root.yml"});
config.insert({"/storage/minhton/name", "sola_node_0"});
auto receive_fct = [](const sola::Message &m){
    std::cout << "Received a SOLA message from " << m.sender_;
};
auto topic_recv = [](const sola::TopicMessage &m){
    std::cout << "Received a message from the topic " << m.topic_;
};
auto storage_logger = std::vector<sola::ManagementOverlayMinhton::Logger>{
      daisi::global_logger_manager->createMinhtonLogger(device_id, "MO")};
auto communication_logger = std::vector<sola::CommunicationNatter::Logger>{
      daisi::global_logger_manager->createNatterLogger(device_id)};

auto sola = new SOLA(
    config, receive_fct, topic_recv, storage_logger, communication_logger
);
```

API:

```cpp
SOLA(
    const std::unordered_map<std::string, std::string> &config,
    MessageReceiveFct receive_fct,
    TopicMessageReceiveFct topic_recv,
    std::vector<typename StorageT::Logger> storage_logger,
    std::vector<typename CommunicationT::Logger> communication_logger
);
```

## Using a SOLA instance

The following methods are defined in the API for interacting with SOLA:

```cpp
void stop();
bool canStop();

//***** CRUD operations for service
void addService(Service service);
void updateService(UUID uuid, Service service);
void removeService(UUID uuid);
std::future<minhton::FindResult> findService(Request r);

// Functions for negotation
void sendData(const std::string &data, Endpoint endpoint);

// event dissemination
void subscribeTopic(const std::string &topic,
                    std::vector<typename StorageT::Logger> logger = {});
void unsubscribeTopic(const std::string &topic);
void publishMessage(const std::string &topic, const std::string &message);

// returns ip/port for SOLA high-level communication
std::string getConectionString() const;
std::string getIP() const;
uint16_t getPort() const;
```
