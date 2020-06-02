#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <exception>
#include <fstream>
#include <streambuf>
#include <sstream>

#include <Antilatency.Api.h>
#include <Antilatency.InterfaceContract.LibraryLoader.h>

#ifndef ANTILATENCY_PACKAGE_DIR
#   define ANTILATENCY_PACKAGE_DIR "./"
#endif

#ifndef AIPNET_LIB
#   define AIPNET_LIB ANTILATENCY_PACKAGE_DIR "lib/libAntilatencyIpNetwork.so"
#endif

std::string getInterfaceMacAddress(const std::string &ifname) {

    try {
        std::ifstream iface("/sys/class/net/"+ifname+"/address");
        std::string macAddress((std::istreambuf_iterator<char>(iface)),
                                std::istreambuf_iterator<char>());
        if (macAddress.length() > 0) {
            macAddress.pop_back();
            return macAddress;
        }
    } catch(const std::exception &) {}

    return "00:00:00:00:00:00";
}

int main(int argc, char *argv[]) {

    std::string localAddress = Antilatency::IpNetwork::Constants::DefaultIfaceAddress;
    std::string multicastAddress = Antilatency::IpNetwork::Constants::DefaultTargetAddress;
    std::int32_t port = Antilatency::IpNetwork::Constants::DefaultTrackingPort;
    std::int32_t commandPort = Antilatency::IpNetwork::Constants::DefaultCommandPort;
    bool send = false;
    bool commandEnabled = false;
    std::int32_t statesNum = 5;
    std::int32_t waitTime = 16;
    std::string id = getInterfaceMacAddress("wlan0");

    std::vector<std::string> params(argv + 1, argv + argc);

    for (const auto &param : params) {
        if ("-h" == param) {
            std::cout << argv[0] << " [--send] [--command] [multicastIpAddress]" << std::endl;
            return 0;
        } else if ("-s" == param || "--send" == param) {
            send = true;
        } else if ("-c" == param || "--command" == param) {
            commandEnabled = true;
        } else {
            multicastAddress = param;
        }
    }

    try {
        Antilatency::IpNetwork::ILibrary netServiceLibrary = Antilatency::InterfaceContract::getLibraryInterface
                <Antilatency::IpNetwork::ILibrary>(AIPNET_LIB);

        if (netServiceLibrary == nullptr) {
            std::cerr << "Couldn't load AntilatencyIpNetwork library!" << std::endl;
            return 1;
        }

        auto version = netServiceLibrary.getVersion();
        std::cout << "AntilatencyIpNetwork library version: " << version << "\n";

        Antilatency::IpNetwork::INetworkServer netServer
                = netServiceLibrary.getNetworkServer(id, localAddress, multicastAddress, port, commandPort);

        netServer.startMessageListening();

        if (commandEnabled == true) {
            netServer.startCommandListening();
        }

        std::vector<Antilatency::IpNetwork::StateMessage> messages{};
        if (statesNum > 0) {
            Antilatency::IpNetwork::StateMessage emptyMessage{};
            messages.push_back(emptyMessage);

            if (statesNum > 1) {
                for (std::int32_t index = 0; index < (statesNum - 1); index++) {
                    Antilatency::IpNetwork::StateMessage message{};
                    message.positionX = 0.0123456789f + index;
                    message.positionY = 0.1234567891f + index;
                    message.positionZ = 0.2345678912f + index;
                    message.rotationX = 0.3456789123f + index;
                    message.rotationY = 0.4567891234f + index;
                    message.rotationZ = 0.5678912345f + index;
                    message.rotationW = 0.6789123456f + index;
                    message.trackerError = Antilatency::IpNetwork::ErrorType::None;
                    message.rawTag = netServiceLibrary.getRawTagFromString("None");
                    messages.push_back(message);
                }
            }
        }

        std::vector<Antilatency::IpNetwork::GpioPinState> pins{
            {25, 1},
            {27, 1},
            {31, 0}
        };

        std::int32_t waitCommandTime = waitTime * 5;
        std::int32_t wait = 0;

        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

            if (send == true) {
                try {
                    netServer.sendStateMessages(messages, pins, "");
                } catch (const std::exception &ex) {
                    std::cerr << "sendStateMessages error: " << ex.what() << std::endl;
                }
            }

            if (commandEnabled == true) {
                if (wait >= waitCommandTime) {
                    wait = 0;
                    try {
                        netServer.sendCommand(Antilatency::IpNetwork::CommandKey::None, "");
                        netServer.sendCommand(Antilatency::IpNetwork::CommandKey::Custom, "Custom");
                        netServer.sendCommand(Antilatency::IpNetwork::CommandKey::SetEnvinromentCode, "AAVSaWdpZBcABnllbGxvdwQEBAABAQMBAQEDAAEAAD_W");
                        netServer.sendCommand(Antilatency::IpNetwork::CommandKey::SetSendingRate, "8");
                    } catch (const std::exception &ex) {
                        std::cerr << "sendCommand error: " << ex.what() << std::endl;
                    }
                }

                wait += waitTime;
            }

            std::stringstream output{};

            if (commandEnabled == true) {
                Antilatency::IpNetwork::IConstCommandList commands = netServer.getCommands();
                for (std::size_t index = 0; index < commands.size(); index++) {
                    auto command = commands.get(index);
                    output << "id: " << command.id()
                           << "; ip: " << command.ipAddress()
                           << "; c_time: " << netServiceLibrary.getCurrentTime()
                           << "; error: " << command.error()
                           << "; r_time: " << command.timeStamp()
                           << "; key: " << Antilatency::enumToString(command.key())
                           << "; value: " << command.value()
                           << "\n";
                }
            }

            Antilatency::IpNetwork::IConstDeviceList providers = netServer.getDeviceList();
            size_t size = providers.size();
            for (std::size_t index = 0; index < size; index++) {
                auto provider = providers.get(index);

                if (false == provider.newState()) {
                    continue;
                }

                auto states = provider.lastStates();
                std::int64_t receiverTime = provider.timeStamp();
                std::int64_t latency = provider.latency();

                output << "id: " << provider.id()
                       << "; ip: " << provider.ipAddress()
                       << "; c_time: " << netServiceLibrary.getCurrentTime()
                       << "; error: " << provider.deviceError();

                output << "\n\t";
                output << "latency: " << latency
                       << "; r_time: " << receiverTime
                       << "; session: " << provider.sessionId()
                       << "; packet_num: " << provider.packetNumber();

                if (provider.gpioState().empty() == false) {
                    output << "; gpio: ";
                }
                for (auto pin : provider.gpioState()) {
                    output << pin.value;
                }

                for (auto const &state : states) {
                    output << "\n\t"
                           << "posX: " << state.positionX
                           << "; posY: " << state.positionY
                           << "; posZ: " << state.positionZ
                           << "; rotX: " << state.rotationX
                           << "; rotY: " << state.rotationY
                           << "; rotZ: " << state.rotationZ
                           << "; rotW: " << state.rotationW
                           << "; error: " << Antilatency::enumToString(state.trackerError)
                           << "; tag: " << netServiceLibrary.getTagFromRawTag(state.rawTag);
                }

                output << '\n';

                std::cout << output.str();
                std::flush(std::cout);
            }
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
