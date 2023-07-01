#include "HousingFacilitiesDevice.hpp"

class LowVoltageSmartElectricEnergyMeterClass : public HousingFacilitiesDeviceClass {
  public:
    /// @brief 低圧スマート電力量メータクラス
    /// @version APPENDIX ECHONET 機器オブジェクト詳細規定 Release R
    enum Property : uint8_t {
        BRouteIdentificationNumber            = 0xC0, ///< Bルート識別番号
        CumulativeEnergy1Minute               = 0xD0, ///< 1分積算電力量計測値 （正方向、逆方向計測値）
        OwnerClassification                   = 0xD1,
        Phase                                 = 0xD2,
        Coefficient                           = 0xD3, ///< 係数
        SyntheticTransformationMagnification  = 0xD4,
        CertifiedNumber                       = 0xD5,
        TestExpirationDate                    = 0xD6,
        CumulativeAmountEnergyEffectiveDigits = 0xD7, ///< 積算電力量有効桁数
        CumulativeEnergyPositive              = 0xE0, ///< 積算電力量計測値（正方向）
        CumulativeEnergyUnit                  = 0xE1, ///< 積算電力量単位（正・逆方向）
        CumulativeEnergyHistoryPositive       = 0xE2, ///< 積算電力量計測値履歴（正方向）
        CumulativeEnergyNegative              = 0xE3, ///< 積算電力量計測値（逆方向）
        CumulativeEnergyHistoryNegative       = 0xE4, ///< 積算電力量計測値履歴（逆方向）
        DateOfCollectCumulativeEnergyHistory  = 0xE5, ///< 積算履歴収集日
        InstantaneousPower                    = 0xE7, ///< 瞬時電力計測値
        InstantaneousCurrents                 = 0xE8, ///< 瞬時電流計測値
        InstantaneousVoltage                  = 0xE9,
        FixedCumulativeEnergyPositive         = 0xEA, ///< 定時積算電力量（正方向）
        FixedCumulativeEnergyNegative         = 0xEB, ///< 定時積算電力量（逆方向）
        CumulativeEnergyHistory2              = 0xEC, ///< 積算電力量計測値履歴２（正方向、逆方向計測値）
        DateOfCollectCumulativeEnergyHistory2 = 0xED, ///< 積算履歴収集日２
        CumulativeEnergyHistory3              = 0xEE, ///< 積算電力量計測値履歴３（正方向、逆方向計測値）
        DateOfCollectCumulativeEnergyHistory3 = 0xEF, ///< 積算履歴収集日３
    };

    float cumulativeEnergyUnit            = 1;
    uint32_t syntheticTransformationRatio = 1;

    LowVoltageSmartElectricEnergyMeterClass()
        : HousingFacilitiesDeviceClass() {
        data.EDATA.DEOJ.classCode = LowVoltageSmartElectricMeter;
    }

    explicit LowVoltageSmartElectricEnergyMeterClass(const std::vector<uint8_t> &property)
        : HousingFacilitiesDeviceClass(property) {
        data.EDATA.DEOJ.classCode = LowVoltageSmartElectricMeter;
    }

    explicit LowVoltageSmartElectricEnergyMeterClass(const std::string &response)
        : HousingFacilitiesDeviceClass(response) {
        for (const EchonetLite::EchonetLitePayload &payload : this->data.payload) {
            switch (payload.echonetLiteProperty) {
                case LowVoltageSmartElectricEnergyMeterClass::Property::CumulativeEnergyUnit:
                    if (payload.payload.size() == sizeof(int8_t))
                        this->cumulativeEnergyUnit = convertCumulativeEnergyUnit(payload.payload[0]);
                    break;
                case LowVoltageSmartElectricEnergyMeterClass::Property::Coefficient:
                    if (payload.payload.size() == sizeof(int32_t))
                        memcpy(&this->syntheticTransformationRatio, payload.payload.data(), sizeof(int32_t));
                    break;
                default:
                    break;
            }
        }
    }

    double convertCumulativeEnergyUnit(const uint8_t raw) {
        switch (raw) {
            case 0x01:
                return 0.1;
            case 0x02:
                return 0.01;
            case 0x03:
                return 0.001;
            case 0x04:
                return 0.0001;
            case 0x0A:
                return 10;
            case 0x0B:
                return 100;
            case 0x0C:
                return 1000;
            case 0x0D:
                return 10000;
            case 0x00:
            default:
                return 1;
        }
    }

    bool getInstantaneousPower(int32_t *const instantaneousPower) {
        const auto result = getSpecifiedPropertyData(LowVoltageSmartElectricEnergyMeterClass::Property::InstantaneousPower, sizeof(int32_t));

        if (result != this->data.payload.end()) {
            int32_t temp;
            memcpy(&temp, (*result).payload.data(), sizeof(int32_t));
            if (isValidValue(temp)) {
                *instantaneousPower = temp;
                return true;
            }
        }
        return false;
    }

    bool getInstantaneousCurrent(float *const current_R, float *const current_T) {
        const auto result = getSpecifiedPropertyData(LowVoltageSmartElectricEnergyMeterClass::Property::InstantaneousCurrents, sizeof(int16_t) * 2);

        if (result != this->data.payload.end()) {
            int16_t temp_R;
            int16_t temp_T;
            memcpy(&temp_R, (*result).payload.data(), sizeof(int16_t));
            memcpy(&temp_T, (*result).payload.data() + sizeof(int16_t), sizeof(int16_t));
            if (isValidValue(temp_R) && isValidValue(temp_T)) {
                *current_R = temp_R * 0.1;
                *current_T = temp_T * 0.1;
                return true;
            }
        }
        return false;
    }

    bool getCumulativeEnergyPositive(float *const cumulativeEnergyPositive) {
        const auto result = getSpecifiedPropertyData(LowVoltageSmartElectricEnergyMeterClass::Property::CumulativeEnergyPositive, sizeof(int32_t));

        if (result != this->data.payload.end()) {
            int32_t temp;
            memcpy(&temp, (*result).payload.data(), sizeof(int32_t));
            if (isValidValue(temp)) {
                *cumulativeEnergyPositive = temp * this->syntheticTransformationRatio * this->cumulativeEnergyUnit;
                return true;
            }
        }
        return false;
    }
};