/// @see https://echonet.jp/spec_g/

#pragma once

// #include "esp32-hal-log.h"
#include <algorithm>
#include <limits>
#include <string.h>
#include <string>
#include <vector>

class EchonetLite {
  public:
    /// @brief 機器オブジェクトスーパークラス
    enum class Property : uint8_t {
        OperationStatus                     = 0x80, ///< 動作状態
        InstallationLocation                = 0x81, ///< 設置場所
        StandardVersionInformation          = 0x82, ///< 規格Version情報
        IdentificationNumber                = 0x83, ///< 識別番号
        InstantaneousPowerConsumption       = 0x84, ///< 瞬時消費電力計測値
        CumulativeElectricEnergyConsumption = 0x85, ///< 積算消費電力量計測値
        ManufacturersFaultCode              = 0x86, ///< メーカ異常コード
        CurrentLimitSetting                 = 0x87, ///< 電流制限設定
        FaultStatus                         = 0x88, ///< 異常発生状態
        FaultDescription                    = 0x89, ///< 異常内容
        ManufacturerCode                    = 0x8A, ///< 会員ID／メーカコード
        BusinessFacilityCode                = 0x8B, ///< 事業場コード
        ProductCode                         = 0x8C, ///< 商品コード
        ProductionNumber                    = 0x8D, ///< 製造番号
        ProductionDate                      = 0x8E, ///< 製造年月日
        PowerSavingOperationSetting         = 0x8F, ///< 節電動作設定
        RemoteControlSetting                = 0x93, ///< 遠隔操作設定
        CurrentTimeSetting                  = 0x97, ///< 現在時刻設定
        CurrentDateSetting                  = 0x98, ///< 現在年月日設定
        PowerLimitSetting                   = 0x99, ///< 電力制限設定
        CumulativeOperatingTime             = 0x9A, ///< 積算運転時間
        SetMPropertyMap                     = 0x9B, ///< SetMプロパティマップ
        GetMPropertyMap                     = 0x9C, ///< GetMプロパティマップ
        StatusChangeAnnouncementPropertyMap = 0x9D, ///< 状変アナウンスプロパティマップ
        SetPropertyMap                      = 0x9E, ///< Setプロパティマップ
        GetPropertyMap                      = 0x9F, ///< Getプロパティマップ
    };

    enum class ClassGroupCode : uint8_t {
        HousingFacilitiesDeviceClassGroup   = 0x02, // 住宅・設備関連機器クラスグループ
        ManagementOperationDeviceClassGroup = 0x05, // 管理・操作関連機器クラスグループ
    };

    enum class ClassCode : uint8_t {
        Controller = 0xFF, // 住宅・設備関連機器クラスグループ
    };

    /// @brief ３.２.１.１ ECHONET Lite ヘッダ１（EHD１）
    enum class EchonetLiteHeader1 : uint8_t {
        LegacyEchonet  = 0b10000000, // 従来のEchone規格(使用不可)
        NewEchonetLite = 0b00010000, // EchonetLite規格
    };

    /// @brief ３.２.１.２ ECHONET Lite ヘッダ２（EHD２）
    enum class EchonetLiteHeader2 : uint8_t {
        Type1 = 0b10000001, // 電文形式1（規定電文形式）
        Type2 = 0b10000010, // 電文形式2（任意電文形式）
    };

    /// @brief ３．２．５ ECHONET Liteサービス（ESV）
    enum class EchonetLiteService : uint8_t {
        SetI_SNA   = 0x50,
        SetC_SNA   = 0x51,
        Get_SNA    = 0x52,
        INF_SNA    = 0x53,
        SetGet_SNA = 0x5E,
        SetI       = 0x60, // プロパティ値書き込み要求（応答不要）
        SetC       = 0x61, // プロパティ値書き込み要求（応答要）
        Get        = 0x62, // プロパティ値読み出し要求
        INF_REQ    = 0x63, // プロパティ値通知要求
        SetGet     = 0x6E, // プロパティ値書き込み・読み出し要求
        Set_Res    = 0x71,
        Get_Res    = 0x72,
        INF        = 0x73,
        INFC       = 0x74,
        INFC_Res   = 0x7A,
        SetGet_Res = 0x7E,
    };

