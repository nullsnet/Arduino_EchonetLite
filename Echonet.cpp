#include "Echonet.hpp"

Echonet::Echonet(){}

Echonet::Echonet(String response)
{
  const size_t responseSize = response.length()/2;
  if(responseSize >= sizeof(EchonetData))
  {
    byte hexdata[sizeof(EchonetData)];
    for(size_t itr = 0;itr < sizeof(EchonetData);itr++)
    {
      hexdata[itr] = strtol(response.substring(itr*2,(itr*2)+2).c_str(),NULL,16);
    }
    data.EHEAD.head1 = Header1(hexdata[0]);
    data.EHEAD.head2 = Header2(hexdata[1]);
    data.EHEAD.TransactionId = (hexdata[3] << 8) + hexdata[2];
    data.EDATA.SEOJ.classGroupCode = hexdata[4];
    data.EDATA.SEOJ.classCode = hexdata[5];
    data.EDATA.SEOJ.instanceCode = hexdata[6];
    data.EDATA.DEOJ.classGroupCode = hexdata[7];
    data.EDATA.DEOJ.classCode = hexdata[8];
    data.EDATA.DEOJ.instanceCode = hexdata[9];
    data.EDATA.service = Service(hexdata[10]);
    data.EDATA.propertyCounter = hexdata[11];
    data.EDATA.property = SmartMeterClass(hexdata[12]);
    data.EDATA.dataCounter = hexdata[13];

    if(responseSize > sizeof(EchonetData))
    {
      for(size_t payloadCounter = sizeof(EchonetData); payloadCounter < responseSize; payloadCounter++)
      {
        payload.insert(payload.begin(),strtol(response.substring(payloadCounter*2,(payloadCounter*2)+2).c_str(),NULL,16));
      }
    }
  }
}

size_t Echonet::size()
{
  return sizeof(EchonetData) + payload.size();
}

uint32_t Echonet::getPayloadInt()
{
  if(payload.size() == sizeof(uint32_t)){
    return *(uint32_t *)payload.data();
  }else
  {
    return 0;
  }
}