/************************************************************************
\file ControlledNode.h
\brief Implementation of the Class ControlledNode
\author rueckerc, Bernecker+Rainer Industrie Elektronik Ges.m.b.H.
\date 04-Feb-2015 09:38:08
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
#if !defined CONTROLLED_NODE_H
#define CONTROLLED_NODE_H

#include "BaseNode.h"
#include "PlkOperationMode.h"
#include "Result.h"
#include "BaseObject.h"
#include "Constants.h"
#include "Direction.h"
#include "Module.h"
#include "Range.h"
#include "Constants.h"

namespace IndustrialNetwork
{
	namespace POWERLINK
	{
		namespace Core
		{
			namespace Node
			{
				/**
				\brief Representes the Controlled Node in the POWERLINK network.
				\author rueckerc, Bernecker+Rainer Industrie Elektronik Ges.m.b.H.
				*/
				class DLLEXPORT ControlledNode : public IndustrialNetwork::POWERLINK::Core::Node::BaseNode
				{

					public:
						ControlledNode(std::uint8_t nodeID, const std::string nodeName = "");
						virtual ~ControlledNode();

						bool AddNodeAssignement(NodeAssignment);
						bool RemoveNodeAssignment(NodeAssignment);
						std::uint32_t GetNodeAssignmentValue();

						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result MapToFrame(IndustrialNetwork::POWERLINK::Core::ObjectDictionary::BaseObject& index, uint32_t position, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::Direction dir);
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetAllMappableObjects(std::vector<std::shared_ptr<IndustrialNetwork::POWERLINK::Core::ObjectDictionary::BaseObject>>& objects, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::Direction dir);
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result MapAllRxObjects();
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result MapAllTxObjects();

					private:
						IndustrialNetwork::POWERLINK::Core::Node::PlkOperationMode operationMode;
				};

			}

		}

	}

}
#endif