    /// @brief ３．２．１ ECHONET Liteヘッダ（EHD）
    struct EchonetLiteHeader {
        EchonetLiteHeader1 head1;
        EchonetLiteHeader2 head2;
        uint16_t TransactionId;
    };

    /// @brief ３．２．４ ECHONET オブジェクト（EOJ）
    struct EchonetLiteObject {
        ClassGroupCode classGroupCode;
        uint8_t classCode;
        uint8_t instanceCode;
    };

    struct EchonetLitePayload {
        uint8_t echonetLiteProperty;  // ３．２．７ ECHONET プロパティ（EPC）
        uint8_t propertyDataCounter;  // ３．２．８ プロパティデータカウンタ（PDC）
        std::vector<uint8_t> payload; // ３．２．９  ECHONET プロパティ値データ（EDT）
    };

    struct EchonetLitePacket {
        /// @brief  ECHONET Liteヘッダ
        EchonetLiteHeader EHEAD;
        struct {
            EchonetLiteObject SEOJ; // 送信元ECHONET Liteオブジェクト指定
            EchonetLiteObject DEOJ; // 相手先ECHONET Liteオブジェクト指定
            EchonetLiteService echonetLiteService;
            uint8_t operationPropertyCounter; // ３．２．６ 処理対象プロパティカウンタ（OPC、OPCSet、OPCGet）
        } EDATA;
        std::vector<EchonetLitePayload> payload;
    } data;

    uint16_t nextTransactionId = 0;

    explicit EchonetLite() {
        format();
    };

    void format() {
        memset(&data.EHEAD, 0, sizeof(data.EHEAD));
        memset(&data.EDATA, 0, sizeof(data.EDATA));
        data.payload.clear();
        data.EHEAD.head1               = EchonetLiteHeader1::NewEchonetLite;
        data.EHEAD.head2               = EchonetLiteHeader2::Type1;
        data.EHEAD.TransactionId       = nextTransactionId;
        data.EDATA.SEOJ.classGroupCode = ClassGroupCode::ManagementOperationDeviceClassGroup;
        data.EDATA.SEOJ.classCode      = static_cast<std::underlying_type<Property>::type>(ClassCode::Controller);
        data.EDATA.SEOJ.instanceCode   = 0x01;
        // data.EDATA.DEOJ.classGroupCode;
        // data.EDATA.DEOJ.classCode;
        data.EDATA.DEOJ.instanceCode = 0x01;
        // data.EDATA.echonetLiteService;
        // data.EDATA.operationPropertyCounter;
        // data.payload;
    }

    /// @brief レスポンスのパース
    bool load(const std::string &response) {
        format();
        const size_t responseSize = response.length() / 2;
        if (responseSize < 12) {
            return false;
        }
        std::vector<uint8_t> hexdata(responseSize, 0);

        for (size_t itr = 0; itr < responseSize; itr++) {
            hexdata[itr] = strtol(response.substr(itr * 2, 2).c_str(), NULL, 16);
        }

        data.EHEAD.head1                    = EchonetLiteHeader1(hexdata[0]);
        data.EHEAD.head2                    = EchonetLiteHeader2(hexdata[1]);
        data.EHEAD.TransactionId            = (hexdata[3] << 8) + hexdata[2];
        data.EDATA.SEOJ.classGroupCode      = ClassGroupCode(hexdata[4]);
        data.EDATA.SEOJ.classCode           = hexdata[5];
        data.EDATA.SEOJ.instanceCode        = hexdata[6];
        data.EDATA.DEOJ.classGroupCode      = ClassGroupCode(hexdata[7]);
        data.EDATA.DEOJ.classCode           = hexdata[8];
        data.EDATA.DEOJ.instanceCode        = hexdata[9];
        data.EDATA.echonetLiteService       = EchonetLiteService(hexdata[10]);
        data.EDATA.operationPropertyCounter = hexdata[11];

        uint8_t counter = 12;
        for (uint8_t i = 0; i < data.EDATA.operationPropertyCounter; i++) {
            if (counter + 2 > hexdata.size()) {
                break;
            }
            EchonetLitePayload payload = {
                .echonetLiteProperty = hexdata[counter++],
                .propertyDataCounter = hexdata[counter++],
                .payload             = std::vector<uint8_t>(),
            };
            for (size_t j = 0; j < payload.propertyDataCounter; j++) {
                if (counter + j >= hexdata.size()) {
                    break;
                }
                payload.payload.push_back(hexdata[counter + j]);
            }
            std::reverse(payload.payload.begin(), payload.payload.end());
            counter += payload.payload.size();
            data.payload.push_back(payload);
        }

        return isTransactionIdExpected();
    }

