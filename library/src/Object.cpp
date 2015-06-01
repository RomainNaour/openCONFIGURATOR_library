/************************************************************************
\file Index.cpp
\brief Implementation of the Class Index
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
#include "Object.h"

using namespace std;
using namespace IndustrialNetwork::POWERLINK::Core::Utilities;
using namespace IndustrialNetwork::POWERLINK::Core::ErrorHandling;

namespace IndustrialNetwork
{
	namespace POWERLINK
	{
		namespace Core
		{
			namespace ObjectDictionary
			{
				Object::Object(uint32_t id, PlkDataType type) : BaseObject(id, type),
					subIndexCollection(unordered_map<uint32_t, shared_ptr<SubObject>>())
				{}

				Object::Object(uint32_t id, PlkDataType type, AccessType accessType, ObjectType objectType, PDOMapping pdoMapping, string defaultValue, string actualValue, uint32_t highlimit, uint32_t lowLimit, string uniqueIdRef, string name): BaseObject(id, type, accessType, objectType, pdoMapping, defaultValue, actualValue, highlimit, lowLimit, uniqueIdRef, name),
					subIndexCollection(unordered_map<uint32_t, shared_ptr<SubObject>>())
				{}

				Object::~Object()
				{}

				IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result Object::AddSubobject(shared_ptr<SubObject>& ref)
				{
					if (this->subIndexCollection.find(ref.get()->GetId()) == this->subIndexCollection.end())
						return Result(ErrorCode::SUBOBJECT_EXISTS);

					this->subIndexCollection.insert(pair<uint32_t, shared_ptr<SubObject>>(ref.get()->GetId(), ref));

					return Result();
				}
			}
		}
	}
}