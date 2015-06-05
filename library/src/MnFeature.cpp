/************************************************************************
\file MnFeature.cpp
\brief Implementation of the Class MnFeature
\author rueckerc, Bernecker+Rainer Industrie Elektronik Ges.m.b.H.
\date 01-May-2015 12:00:00
************************************************************************/

/*------------------------------------------------------------------------------
Copyright (c) 2015, Bernecker+Rainer Industrie-Elektronik Ges.m.b.H. (B&R)
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holders nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------*/
#include "MnFeature.h"

using namespace std;
using namespace IndustrialNetwork::POWERLINK::Core::ErrorHandling;
using namespace IndustrialNetwork::POWERLINK::Core::Utilities;
using namespace IndustrialNetwork::POWERLINK::Core::CoreConfiguration;

namespace IndustrialNetwork
{
	namespace POWERLINK
	{
		namespace Core
		{
			namespace Node
			{

				MnFeature::MnFeature(MNFeatureEnum type) : PlkFeature<MNFeatureEnum>(type)
				{
					SetTypedValues(PlkFeatureDefaultValues[type], "");
				}


				MnFeature::~MnFeature()
				{}

				const string MnFeature::GetName()
				{
					return PlkFeatureStrings[this->GetFeatureId()];
				}

				template<class T>
				Result MnFeature::GetDefaultValue(T& value)
				{
					if (this->GetUntypedDefaultValue().type() == typeid(T))
					{
						//return original stored value
						value =  boost::any_cast<T>(this->GetUntypedDefaultValue());
						return Result();
					}
					//Datatype does not match
					boost::format formatter(kMsgDatatypeMismatch);
					formatter
					% typeid(T).name()
					% this->GetUntypedDefaultValue().type().name();
					LOG_FATAL() << formatter.str();
					return Result(ErrorCode::DATATYPE_MISMATCH, formatter.str());
				}
				template Result MnFeature::GetDefaultValue(bool& value);
				template Result MnFeature::GetDefaultValue(uint8_t& value);
				template Result MnFeature::GetDefaultValue(uint16_t& value);
				template Result MnFeature::GetDefaultValue(uint32_t& value);


				Result MnFeature::SetTypedValues(string defaultValue, string actualValue)
				{
					switch (this->GetFeatureId())
					{
						case MNFeatureEnum::DLLErrMNMultipleMN:
						case MNFeatureEnum::DLLMNFeatureMultiplex:
						case MNFeatureEnum::DLLMNPResChaining:
						case MNFeatureEnum::DLLMNFeaturePResTx:
						case MNFeatureEnum::NMTMNBasicEthernet:
						case MNFeatureEnum::NMTNetTime:
						case MNFeatureEnum::NMTNetTimeIsRealTime:
						case MNFeatureEnum::NMTRelativeTime:
						case MNFeatureEnum::NMTSimpleBoot:
							{
								if (!defaultValue.empty())
								{
									bool value = StringToBool(defaultValue);
									this->SetUntypedDefaultValue(boost::any(value));
									break;
								}
								if (!actualValue.empty())
								{
									bool value = StringToBool(actualValue);
									this->SetUntypedActualValue(boost::any(value));
									break;
								}
							}
						case MNFeatureEnum::NMTMNMultiplCycMax:
							{
								if (!defaultValue.empty())
								{
									uint8_t value = HexToInt<uint8_t>(defaultValue);
									this->SetUntypedDefaultValue(boost::any(value));
									break;
								}
								if (!actualValue.empty())
								{
									uint8_t value = HexToInt<uint8_t>(actualValue);
									this->SetUntypedActualValue(boost::any(value));
									break;
								}
							}
						case MNFeatureEnum::NMTMNASnd2SoC:
						case MNFeatureEnum::NMTMNPRes2PReq:
						case MNFeatureEnum::NMTMNPRes2PRes:
						case MNFeatureEnum::NMTMNPResRx2SoA:
						case MNFeatureEnum::NMTMNPResTx2SoA:
						case MNFeatureEnum::NMTMNSoA2ASndTx:
						case MNFeatureEnum::NMTMNSoC2PReq:
							{
								if (!actualValue.empty())
								{
									uint32_t value = HexToInt<uint32_t>(actualValue);
									this->SetUntypedActualValue(boost::any(value));
									break;
								}
							}
						case MNFeatureEnum::PDOTPDOChannels:
							{
								if (!defaultValue.empty())
								{
									uint16_t value = HexToInt<uint16_t>(defaultValue);
									this->SetUntypedDefaultValue(boost::any(value));
									break;
								}
								if (!actualValue.empty())
								{
									uint16_t value = HexToInt<uint16_t>(actualValue);
									this->SetUntypedActualValue(boost::any(value));
									break;
								}
							}
						default:
							break;
					}
					return Result();
				}

				template<class T>
				Result MnFeature::GetActualValue(T& value)
				{
					if (this->GetUntypedActualValue().type() == typeid(T))
					{
						//return original stored value
						value =  boost::any_cast<T>(this->GetUntypedActualValue());
						return Result();
					}
					//Datatype does not match
					boost::format formatter(kMsgDatatypeMismatch);
					formatter
					% typeid(T).name()
					% this->GetUntypedActualValue().type().name();
					LOG_FATAL() << formatter.str();
					return Result(ErrorCode::DATATYPE_MISMATCH, formatter.str());
				}
				template Result MnFeature::GetActualValue(bool& value);
				template Result MnFeature::GetActualValue(uint8_t& value);
				template Result MnFeature::GetActualValue(uint16_t& value);
				template Result MnFeature::GetActualValue(uint32_t& value);

				template<class T>
				Result MnFeature::SetActualValue(const T actualValue)
				{
					if (this->GetUntypedActualValue().type() == typeid(T))
					{
						//return original stored value
						this->SetUntypedActualValue(boost::any(actualValue));
						return Result();
					}
					//Datatype does not match
					boost::format formatter(kMsgDatatypeMismatch);
					formatter
					% typeid(T).name()
					% this->GetUntypedActualValue().type().name();
					LOG_FATAL() << formatter.str();
					return Result(ErrorCode::DATATYPE_MISMATCH, formatter.str());
				}
				template Result MnFeature::SetActualValue(const bool value);
				template Result MnFeature::SetActualValue(const uint8_t value);
				template Result MnFeature::SetActualValue(const uint16_t value);
				template Result MnFeature::SetActualValue(const uint32_t value);
			}
		}
	}
}