    /// @brief Get要求リクエストデータ生成
    template <class PropertyType>
    void generateGetRequest(const std::vector<PropertyType> &property) {
        format();
        this->data.EDATA.echonetLiteService       = EchonetLiteService::Get;
        this->data.EDATA.operationPropertyCounter = property.size();
        this->data.EHEAD.TransactionId            = ++nextTransactionId;
        for (const PropertyType &prop : property) {
            EchonetLitePayload payload = {
                .echonetLiteProperty = static_cast<typename std::underlying_type<PropertyType>::type>(prop),
                .propertyDataCounter = 0x00,
                .payload             = std::vector<uint8_t>(),
            };
            this->data.payload.push_back(payload);
        }
    }

    bool isTransactionIdExpected() const {
        return this->data.EHEAD.TransactionId == this->nextTransactionId;
    }

    /// @brief EchonetLiteデータサイズ取得
    size_t size() const {
        size_t size = sizeof(data.EHEAD) + sizeof(data.EDATA);
        for (const EchonetLitePayload &payload : data.payload) {
            size += sizeof(payload.echonetLiteProperty);
            size += sizeof(payload.propertyDataCounter);
            size += payload.payload.size();
        }
        return size;
    }

    /// @brief EchonetLiteバイナリデータ取得
    std::vector<uint8_t> getRawData() const {
        std::vector<uint8_t> rawData;
        rawData.reserve(this->size());

        rawData.push_back(static_cast<uint8_t>(data.EHEAD.head1));
        rawData.push_back(static_cast<uint8_t>(data.EHEAD.head2));
        rawData.push_back(data.EHEAD.TransactionId & 0xFF);
        rawData.push_back((data.EHEAD.TransactionId >> 8) & 0xFF);

        rawData.push_back(static_cast<uint8_t>(data.EDATA.SEOJ.classGroupCode));
        rawData.push_back(data.EDATA.SEOJ.classCode);
        rawData.push_back(data.EDATA.SEOJ.instanceCode);
        rawData.push_back(static_cast<uint8_t>(data.EDATA.DEOJ.classGroupCode));
        rawData.push_back(data.EDATA.DEOJ.classCode);
        rawData.push_back(data.EDATA.DEOJ.instanceCode);
        rawData.push_back(static_cast<uint8_t>(data.EDATA.echonetLiteService));
        rawData.push_back(data.EDATA.operationPropertyCounter);

        for (const EchonetLitePayload &payload : this->data.payload) {
            rawData.push_back(payload.echonetLiteProperty);
            rawData.push_back(payload.propertyDataCounter);
            rawData.insert(rawData.end(), payload.payload.begin(), payload.payload.end());
        }

        return rawData;
    };

    /// @brief 取得データのバリデーション（未設定値を除外）
    template <class T>
    bool isValidValue(const T value) const {
        return value != std::numeric_limits<T>::min() && value != std::numeric_limits<T>::max() && value != std::numeric_limits<T>::max() - 1;
    }

