# Igloo

Igloo is a home automation system designed to be easy to use with respect to privacy. 

There will be single hub, that applicable devices can connect, this way, I only need to create a web server to handle "User Interaction" on the hub, and it will pass down relevant data to controllers. This way I don't have to implement so much in ESP12's constraints. This has significant positives:

- Only need to create one web app to handle multi devices.
- Ease of adding and removing devices to system
  - Old way, I have to add a device to a network and set it up, and use it alone. This way, a newly added device can search the network for a compliant device and set up a connection and set the device up. After we can manage the controller device with the unified web app.
- Ease of extensibility. 
  - Host system can be updated to handle more types of devices.
  - Host can be changed with a compliant system.
  - Host can implement third party home automation systems.
- Can update controller systems through air? Should be possible, have seen it, need to check how to implement it.



# Devices

- [Aurora](aurora/readme.md)

