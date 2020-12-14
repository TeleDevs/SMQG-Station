# SMQG Station
## Requirements
Functional requirements:

- RF.01 The system must read data from external sensors. 
- RF.02 The system must broadcast the data collected from sensors through a LoRa module.
- RF.03 The system must broadcast the data collected from sensors through a BLE module.

Non-functional requirements:

- RF.01 The system must be compatible with DHT11 sensor.
- RF.02 The system must be implemented in a ESP32 board.


## Flow chart

```mermaid
graph LR
A((Start)) --> B(Idle)
B --> C(Read)
C --> D(Broadcast)
D --> B
```
- Idle: Stage when the system waiting, with all the external devices shutdow, a the timeout to start the reading cicle.
- Read: The system reads all sensors and buffers the data in JSON format.
- Broadcast: The system broadcast all the buffered data through LoRa and BLE module.

## Libraries

|Functionality   |Library                        |Tutorial                     |
|----------------|-------------------------------|-----------------------------|
|DHT11 reading   |Adahfruit                      |https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-sensor-arduino-ide/ |
|BLE broadcasting     |XXXX | XXXX |
|LoRa broadcasting    |XXXX | XXXX |
