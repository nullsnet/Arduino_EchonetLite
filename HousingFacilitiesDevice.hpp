#include "EchonetLite.hpp"

class HousingFacilitiesDeviceClass : public EchonetLite {
  public:
    enum ClassCode : uint8_t {
        LowVoltageSmartElectricMeter = 0x88, // 低圧スマート電力量メータ
    };

    HousingFacilitiesDeviceClass()
        : EchonetLite() {
        data.EDATA.DEOJ.classGroupCode = HousingFacilitiesDeviceClassGroup;
    }

    explicit HousingFacilitiesDeviceClass(const std::string &response)
        : EchonetLite(response) {
    }

    explicit HousingFacilitiesDeviceClass(const std::vector<uint8_t> &property)
        : EchonetLite(property) {
        data.EDATA.DEOJ.classGroupCode = HousingFacilitiesDeviceClassGroup;
    }
};