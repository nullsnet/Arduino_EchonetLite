#pragma once

#include "HousingFacilitiesDevice.hpp"

class LowVoltageSmartElectricEnergyMeterClass : public HousingFacilitiesDeviceClass {
  private:
    bool initializedParameter = false;

  public:
    /// @brief 低圧スマート電力量メータクラス
    /// @version APPENDIX ECHONET 機器オブジェクト詳細規定 Release R
    enum class Property : uint8_t {
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

    double cumulativeEnergyUnit           = 1.0; ///< 積算電力量単位デフォルト値
    uint32_t syntheticTransformationRatio = 1;   ///< 係数デフォルト値

    /// @brief 単位初期化
    bool initCumulativeEnergyUnit(void) {
        int8_t unit;
        if (getSpecifiedPropertyData(Property::CumulativeEnergyUnit, &unit) && convertCumulativeEnergyUnit(unit, &this->cumulativeEnergyUnit)) {
            return true;
        }
        return false;
    }

    /// @brief 係数初期化
    bool initSyntheticTransformationRatio(void) {
        uint32_t ratio;
        if (getSpecifiedPropertyData(Property::Coefficient, &ratio) && ratio <= 999999) {
            this->syntheticTransformationRatio = ratio;
            return true;
        }
        return false;
    }

    /// @brief 定数初期化
    bool initializeParameter() {
        this->initializedParameter = (initCumulativeEnergyUnit() && initSyntheticTransformationRatio());
        return this->initializedParameter;
    }

    /// @brief 定数初期化有無
    bool isInitializedParameter() const {
        return this->initializedParameter;
    }

    /// @brief Get要求リクエストデータ生成
    template <class PropertyType>
    void generateGetRequest(const std::vector<PropertyType> &property) {
        HousingFacilitiesDeviceClass::generateGetRequest(property);
        data.EDATA.DEOJ.classCode = static_cast<uint8_t>(ClassCode::LowVoltageSmartElectricMeter);
    }

    /// @brief レスポンスのパース
    bool load(const std::string &response) {
        const bool result = HousingFacilitiesDeviceClass::load(response);
        for (const EchonetLite::EchonetLitePayload &payload : this->data.payload) {
            // 係数と単位はあらかじめパースしておく
            switch (static_cast<Property>(payload.echonetLiteProperty)) {
                case LowVoltageSmartElectricEnergyMeterClass::Property::CumulativeEnergyUnit:
                    initCumulativeEnergyUnit();
                    break;
                case LowVoltageSmartElectricEnergyMeterClass::Property::Coefficient:
                    initSyntheticTransformationRatio();
                    break;
                default:
                    break;
            }
        }
        return result;
    }

    /// @brief 積算電力量単位変換
    bool convertCumulativeEnergyUnit(const uint8_t in, double *const out) {
        switch (in) {
            case 0x00:
                *out = 1.0;
                return true;
            case 0x01:
                *out = 0.1;
                return true;
            case 0x02:
                *out = 0.01;
                return true;
            case 0x03:
                *out = 0.001;
                return true;
            case 0x04:
                *out = 0.0001;
                return true;
            case 0x0A:
                *out = 10.0;
                return true;
            case 0x0B:
                *out = 100.0;
                return true;
            case 0x0C:
                *out = 1000.0;
                return true;
            case 0x0D:
                *out = 10000.0;
                return true;
            default:
                return false;
        }
    }

    /// @brief 瞬時電力計測値取得
    bool getInstantaneousPower(int32_t *const instantaneousPower) const {
        return getSpecifiedPropertyData(Property::InstantaneousPower, instantaneousPower);
    }

