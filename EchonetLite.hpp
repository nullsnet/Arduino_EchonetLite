/// @see https://echonet.jp/spec_g/

#pragma once

#include <string.h>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

class EchonetLite {
  public:
    /// @brief 機器オブジェクトスーパークラス
    enum SuperClassProperty : uint8_t {
        OperationStatus                     = 0x80,
        InstallationLocation                = 0x81,
        StandardVersionInformation          = 0x82,
        IdentificationNumber                = 0x83,
        InstantaneousPowerConsumption       = 0x84,
        CumulativeElectricEnergyConsumption = 0x85,
        ManufacturersFaultCode              = 0x86,
        CurrentLimitSetting                 = 0x87,
        FaultStatus                         = 0x88,
        FaultDescription                    = 0x89,
        ManufacturerCode                    = 0x8A,
        BusinessFacilityCode                = 0x8B,
        ProductCode                         = 0x8C,
        ProductionNumber                    = 0x8D,
        ProductionDate                      = 0x8E,
        PowerSavingOperationSetting         = 0x8F,
        RemoteControlSetting                = 0x93,
        CurrentTimeSetting                  = 0x97,
        CurrentDateSetting                  = 0x98,
        PowerLimitSetting                   = 0x99,
        CumulativeOperatingTime             = 0x9A,
        SetMPropertyMap                     = 0x9B,
        GetMPropertyMap                     = 0x9C,
        StatusChangeAnnouncementPropertyMap = 0x9D,
        SetPropertyMap                      = 0x9E,
        GetPropertyMap                      = 0x9F,
    };

    enum ClassGroupCode : uint8_t {
        HousingFacilitiesDeviceClassGroup   = 0x02, // 住宅・設備関連機器クラスグループ
        ManagementOperationDeviceClassGroup = 0x05, // 管理・操作関連機器クラスグループ
    };

    /// @brief ３.２.１.１ ECHONET Lite ヘッダ１（EHD１）
    enum EchonetLiteHeader1 : uint8_t {
        LegacyEchonet  = 0b10000000, // 従来のEchone規格(使用不可)
        NewEchonetLite = 0b00010000, // EchonetLite規格
    };

    /// @brief ３.２.１.２ ECHONET Lite ヘッダ２（EHD２）
    enum EchonetLiteHeader2 : uint8_t {
        Type1 = 0b10000001, // 電文形式1（規定電文形式）
        Type2 = 0b10000010, // 電文形式2（任意電文形式）
    };

    /// @brief ３．２．５ ECHONET Liteサービス（ESV）
    enum EchonetLiteService : uint8_t {
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

    const size_t withoutPayloadSize   = sizeof(EchonetLiteHeader) + (sizeof(EchonetLiteObject) * 2) + sizeof(EchonetLiteService) + sizeof(uint8_t);
    const uint8_t ControllerClassCode = 0xFF;

    EchonetLite() {
        data.EHEAD.head1               = NewEchonetLite;
        data.EHEAD.head2               = Type1;
        data.EHEAD.TransactionId       = 0x0001;
        data.EDATA.SEOJ.classGroupCode = ManagementOperationDeviceClassGroup;
        data.EDATA.SEOJ.classCode      = ControllerClassCode;
        data.EDATA.SEOJ.instanceCode   = 0x01;
        // data.EDATA.DEOJ.classGroupCode;
        // data.EDATA.DEOJ.classCode;
        data.EDATA.DEOJ.instanceCode = 0x01;
        // data.EDATA.echonetLiteService;
        // data.EDATA.operationPropertyCounter;
        // data.payload;
    }

    explicit EchonetLite(const std::string &response) {
        const size_t responseSize     = response.length() / 2;
        uint8_t hexdata[responseSize] = {0};

        for (size_t itr = 0; itr < responseSize; itr++) {
            hexdata[itr] = strtol(response.substr(itr * 2, 2).c_str(), NULL, 16);
        }

        data.EHEAD.head1                    = EchonetLiteHeader1(hexdata[0]);
        data.EHEAD.head2                    = EchonetLiteHeader2(hexdata[1]);
        data.EHEAD.TransactionId            = (hexdata[3] << 8) + hexdata[2];
        data.EDATA.SEOJ.classGroupCode      = (ClassGroupCode)hexdata[4];
        data.EDATA.SEOJ.classCode           = hexdata[5];
        data.EDATA.SEOJ.instanceCode        = hexdata[6];
        data.EDATA.DEOJ.classGroupCode      = (ClassGroupCode)hexdata[7];
        data.EDATA.DEOJ.classCode           = hexdata[8];
        data.EDATA.DEOJ.instanceCode        = hexdata[9];
        data.EDATA.echonetLiteService       = EchonetLiteService(hexdata[10]);
        data.EDATA.operationPropertyCounter = hexdata[11];

        uint8_t counter = 12;
        for (uint8_t i = 0; i < data.EDATA.operationPropertyCounter; i++) {
            EchonetLitePayload payload = {
                .echonetLiteProperty = hexdata[counter++],
                .propertyDataCounter = hexdata[counter++],
            };
            for (size_t j = 0; j < payload.propertyDataCounter; j++) {
                payload.payload.insert(payload.payload.begin(), hexdata[counter + j]);
            }
            counter += payload.payload.size();
            data.payload.push_back(payload);
        }
    }

    explicit EchonetLite(const std::vector<uint8_t> &property)
        : EchonetLite() {
        this->data.EDATA.echonetLiteService       = Get;
        this->data.EDATA.operationPropertyCounter = property.size();
        for (const uint8_t &prop : property) {
            EchonetLitePayload payload = {
                .echonetLiteProperty = prop,
                .propertyDataCounter = 0x00,
                .payload             = std::vector<uint8_t>(),
            };
            this->data.payload.push_back(payload);
        }
    }

    size_t size() {
        size_t size = withoutPayloadSize;
        for (const EchonetLitePayload &payload : data.payload) {
            size += sizeof(payload.echonetLiteProperty);
            size += sizeof(payload.propertyDataCounter);
            size += payload.payload.size();
        }
        return size;
    }

    std::vector<uint8_t> getRawData() {
        uint8_t arrayData[this->size()];
        memcpy(arrayData, reinterpret_cast<uint8_t *>(&this->data), withoutPayloadSize);

        int ptr = withoutPayloadSize;
        for (const EchonetLitePayload &payload : this->data.payload) {
            arrayData[ptr++] = payload.echonetLiteProperty;
            arrayData[ptr++] = payload.propertyDataCounter;
            memcpy(&arrayData[ptr], payload.payload.data(), payload.payload.size());
            ptr += payload.payload.size();
        }

        std::vector<uint8_t> rawData(arrayData, arrayData + this->size());
        return rawData;
    };

    std::vector<EchonetLite::EchonetLitePayload>::iterator getSpecifiedPropertyData(const uint8_t property, const size_t size) {
        return std::find_if(this->data.payload.begin(), this->data.payload.end(), [property, size](EchonetLite::EchonetLitePayload &payload) {
            return payload.echonetLiteProperty == property && payload.payload.size() == size;
        });
    }

    bool isValidValue(const int32_t value) {
        return value != 0x80000000 && value != 0x7FFFFFFF && value != 0x7FFFFFFE;
    }

    bool isValidValue(const int16_t value) {
        return value != 0x8000 && value != 0x7FFF && value != 0x7FFE;
    }
};
