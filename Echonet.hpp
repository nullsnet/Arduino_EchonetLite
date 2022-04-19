#ifndef __Echonet_HPP
#define __Echonet_HPP

#include <Arduino.h>

class Echonet
{
public:
  enum SmartMeterClass : byte
  {
    State = 0x80,
    PowerClassification = 0xD0,
    OwnerClassification = 0xD1,
    Phase = 0xD2,
    SyntheticTransformationRatio = 0xD3,
    SyntheticTransformationMagnification = 0xD4,
    CertifiedNumber = 0xD5,
    TestExpirationDate = 0xD6,
    IntegrationPowerEffectiveDigit = 0xD7,
    IntegrationPowerPositive = 0xE0,
    IntegrationPowerUnitPositive = 0xE1,
    IntegrationPowerHistoryPositive = 0xE2,
    IntegrationPowerNegative = 0xE3,
    IntegrationPowerHistoryNegative = 0xE4,
    DateOfCollectIntegrationPowerHistory = 0xE5,
    InstantPower = 0xE7,
    InstantCurrent = 0xE8,
    InstantVoltage = 0xE9,
    FixedIntegrationPowerPositive = 0xEA,
    FixedIntegrationPowerNegative = 0xEB,
  };

  enum Header1 : byte
  {
    // ３.２.１.１ ECHONET Lite ヘッダ１（EHD１）
    LegacyEchonet = 0b10000000,
    EchonetLite = 0b00010000,
  };

  enum Header2 : byte
  {
    // ３.２.１.２ ECHONET Lite ヘッダ２（EHD２）
    Type1 = 0b10000001,
    Type2 = 0b10000010,
  };

  enum Service : byte
  {
    SetI = 0x60,
    SetC = 0x61,
    Get = 0x62,
    INF_REQ = 0x63,
    SetGet = 0x6E,
  };

  struct EchonetData {
    struct{
      Header1 head1 = EchonetLite;
      Header2 head2 = Type1;
      uint16_t TransactionId = 0x0001;
    }EHEAD;
    struct{
      struct{
        byte classGroupCode = 0x05;
        byte classCode = 0xFF;
        byte instanceCode = 0x01;
      }SEOJ;
      struct{
        byte classGroupCode = 0x02;
        byte classCode = 0x88;
        byte instanceCode = 0x01;
      }DEOJ;
      Service service = Get;
      byte propertyCounter = 0x01;
      SmartMeterClass property;
      byte dataCounter = 0x00;
    }EDATA;
  }data;
  std::vector<byte> payload;
  Echonet();
  Echonet(SmartMeterClass property);
  Echonet(String data);
  size_t size();
  std::vector<byte> getRawData();
};

#endif // __Echonet_HPP