    /// @brief レスポンスからプロパティデータをコピー（再帰的テンプレート基底ケース）
    bool copyPropertyDataImpl(const std::vector<uint8_t> &payload, size_t &offset) const {
        return true;
    }

    /// @brief レスポンスからプロパティデータをコピー（再帰的テンプレート）
    template <class PropertyDataType, class... Rest>
    bool copyPropertyDataImpl(const std::vector<uint8_t> &payload, size_t &offset, PropertyDataType *data, Rest *...rest) const {
        if (offset + sizeof(PropertyDataType) > payload.size()) {
            return false;
        }
        PropertyDataType temp;
        memcpy(&temp, payload.data() + offset, sizeof(PropertyDataType));
        if (!isValidValue(temp)) {
            return false;
        }
        *data = temp;
        offset += sizeof(PropertyDataType);
        return copyPropertyDataImpl(payload, offset, rest...);
    }

    /// @brief レスポンスから特定プロパティのデータ取得（可変長テンプレート）
    template <class PropertyType, class... PropertyDataTypes>
    bool getSpecifiedPropertyData(PropertyType prop, PropertyDataTypes *const... data) const {
        const size_t totalSize = (sizeof(PropertyDataTypes) + ...);
        const auto result      = std::find_if(this->data.payload.cbegin(), this->data.payload.cend(), [prop, totalSize](const EchonetLite::EchonetLitePayload &payload) {
            return payload.echonetLiteProperty == static_cast<typename std::underlying_type<PropertyType>::type>(prop) && payload.payload.size() == totalSize;
        });

        if (result == this->data.payload.end()) {
            return false;
        }
        size_t offset = 0;
        return copyPropertyDataImpl(result->payload, offset, data...);
    }

    /// @brief 可変長プロパティデータ取得（GetPropertyMap等用）
    /// @note load()でreverseされたデータを元のワイヤオーダーに戻して返す
    template <class PropertyType>
    bool getVariableLengthPropertyData(PropertyType prop, std::vector<uint8_t> *out) const {
        const auto it = std::find_if(this->data.payload.cbegin(), this->data.payload.cend(), [prop](const EchonetLitePayload &payload) {
            return payload.echonetLiteProperty == static_cast<typename std::underlying_type<PropertyType>::type>(prop);
        });
        if (it == this->data.payload.end()) {
            return false;
        }
        *out = it->payload;
        std::reverse(out->begin(), out->end());
        return !out->empty();
    }

    /// @brief 動作状態
    bool getOperationStatus(uint8_t *const installationLocation) const {
        return getSpecifiedPropertyData(Property::OperationStatus, installationLocation);
    }

    /// @brief 設置場所
    bool getInstallationLocation(std::vector<uint8_t> *const installationLocation) const {
        return getVariableLengthPropertyData(Property::InstallationLocation, installationLocation);
    }

    /// @brief 規格Version情報
    bool getStandardVersionInformation(uint32_t *const standardVersionInformation) const {
        return getSpecifiedPropertyData(Property::StandardVersionInformation, standardVersionInformation);
    }

    /// @brief 異常発生状態
    bool getFaultStatus(uint8_t *const faultStatus) const {
        return getSpecifiedPropertyData(Property::FaultStatus, faultStatus);
    }

    /// @brief 会員ID／メーカコード
    bool getManufacturerCode(std::vector<uint8_t> *const manufacturerCode) const {
        return getVariableLengthPropertyData(Property::ManufacturerCode, manufacturerCode);
    }

    /// @brief 状変アナウンスプロパティマップ
    bool getStatusChangeAnnouncementPropertyMap(std::vector<uint8_t> *const propertyMap) const {
        return getVariableLengthPropertyData(Property::StatusChangeAnnouncementPropertyMap, propertyMap);
    }

    /// @brief Getプロパティマップ取得
    bool getPropertyMap(std::vector<uint8_t> *const propertyMap) const {
        return getVariableLengthPropertyData(Property::GetPropertyMap, propertyMap);
    }
};
