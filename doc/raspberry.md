# Setting up Raspberry Pi

## Creating a user

With the release of Raspberry Pi OS Bullseye, the default user `pi` has been [removed for security reasons](https://www.raspberrypi.com/news/raspberry-pi-bullseye-update-april-2022/). If you use a headless setup, you need to create the user in the Raspberry Imager tool [as described in this post](https://www.raspberrypi.com/news/raspberry-pi-bullseye-update-april-2022/). To do this, after selecting the source image and the destination where it should be written to, click on the Settings button (the cogwheel logo in the bottom right corner) before you click on Write. In the Advanced Options, you can enter a username and password.

## Enabling SSH

By default, SSH is disabled in Raspbian. If you use a headless setup, you can enable it by creating a file with name "SSH" in the boot partition of your SD card after writing the image with the Raspberry Imager tool. If you're on a Linux machine, simply open the boot partition in the terminal and execute:

```bash
~ touch SSH
```

Alternatively, you can enable SSH in the Raspberry Imager settings.

## Installing MQTT broker

Mosquitto is an open-source MQTT broker that handles the distribution of messages to and from various clients. To install Mosquitto and its clients package:

```bash
~ sudo apt install -y mosquitto mosquitto-clients
```

After the installation, Mosquitto service will start automatically. To check its status:

```bash
~ sudo systemctl status mosquitto
● mosquitto.service - Mosquitto MQTT Broker
     Loaded: loaded (/lib/systemd/system/mosquitto.service; enabled; preset: en>
     Active: active (running) since Sun 2024-03-31 11:40:51 BST; 35s ago
       Docs: man:mosquitto.conf(5)
             man:mosquitto(8)
    Process: 2428 ExecStartPre=/bin/mkdir -m 740 -p /var/log/mosquitto (code=ex>
    Process: 2429 ExecStartPre=/bin/chown mosquitto /var/log/mosquitto (code=ex>
    Process: 2430 ExecStartPre=/bin/mkdir -m 740 -p /run/mosquitto (code=exited>
    Process: 2432 ExecStartPre=/bin/chown mosquitto /run/mosquitto (code=exited>
   Main PID: 2433 (mosquitto)
      Tasks: 1 (limit: 3910)
        CPU: 38ms
     CGroup: /system.slice/mosquitto.service
             └─2433 /usr/sbin/mosquitto -c /etc/mosquitto/mosquitto.conf

Mar 31 11:40:51 mario systemd[1]: Starting mosquitto.service - Mosquitto MQTT B>
Mar 31 11:40:51 mario systemd[1]: Started mosquitto.service - Mosquitto MQTT Br>
lines 1-17/17 (END)
```

If Mosquitto is not running:

```bash
~ sudo systemctl start mosquitto
```

To ensure Mosquitto starts at boot:

```bash
~ sudo systemctl enable mosquitto
```

To subscribe to a topic:

```bash
~ mosquitto_sub -h localhost -t test/topic
```

To publish a message to a topic:

```bash
~ mosquitto_pub -h localhost -t test/topic -m "Hello, MQTT!"
```

### Configuring MQTT broker

Mosquitto configuration is stored in `mosquitto.conf`. To get details:

```bash
~ man 5 mosquitto.conf
```

By default, setting `allow_anonymous` is set to `false` meaning that only clients which provide their username are allowed to connect to the broker. Set it to `true` if necessary. In addition, setting `listener` must be set to the corresponding port number (`1883` by default). Otherwise you can only access the broker from localhost.

So, [add the following two lines](http://www.steves-internet-guide.com/mosquitto-broker/) to `/etc/mosquitto/mosquitto.conf`:

```
listener 1883
allow_anonymous true
```
