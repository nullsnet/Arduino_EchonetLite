#include "HousingAndEquipment.hpp"

class LowVoltageSmartElectricMeterClass : public HousingAndEquipmentClass {
  public:
    /// @brief 低圧スマート電力量メータクラス
    enum Property : uint8_t {
        State                                = 0x80, // 動作状態
        BRouteIdentificationNumber           = 0xC0, // Bルート識別番号
        PowerClassification                  = 0xD0, // 1分積算電力量計測値 （正方向、逆方向計測値）
        OwnerClassification                  = 0xD1,
        Phase                                = 0xD2,
        SyntheticTransformationRatio         = 0xD3,
        SyntheticTransformationMagnification = 0xD4,
        CertifiedNumber                      = 0xD5,
        TestExpirationDate                   = 0xD6,
        IntegrationPowerEffectiveDigit       = 0xD7,
        IntegrationPowerPositive             = 0xE0,
        IntegrationPowerUnitPositive         = 0xE1,
        IntegrationPowerHistoryPositive      = 0xE2,
        IntegrationPowerNegative             = 0xE3,
        IntegrationPowerHistoryNegative      = 0xE4,
        DateOfCollectIntegrationPowerHistory = 0xE5,
        InstantPower                         = 0xE7, // 瞬時電力計測値
        InstantCurrent                       = 0xE8,
        InstantVoltage                       = 0xE9,
        FixedIntegrationPowerPositive        = 0xEA,
        FixedIntegrationPowerNegative        = 0xEB,
    };

    LowVoltageSmartElectricMeterClass()
        : HousingAndEquipmentClass() {
        data.EDATA.DEOJ.classCode = LowVoltageSmartElectricMeter;
    }

    LowVoltageSmartElectricMeterClass(const std::string response)
        : HousingAndEquipmentClass(response) {
    }

    LowVoltageSmartElectricMeterClass(const std::vector<uint8_t> property)
        : HousingAndEquipmentClass(property) {
        data.EDATA.DEOJ.classCode = LowVoltageSmartElectricMeter;
    }
};