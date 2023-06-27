#include "EchonetLite.hpp"

class HousingAndEquipmentClass : public EchonetLite {
  public:
    enum ClassCode : uint8_t {
        LowVoltageSmartElectricMeter = 0x88, // 低圧スマート電力量メータ
    };

    HousingAndEquipmentClass()
        : EchonetLite() {
        data.EDATA.DEOJ.classGroupCode = HousingAndEquipmentClassGroup;
    }

    HousingAndEquipmentClass(const std::string response)
        : EchonetLite(response) {
    }

    HousingAndEquipmentClass(const std::vector<uint8_t> property)
        : EchonetLite(property) {
        data.EDATA.DEOJ.classGroupCode = HousingAndEquipmentClassGroup;
    }
};