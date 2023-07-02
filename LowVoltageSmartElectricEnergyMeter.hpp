#include "HousingFacilitiesDevice.hpp"

class LowVoltageSmartElectricEnergyMeterClass : public HousingFacilitiesDeviceClass {
  public:
    /// @brief 低圧スマート電力量メータクラス
    /// @version APPENDIX ECHONET 機器オブジェクト詳細規定 Release R
    enum Property : uint8_t {
        BRouteIdentificationNumber            = 0xC0, ///< Bルート識別番号
        CumulativeEnergy1Minute               = 0xD0, ///< 1分積算電力量計測値 （正方向、逆方向計測値）
        OwnerClassification                   = 0xD1, ///< 所有者区分
        Phase                                 = 0xD2, ///< 相線式設定状態
        Coefficient                           = 0xD3, ///< 係数(合成変成比)
        SyntheticTransformationMagnification  = 0xD4, ///< 係数(合成変成比)の倍率
        CertifiedNumber                       = 0xD5, ///< 計器認定番号
        TestExpirationDate                    = 0xD6, ///< 検定満了年月
        CumulativeAmountEnergyEffectiveDigits = 0xD7, ///< 積算電力量有効桁数
        CumulativeEnergyPositive              = 0xE0, ///< 積算電力量計測値（正方向）
        CumulativeEnergyUnit                  = 0xE1, ///< 積算電力量単位（正・逆方向）
        CumulativeEnergyHistoryPositive       = 0xE2, ///< 積算電力量計測値履歴（正方向）
        CumulativeEnergyNegative              = 0xE3, ///< 積算電力量計測値（逆方向）
        CumulativeEnergyHistoryNegative       = 0xE4, ///< 積算電力量計測値履歴（逆方向）
        DateOfCollectCumulativeEnergyHistory  = 0xE5, ///< 積算履歴収集日
        InstantaneousPower                    = 0xE7, ///< 瞬時電力計測値
        InstantaneousCurrents                 = 0xE8, ///< 瞬時電流計測値
        InstantaneousVoltage                  = 0xE9, ///< 瞬時電圧計測値
        FixedCumulativeEnergyPositive         = 0xEA, ///< 定時積算電力量（正方向）
        FixedCumulativeEnergyNegative         = 0xEB, ///< 定時積算電力量（逆方向）
        CumulativeEnergyHistory2              = 0xEC, ///< 積算電力量計測値履歴２（正方向、逆方向計測値）
        DateOfCollectCumulativeEnergyHistory2 = 0xED, ///< 積算履歴収集日２
        CumulativeEnergyHistory3              = 0xEE, ///< 積算電力量計測値履歴３（正方向、逆方向計測値）
        DateOfCollectCumulativeEnergyHistory3 = 0xEF, ///< 積算履歴収集日３
    };

    float cumulativeEnergyUnit            = 1; ///< 積算電力量単位デフォルト値
    uint32_t syntheticTransformationRatio = 1; ///< 係数デフォルト値

    /// @brief リクエストデータ生成
    LowVoltageSmartElectricEnergyMeterClass()
        : HousingFacilitiesDeviceClass() {
        data.EDATA.DEOJ.classCode = LowVoltageSmartElectricMeter;
    }

    /// @brief Get要求リクエストデータ生成
    explicit LowVoltageSmartElectricEnergyMeterClass(const std::vector<uint8_t> &property)
        : HousingFacilitiesDeviceClass(property) {
        data.EDATA.DEOJ.classCode = LowVoltageSmartElectricMeter;
    }

    /// @brief レスポンスのパース
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

    /// @brief 積算電力量単位変換
    float convertCumulativeEnergyUnit(const uint8_t raw) {
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

    /// @brief 瞬時電力計測値取得
    bool getInstantaneousPower(int32_t *const instantaneousPower) {
        return getSpecifiedPropertyData(Property::InstantaneousPower, instantaneousPower);
    }

    /// @brief 瞬時電流計測値取得
    bool getInstantaneousCurrent(float *const current_R, float *const current_T) {
        int16_t current_R_Int = 0;
        int16_t current_T_Int = 0;
        const auto hasData    = getSpecifiedPropertyData(Property::InstantaneousCurrents, &current_R_Int, &current_T_Int);
        if (hasData) {
            *current_R = current_R_Int * 0.1;
            *current_T = current_T_Int * 0.1;
        }
        return hasData;
    }

    /// @brief 積算電力量計測値（正方向）取得
    bool getCumulativeEnergyPositive(float *const cumulativeEnergyPositive) {
        int32_t cumulativeEnergyPositiveInt = 0;
        bool hasData                        = getSpecifiedPropertyData(Property::CumulativeEnergyPositive, &cumulativeEnergyPositiveInt);
        if (hasData) {
            *cumulativeEnergyPositive = cumulativeEnergyPositiveInt * this->syntheticTransformationRatio * this->cumulativeEnergyUnit;
        }
        return hasData;
    }
};