/************************************************************************
\file ModularControlledNode.cpp
\brief Implementation of the Class ModularControlledNode
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
#include "ModularControlledNode.h"

using namespace IndustrialNetwork::POWERLINK::Core::Node;
using namespace IndustrialNetwork::POWERLINK::Core::ModularNode;
using namespace IndustrialNetwork::POWERLINK::Core::ErrorHandling;

ModularControlledNode::ModularControlledNode(std::uint8_t nodeID, const std::string& nodeName) : ControlledNode(nodeID, nodeName),
	maxChildren(),
	unusedSlots(false),
	multipleChildren(false),
	rangeList(std::vector<std::shared_ptr<Range>>()),
	moduleCollection(std::vector<std::shared_ptr<Module>>())
{}

ModularControlledNode::~ModularControlledNode()
{}

Result AddModule(const Module&)
{
	return Result();
}

Result ChangeModuleOrder(const Module&, std::uint32_t, bool)
{
	return Result();
}

Result RemoveModule(const Module&, bool)
{
	return Result();
}

bool ModularControlledNode::RecalculateModularNodeOd()
{
	return false;
}

std::uint32_t ModularControlledNode::GetModuleCount()
{
	return 0;
}
