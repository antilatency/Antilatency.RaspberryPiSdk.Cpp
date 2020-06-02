Antilatency tracking example for Raspberry Pi
---------------------------------------------

0) Connect to Raspberry Pi board via SSH and install toolchain:
```
sudo apt update && sudo apt install git build-essential
```

1) Clone Antilatency.RaspberryPiSdk.Cpp repo:
```
git clone https://github.com/antilatency/Antilatency.RaspberryPiSdk.Cpp && \
cd Antilatency.RaspberryPiSdk.Cpp/examples/tracking
```

2) Copy udev rules for Antilatency devices to `/etc/` and reload udev:
```
sudo cp antilatency.rules /etc/udev/rules.d/ && \
sudo udevadm control --reload-rules
```

3) Place libs to Antilatency directory:
```
sudo mkdir -p /opt/antilatency/lib && \
sudo cp libAntilatencyAltTracking.so /opt/antilatency/lib && \
sudo cp libAntilatencyDeviceNetwork.so /opt/antilatency/lib
```

4) Build and run example app:
```
make && \
./tracking -v AAVSaWdpZBcABnllbGxvdwQEBAABAQMBAQEDAAEAAD_W
```

