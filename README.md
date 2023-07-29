# glucolamp
ESP8266-based nightlight changing color based on Nightscout glucose value

I did not finish the hardware (case) of the project, so I don't consider it as being production-ready.

## A few details
This project runs on ESP8266 [NodeMCU v3 CH340](https://www.aliexpress.com/item/32647690484.html?spm=a2g0o.order_list.order_list_main.10.39861802WQGwlw) and users circle of 8 [WS8212](https://www.aliexpress.com/item/32674604552.html?spm=a2g0o.order_list.order_list_main.45.39861802WQGwlw) LEDs. It also has a [button](https://www.aliexpress.com/item/4001209723984.html?spm=a2g0o.order_list.order_list_main.75.39861802WQGwlw)

The data wire of the LED circle should be connected to the EPS8266 pin marked RX. The button should be betn D1 and ground.

When started for the first time, the device will go into the AP mode. The network name is `Glucolamp`. Go to http://192.168.4.1/ to configure the device. There will be parameters for Nightscout host (enter the hostname, not the URL, e.g. myglucose.heroku.com not https://myglucose.heroku.com/), then enter high and low points (everything in mg/dl, integrers only). The low and high point define your "green zone". If the blood glucose is lower than the low point, the light starts getting blue nuances, the lower the bluer, for the high values, it goes to yellow and than the higher the redish it becomes.

## Build
Install VSCode and PlatformIO extension. Build, Upload

---
Feel free to contribute, implement it on your own or do whatever you want with this code. If you need my support or have a question, just shoot a message.





