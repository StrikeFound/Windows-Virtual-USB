#include "VirtualUsb.h"
#include <iostream>
#include <stdexcept>
#include <utility>
#include <type_traits>

namespace VirtualUsb::Detail {
// Internal SDK helper exercised without opening a driver or creating a device.
std::pair<std::uint8_t,std::uint8_t> Input_Endpoint(const Device_Config&) noexcept;
}

namespace {
void Require(bool value,const char* message) {
    if (!value) throw std::runtime_error(message);
}
VirtualUsb::Device_Config Config(std::initializer_list<std::uint8_t> endpoints) {
    VirtualUsb::Device_Config result;
    VirtualUsb::Descriptor device;
    device.data={18,1,0,2,0,0,0,64,0xff,0xff,1,0,0,1,0,0,0,1};
    VirtualUsb::Descriptor configuration;
    const auto total=static_cast<std::uint8_t>(18+7*endpoints.size());
    configuration.data={9,2,total,0,1,1,0,0x80,50,
        9,4,0,0,static_cast<std::uint8_t>(endpoints.size()),0xff,0,0,0};
    for (const auto address:endpoints)
        configuration.data.insert(configuration.data.end(),{7,5,address,3,64,0,1});
    result.descriptors={device,configuration};return result;
}
}

int main() {
    try {
        static_assert(std::is_trivially_copyable_v<VirtualUsb::Bytes>);
        const std::uint8_t raw[]={1,2,3};
        const std::array<std::uint8_t,3> fixed={1,2,3};
        const std::vector<std::uint8_t> dynamic={1,2,3};
        const VirtualUsb::Bytes a=raw,b=fixed,c=dynamic,empty;
        Require(a.data()==raw && a.size()==3,"C arrays must be borrowed without copying.");
        Require(b.data()==fixed.data() && b.size()==3,"std::array reports must be borrowed without copying.");
        Require(c.data()==dynamic.data() && c.size()==3,"Vector reports must be borrowed without copying.");
        Require(empty.empty() && empty.size()==0,"Default byte range must be empty.");
        const auto immediate=[](VirtualUsb::Bytes bytes){return bytes.size()==3 && bytes.data()[2]==3;};
        Require(immediate({1,2,3}),"Inline byte lists must remain valid throughout the call.");
        for (const auto& endpoints : {std::initializer_list<std::uint8_t>{}, {0x01}, {0x81}, {0x82,0x01}, {0x81,0x82}, {0x01,0x81,0x02,0x82}}) {
            const auto config=Config(endpoints);
            Require(bool(VirtualUsb::Check(config)),"A supported test layout was rejected.");
            const auto selection=VirtualUsb::Detail::Input_Endpoint(config);
            std::uint8_t count=0,first=0;
            for (const auto endpoint:endpoints) if (endpoint&0x80) {if (!count) first=endpoint;++count;}
            Require(selection.first==count && selection.second==first,"Input endpoint selection differs from the declared topology.");
        }
        VirtualUsb::Device inactive;
        const auto automatic=VirtualUsb::Write(inactive,{1,2,3});
        Require(!automatic && automatic.error==6 && automatic.message=="Device is not active.","Automatic Write did not reject an inactive device.");
        const auto explicitEndpoint=VirtualUsb::Write(inactive,0x81,{1,2,3});
        Require(VirtualUsb::Write(inactive,raw).error==6,"C-array Write must reject inactive handles without opening a driver.");
        Require(VirtualUsb::Write(inactive,0x81,fixed).error==6,"Fixed-array Write must reject inactive handles without opening a driver.");
        Require(!explicitEndpoint && explicitEndpoint.error==6,"Explicit Write did not reject an inactive device.");
        std::cout << "WindowsVirtualUsb endpoint selection and inactive-device checks passed; no driver opened.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';return 1;
    }
}