    /// @brief 瞬時電流計測値取得
    bool getInstantaneousCurrent(float *const current_R, float *const current_T) const {
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
    bool getCumulativeEnergyPositive(float *const cumulativeEnergyPositive) const {
        int32_t cumulativeEnergyPositiveInt = 0;
        bool hasData                        = getSpecifiedPropertyData(Property::CumulativeEnergyPositive, &cumulativeEnergyPositiveInt);
        if (hasData) {
            *cumulativeEnergyPositive = cumulativeEnergyPositiveInt * this->syntheticTransformationRatio * this->cumulativeEnergyUnit;
        }
        return hasData;
    }

    /// @brief 瞬時電圧計測値取得
    bool getInstantaneousVoltage(float *const voltage_R, float *const voltage_T) const {
        int16_t voltage_R_Int = 0;
        int16_t voltage_T_Int = 0;
        const auto hasData    = getSpecifiedPropertyData(Property::InstantaneousVoltage, &voltage_R_Int, &voltage_T_Int);
        if (hasData) {
            *voltage_R = voltage_R_Int * 0.1;
            *voltage_T = voltage_T_Int * 0.1;
        }
        return hasData;
    }

    /// @brief 積算電力量計測値（逆方向）取得
    bool getCumulativeEnergyNegative(float *const cumulativeEnergyNegative) const {
        int32_t cumulativeEnergyNegativeInt = 0;
        bool hasData                        = getSpecifiedPropertyData(Property::CumulativeEnergyNegative, &cumulativeEnergyNegativeInt);
        if (hasData) {
            *cumulativeEnergyNegative = cumulativeEnergyNegativeInt * this->syntheticTransformationRatio * this->cumulativeEnergyUnit;
        }
        return hasData;
    }

    /// @brief 1分積算電力量計測値取得（正方向、逆方向）
    bool getCumulativeEnergy1Minute(float *const cumulativeEnergyPositive, float *const cumulativeEnergyNegative) const {
        int32_t cumulativeEnergyPositiveInt = 0;
        int32_t cumulativeEnergyNegativeInt = 0;
        const auto hasData                  = getSpecifiedPropertyData(Property::CumulativeEnergy1Minute, &cumulativeEnergyPositiveInt, &cumulativeEnergyNegativeInt);
        if (hasData) {
            *cumulativeEnergyPositive = cumulativeEnergyPositiveInt * this->syntheticTransformationRatio * this->cumulativeEnergyUnit;
            *cumulativeEnergyNegative = cumulativeEnergyNegativeInt * this->syntheticTransformationRatio * this->cumulativeEnergyUnit;
        }
        return hasData;
    }

    /// @brief 定時積算電力量（正方向）取得
    bool getFixedCumulativeEnergyPositive(float *const fixedCumulativeEnergyPositive) const {
        int32_t fixedCumulativeEnergyPositiveInt = 0;
        bool hasData                             = getSpecifiedPropertyData(Property::FixedCumulativeEnergyPositive, &fixedCumulativeEnergyPositiveInt);
        if (hasData) {
            *fixedCumulativeEnergyPositive = fixedCumulativeEnergyPositiveInt * this->syntheticTransformationRatio * this->cumulativeEnergyUnit;
        }
        return hasData;
    }

    /// @brief 定時積算電力量（逆方向）取得
    bool getFixedCumulativeEnergyNegative(float *const fixedCumulativeEnergyNegative) const {
        int32_t fixedCumulativeEnergyNegativeInt = 0;
        bool hasData                             = getSpecifiedPropertyData(Property::FixedCumulativeEnergyNegative, &fixedCumulativeEnergyNegativeInt);
        if (hasData) {
            *fixedCumulativeEnergyNegative = fixedCumulativeEnergyNegativeInt * this->syntheticTransformationRatio * this->cumulativeEnergyUnit;
        }
        return hasData;
    }

    /// @brief 積算電力量計測値履歴（正方向）取得
    bool getCumulativeEnergyHistoryPositive(float *const cumulativeEnergyHistoryPositive) const {
        int32_t cumulativeEnergyHistoryPositiveInt = 0;
        bool hasData                               = getSpecifiedPropertyData(Property::CumulativeEnergyHistoryPositive, &cumulativeEnergyHistoryPositiveInt);
        if (hasData) {
            *cumulativeEnergyHistoryPositive = cumulativeEnergyHistoryPositiveInt * this->syntheticTransformationRatio * this->cumulativeEnergyUnit;
        }
        return hasData;
    }

    /// @brief 積算電力量計測値履歴（逆方向）取得
    bool getCumulativeEnergyHistoryNegative(float *const cumulativeEnergyHistoryNegative) const {
        int32_t cumulativeEnergyHistoryNegativeInt = 0;
        bool hasData                               = getSpecifiedPropertyData(Property::CumulativeEnergyHistoryNegative, &cumulativeEnergyHistoryNegativeInt);
        if (hasData) {
            *cumulativeEnergyHistoryNegative = cumulativeEnergyHistoryNegativeInt * this->syntheticTransformationRatio * this->cumulativeEnergyUnit;
        }
        return hasData;
    }

    /// @brief 積算電力量計測値履歴２取得（正方向、逆方向）
    bool getCumulativeEnergyHistory2(float *const cumulativeEnergyPositive, float *const cumulativeEnergyNegative) const {
        int32_t cumulativeEnergyPositiveInt = 0;
        int32_t cumulativeEnergyNegativeInt = 0;
        const auto hasData                  = getSpecifiedPropertyData(Property::CumulativeEnergyHistory2, &cumulativeEnergyPositiveInt, &cumulativeEnergyNegativeInt);
        if (hasData) {
            *cumulativeEnergyPositive = cumulativeEnergyPositiveInt * this->syntheticTransformationRatio * this->cumulativeEnergyUnit;
            *cumulativeEnergyNegative = cumulativeEnergyNegativeInt * this->syntheticTransformationRatio * this->cumulativeEnergyUnit;
        }
        return hasData;
    }

    /// @brief 積算電力量計測値履歴３取得（正方向、逆方向）
    bool getCumulativeEnergyHistory3(float *const cumulativeEnergyPositive, float *const cumulativeEnergyNegative) const {
        int32_t cumulativeEnergyPositiveInt = 0;
        int32_t cumulativeEnergyNegativeInt = 0;
        const auto hasData                  = getSpecifiedPropertyData(Property::CumulativeEnergyHistory3, &cumulativeEnergyPositiveInt, &cumulativeEnergyNegativeInt);
        if (hasData) {
            *cumulativeEnergyPositive = cumulativeEnergyPositiveInt * this->syntheticTransformationRatio * this->cumulativeEnergyUnit;
            *cumulativeEnergyNegative = cumulativeEnergyNegativeInt * this->syntheticTransformationRatio * this->cumulativeEnergyUnit;
        }
        return hasData;
    }

    /// @brief 積算履歴収集日取得
    bool getDateOfCollectCumulativeEnergyHistory(uint16_t *const date) const {
        return getSpecifiedPropertyData(Property::DateOfCollectCumulativeEnergyHistory, date);
    }

    /// @brief 積算履歴収集日２取得
    bool getDateOfCollectCumulativeEnergyHistory2(uint16_t *const date) const {
        return getSpecifiedPropertyData(Property::DateOfCollectCumulativeEnergyHistory2, date);
    }

    /// @brief 積算履歴収集日３取得
    bool getDateOfCollectCumulativeEnergyHistory3(uint16_t *const date) const {
        return getSpecifiedPropertyData(Property::DateOfCollectCumulativeEnergyHistory3, date);
    }

    /// @brief 所有者区分取得
    bool getOwnerClassification(uint8_t *const ownerClassification) const {
        return getSpecifiedPropertyData(Property::OwnerClassification, ownerClassification);
    }

    /// @brief 相線式設定状態取得
    bool getPhase(uint8_t *const phase) const {
        return getSpecifiedPropertyData(Property::Phase, phase);
    }

    /// @brief 計器認定番号取得
    bool getCertifiedNumber(std::string *const certifiedNumber) const {
        const auto result = findSpecifiedPropertyData(Property::CertifiedNumber, 0);
        if (result != this->data.payload.end()) {
            certifiedNumber->assign(result->payload.begin(), result->payload.end());
            return true;
        }
        return false;
    }

    /// @brief 検定満了年月取得
    bool getTestExpirationDate(uint16_t *const date) const {
        return getSpecifiedPropertyData(Property::TestExpirationDate, date);
    }

    /// @brief 積算電力量有効桁数取得
    bool getCumulativeAmountEnergyEffectiveDigits(uint8_t *const digits) const {
        return getSpecifiedPropertyData(Property::CumulativeAmountEnergyEffectiveDigits, digits);
    }
};