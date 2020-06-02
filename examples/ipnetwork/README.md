AntilatencyIpNetwork example for Raspberry Pi
---------------------------------------------

0) Connect to Raspberry Pi board via SSH and install toolchain:
```
sudo apt update && sudo apt install git build-essential
```

1) Clone Antilatency.RaspberryPiSdk.Cpp repo:
```
git clone https://github.com/antilatency/Antilatency.RaspberryPiSdk.Cpp && \
cd Antilatency.RaspberryPiSdk.Cpp/examples
```

2) Place libs to Antilatency directory:
```
sudo mkdir -p /opt/antilatency/lib && \
sudo cp libAntilatencyIpNetwork.so /opt/antilatency/lib
```

3) Build and run example app:
```
cd ipnetwork && make && \
./ipnetwork -s -c 239.255.111.55
```

