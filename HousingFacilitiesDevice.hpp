#include "EchonetLite.hpp"

class HousingFacilitiesDeviceClass : public EchonetLite {
  public:
    enum class ClassCode : uint8_t {
        LowVoltageSmartElectricMeter = 0x88, // 低圧スマート電力量メータ
    };

    HousingFacilitiesDeviceClass()
        : EchonetLite() {
        this->data.EDATA.DEOJ.classGroupCode = EchonetLite::ClassGroupCode::HousingFacilitiesDeviceClassGroup;
    }

    explicit HousingFacilitiesDeviceClass(const std::string &response)
        : EchonetLite(response) {
    }

    template <class PropertyType>
    explicit HousingFacilitiesDeviceClass(const std::vector<PropertyType> &property)
        : EchonetLite(property) {
        this->data.EDATA.DEOJ.classGroupCode = EchonetLite::ClassGroupCode::HousingFacilitiesDeviceClassGroup;
    }
};