/************************************************************************
\file BaseProcessImageObject.cpp
\brief Implementation of the Class BaseProcessImageObject
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
#include "BaseProcessImageObject.h"

using namespace IndustrialNetwork::POWERLINK::Core::ObjectDictionary;
using namespace IndustrialNetwork::POWERLINK::Core::Utilities;

BaseProcessImageObject::BaseProcessImageObject(const std::string& name, IEC_Datatype dataType, std::uint32_t piOffset, std::uint32_t size) :
	name(name),
	dataType(dataType),
	size(size),
	piOffset(piOffset),
	bitOffset(boost::optional<std::uint32_t>())
{
	if (size == 0)
		this->size = GetIECDataTypeBitSize(this->dataType);
}

BaseProcessImageObject::BaseProcessImageObject(const std::string& name, IEC_Datatype dataType, std::uint32_t piOffset, std::uint32_t bitOffset, std::uint32_t size) :
	name(name),
	dataType(dataType),
	size(size),
	piOffset(piOffset),
	bitOffset(bitOffset)
{
	if (size == 0)
		this->size = GetIECDataTypeBitSize(this->dataType);
}

BaseProcessImageObject::~BaseProcessImageObject()
{}

const std::string& BaseProcessImageObject::GetName()
{
	return this->name;
}

IEC_Datatype BaseProcessImageObject::GetDataType()
{
	return this->dataType;
}

std::uint32_t BaseProcessImageObject::GetSize()
{
	return this->size;
}

std::uint32_t BaseProcessImageObject::GetPiOffset()
{
	return this->piOffset;
}

const boost::optional<std::uint32_t>& BaseProcessImageObject::GetBitOffset()
{
	return this->bitOffset;
}

void BaseProcessImageObject::SetPIOffset(std::uint32_t piOffset)
{
	this->piOffset = piOffset;
}

void BaseProcessImageObject::SetName(const std::string& name)
{
	this->name = name;
}
