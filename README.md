I have one of those led strips which controlled with a colorful remote and shows you the rainbow. I like them, setting the mood up so I can spend time on dark themed environments, or not having to get up after I'm done for the night and want some sleep.
But I think they lack some of those IOT and modern electronics magic we get used to. I only have cheap ones, not those philips hue or some other brand. 
The ones I have, has 20 preconfigured color and 4 different seizure inducing modes, can alter brightness. Everthing is fine and dandy. But if you want to have another color, too bad. Want to turn your home something other than a disco, no mode for that.

In my mind perfect control device should have:
1. Can alter colors without any limit.
2. Can alter modes without any limit.
3. Set auto on-off setting.
4. Can controlled from old school remotes.
5. Integration.

## 1 - Coloring

It just PWM how hard can it be. 

Right now I'm leaning into 3 color channels + 1 common channel. From my market search most of these strips are common VCC and RGB, there are some RGBW but not really widespread, at least in my country.

## 2 - Modes

The ones I have seen have

- Strobe
- Fade
- Ones above but with different colors.

I think these are not really nice modes, fade is somewhat bearable. Having ability to change fade colors, times and looping would be great, one can achieve so much with mere settings. 

## 3 - Auto

Automatic off would be nice set a timeout and let it fade out, or set a timer and wake up to fake sunset in the middle of winter.

## 4 - Remote

Although having a network connection is great we can't be certain a networked device would be avaible for, this reason there should be sane defaults, and ability to set any random IR remote as remote would be great. 

## 5 - Integration

I though about a bit about how can I implement multi device, controller etc in a system only composed of ESP12. It is not actually hard, but I didn't like the complexity of doing it all in C++, with some half-baked http library.

Thinking about Raspberry Pi Mini, an idea occurred, lets make a rPi a host, that applicable devices can connect, this way, I only need to create a web server to handle "User Interaction" on rPi, and it will pass down relevant data to controllers. This way I don't have to implement so much in ESP12's constraints. This has significant positives:

- Only need to create one web app to handle multi devices.
- Ease of adding and removing devices to system
  - Old way, I have to add a device to a network and set it up, and use it alone. This way, a newly added device can search the network for a compliant device and set up a connection and set the device up. After we can manage the controller device with the unified webapp.
- Ease of extensibility. 
  - Host system can be updated to handle more types of devices.
  - Host can be changed with a compliant system.
  - Host can implement third party home automation systems.
- Can update controller systems through air? Should be possible, have seen it, need to check how to implement it.

## Hardware

I have a look at some devices to use

- Raspberry Pi mini as host device. May create a hat for it to act as also, but not really sure about it.

- ESP12, is a good choice for sole pwm controlling job. It can do 200Hz refresh rate with 5ms downtime with software pwm. For hardware pwm, it has limited(2?) channels and not enough PWM counters to handle 3 different pwm channels.
- IRLML0030TRBF (TR Tape reel, BF ?), is a great little mosfet.
  - SOT23, so small
  - V_gth = 2.9V, which is great for most microcontrollers
  - Rds_on = 40mΩ @ 4.5V up to 4.2 amp continuos current. Which is plenty. Will mean ~50W @ 12V per channel. Prolly should limit it about 30W for 10 meter of 3W/m strip per channel.
  - PD of 0.65W @3.3V gate voltage 4 amp current, which will create a junction temperature of 65°C over ambient.
  - Not expensive @ 0.42$ per piece.
- Board will be 2 layers, preferably smaller than 40x60mm.