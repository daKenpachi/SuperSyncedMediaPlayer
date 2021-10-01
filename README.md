# SuperSyncedMediaPlayer

This is a media player meant to synchronize video playback between different devices.
It mainly aims at multichannel video installations.

The videos that are played have to be stored locally on the devices (there is no video streaming from one device to another).

It´s written in C++ and use [Openframeworks](https://openframeworks.cc/), therefore it is compilabel for different Operating Systems.
Though it should be mentioned that it was mainly tested on MacOS.

# How to use

Each device should locally store the executable of the SuperSyncedMediaPlayer as well as the video that should be played on that device.
One device acts as a server (controlling start/stop behaviour), all others are clients.

The binary folder structure is as follows:
```
+-- .
+-- SyperSyncedMediaPlayer
+-- data
|  +-- fingers.mp4 (example video)
|  +-- (store your videos here)
|  +-- settings.xml
|  +-- log.txt (if set up)
```

