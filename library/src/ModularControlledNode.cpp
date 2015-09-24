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
using namespace IndustrialNetwork::POWERLINK::Core::ObjectDictionary;

ModularControlledNode::ModularControlledNode(std::uint8_t nodeID, const std::string& nodeName) : ControlledNode(nodeID, nodeName),
	interfaceList(std::vector<std::shared_ptr<IndustrialNetwork::POWERLINK::Core::ModularNode::Interface>>())
{}

ModularControlledNode::~ModularControlledNode()
{}

Result ModularControlledNode::AddInterface(const std::string& uniqueId, const std::string& type, ModuleAddressing moduleAddressing, std::uint32_t maxModules, bool unusedSlots, bool multipleModules)
{
	for (auto& interf : this->interfaceList)
	{
		if (interf->GetUniqueId() == uniqueId)
			return Result(ErrorCode::INTERFACE_ALREADY_EXISTS);
	}
	this->interfaceList.push_back(std::shared_ptr<Interface>(new Interface(uniqueId, type, moduleAddressing, maxModules, unusedSlots, multipleModules)));
	return Result();
}

Result ModularControlledNode::GetModule(const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition, std::shared_ptr<Module>& moduleRet)
{
	for (auto& interf : this->interfaceList)
	{
		if (interf->GetUniqueId() == interfaceId)
		{
			return interf->GetModule(moduleId, modulePosition, moduleRet);
		}
	}
	return Result(ErrorCode::INTERFACE_DOES_NOT_EXIST);
}

Result ModularControlledNode::AddModule(const std::string& interfaceId, const std::string& moduleId, const std::string& moduleType, ModuleAddressing addressing, std::uint32_t modulePosition, std::uint32_t moduleAddress, const std::string& moduleName, std::uint16_t minPosition, std::uint16_t maxPosition, std::uint16_t minAddress, std::uint16_t maxAddress, std::uint16_t maxCount)
{
	for (auto& interf : this->interfaceList)
	{
		if (interf->GetUniqueId() == interfaceId)
		{
			interf->AddModule(modulePosition, std::shared_ptr<Module>(new Module(this->GetNodeId(), moduleId, moduleType, addressing, moduleAddress, modulePosition, moduleName, minPosition, maxPosition, minAddress, maxAddress, maxCount)));
			return Result();
		}
	}
	return Result(ErrorCode::INTERFACE_DOES_NOT_EXIST);
}

Result ModularControlledNode::AddRange(const std::string& interfaceId, const std::string& name, std::uint32_t baseIndex, std::uint32_t maxIndex, std::uint32_t maxSubIndex, std::uint32_t sortStep, SortMode sortMode, SortNumber sortNumber, PDOMapping pdoMapping)
{
	for (auto& interf : this->interfaceList)
	{
		if (interf->GetUniqueId() == interfaceId)
		{
			if (sortNumber == SortNumber::ADDRESS && sortMode == SortMode::SUBINDEX)
				return Result(ErrorCode::RANGE_INVALID);
			std::shared_ptr<Range> range = std::shared_ptr<Range>(new Range(name, baseIndex, maxIndex, maxSubIndex, sortStep, sortMode, sortNumber, pdoMapping));
			interf->AddRange(range);
			return Result();
		}
	}
	return Result(ErrorCode::INTERFACE_DOES_NOT_EXIST);
}

Result ModularControlledNode::ChangeModuleOrderOnInterface(const std::string& interfaceId, const std::string& moduleId, std::uint32_t oldPos, std::uint32_t newPos)
{
	for (auto& interf : this->interfaceList)
	{
		if (interf->GetUniqueId() == interfaceId)
		{
			if (interf->IsPositionOccupied(newPos))
				return Result(ErrorCode::UNHANDLED_EXCEPTION);
			Result res = interf->ChangeModulePosition(moduleId, oldPos, newPos);
			if (!res.IsSuccessful())
				return res;
			return this->UpdateControlledNodeOd();
		}
	}
	return Result(ErrorCode::INTERFACE_DOES_NOT_EXIST);
}

Result ModularControlledNode::RemoveModule(const std::string& interfaceId, const std::string& moduleId, std::uint32_t position)
{
	for (auto& interf : this->interfaceList)
	{
		if (interf->GetUniqueId() == interfaceId)
		{
			if (interf->GetModuleCollection().find(position) != interf->GetModuleCollection().end())
			{
				for (auto param : interf->GetModuleCollection().find(position)->second->GetParameterNameMapping())
				{
					this->GetApplicationProcess()->RemoveParameter(param.second);
				}
			}
			else
				return Result(ErrorCode::MODULE_DOES_NOT_EXIST);

			Result res = interf->RemoveModule(moduleId, position);
			if (!res.IsSuccessful())
				return res;
			return this->UpdateControlledNodeOd();
		}
	}
	return Result(ErrorCode::INTERFACE_DOES_NOT_EXIST);
}

Result ModularControlledNode::AddObjectToModule(const std::string& interfaceId, std::uint32_t modulePosition, const std::string& moduleId, std::shared_ptr<IndustrialNetwork::POWERLINK::Core::ObjectDictionary::Object> obj, const std::string& rangeSelector, std::uint32_t& objectId)
{
	for (auto& inter : this->interfaceList)
	{
		if (inter->GetUniqueId() == interfaceId)
		{
			std::shared_ptr<Module> module;
			Result res = inter->GetModule(moduleId, modulePosition, module);
			if (!res.IsSuccessful())
				return res;

			std::shared_ptr<Range> range;
			res = inter->GetRange(rangeSelector, range);
			if (!res.IsSuccessful())
				return res;

			res = range->GetNextIndex(objectId, module->GetAddress());
			if (!res.IsSuccessful())
				return res;

			obj->SetRangeSelector(rangeSelector);

			res = module->AddObject(obj);
			if (!res.IsSuccessful())
				return res;

			//Change object Id before adding to nodes od
			obj->SetId(objectId);

			if (range->GetPdoMpapping() != PDOMapping::UNDEFINED)
				obj->SetPDOMapping(range->GetPdoMpapping());

			if (module->IsEnabled())
				return this->AddObject(obj);
			else
				return Result();
		}
	}
	return Result();
}

Result ModularControlledNode::AddSubObjectToModule(const std::string& interfaceId, std::uint32_t modulePosition, const std::string& moduleId, std::uint32_t& objectId, std::uint16_t& subObjectId, std::shared_ptr<IndustrialNetwork::POWERLINK::Core::ObjectDictionary::SubObject> subObj)
{
	for (auto& inter : this->interfaceList)
	{
		if (inter->GetUniqueId() == interfaceId)
		{
			std::shared_ptr<Module> module;
			Result res = inter->GetModule(moduleId, modulePosition, module);
			if (!res.IsSuccessful())
				return res;

			std::shared_ptr<Object> object;
			res = module->GetObject(objectId, object);
			if (!res.IsSuccessful())
				return res;

			std::shared_ptr<Range> range;
			res = inter->GetRange(object->GetRangeSelector().get(), range);
			if (!res.IsSuccessful())
				return res;

			std::uint32_t objectCurrentId = object->GetObjectId();
			res = range->GetNextSubIndex(objectCurrentId, subObjectId, module->GetAddress());
			if (!res.IsSuccessful())
				return res;

			res = module->AddSubObject(objectId, subObj);
			if (!res.IsSuccessful())
				return res;

			objectId = objectCurrentId;
			subObj->SetId(subObjectId);
			if (range->GetPdoMpapping() != PDOMapping::UNDEFINED)
				subObj->SetPDOMapping(range->GetPdoMpapping());

			if (module->IsEnabled())
				return this->AddSubObject(objectId, subObj);
			else
				return Result();
		}
	}
	return Result();
}

Result ModularControlledNode::EnableModule(const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition, bool enable)
{
	for (auto& interf : this->interfaceList)
	{
		if (interf->GetUniqueId() == interfaceId)
		{
			Result res = interf->EnableModule(moduleId, modulePosition, enable);
			if (!res.IsSuccessful())
				return res;
			return this->UpdateControlledNodeOd();
		}
	}
	return Result(ErrorCode::INTERFACE_DOES_NOT_EXIST);
}

Result ModularControlledNode::RemoveObjectFromOd(std::uint32_t objectId)
{
	if (this->GetObjectDictionary().find(objectId) != this->GetObjectDictionary().end())
	{
		auto obj = this->GetObjectDictionary().find(objectId);
		if (obj->second->GetRangeSelector().is_initialized())
			this->GetObjectDictionary().erase(objectId);
	}
	return Result();
}

Result ModularControlledNode::UpdateControlledNodeOd()
{
	for (auto& inter : this->interfaceList)
	{
		for (auto& range : inter->GetRangeList())
		{
			range->Reset();
		}

		for (auto& module : inter->GetModuleCollection())
		{
			for (auto& obj : module.second->GetObjectDictionary())
			{
				this->RemoveObjectFromOd(obj.first);
				std::shared_ptr<Range> range;
				Result res = inter->GetRange(obj.second->GetRangeSelector().get(), range);
				if (!res.IsSuccessful())
					return res;

				std::uint32_t objectId = obj.first;
				res = range->GetNextIndex(objectId, module.second->GetAddress());
				if (!res.IsSuccessful())
					return res;

				obj.second->SetId(objectId);

				if (module.second->IsEnabled())
				{
					this->AddObject(obj.second);
					if (!res.IsSuccessful())
						return res;
				}

				for (auto& subobj : obj.second->GetSubObjectDictionary())
				{
					std::uint16_t subObjectId = (std::uint16_t) subobj.first;
					res = range->GetNextSubIndex(objectId, subObjectId, module.second->GetAddress());
					if (!res.IsSuccessful())
						return res;

					subobj.second->SetId(subObjectId);

					res = module.second->AddSubObject(objectId, subobj.second);
					if (!res.IsSuccessful())
						return res;
				}
			}
		}
	}
	return Result();
}

Result ModularControlledNode::GetModuleObjectCurrentIndex(const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition, std::uint32_t& objectId, std::int32_t& subObjectId)
{
	for (auto& interf : this->interfaceList)
	{
		if (interf->GetUniqueId() == interfaceId)
		{
			std::shared_ptr<Module> module;
			Result res = interf->GetModule(moduleId, modulePosition, module);
			if (!res.IsSuccessful())
				return res;

			auto& moduleOd = module->GetObjectDictionary();
			if (moduleOd.find(objectId) != moduleOd.end())
			{
				if (subObjectId == -1)
				{
					objectId = moduleOd.find(objectId)->second->GetObjectId();
					return Result();
				}
				else
				{
					if (moduleOd.find(objectId)->second->GetSubObjectDictionary().find(subObjectId) != moduleOd.find(objectId)->second->GetSubObjectDictionary().end())
					{
						subObjectId = moduleOd.find(objectId)->second->GetSubObjectDictionary().find(subObjectId)->second->GetObjectId();
						objectId = moduleOd.find(objectId)->second->GetObjectId();
						return Result();
					}
					else
						return Result(ErrorCode::SUBOBJECT_DOES_NOT_EXIST);
				}
			}
			else
				return Result(ErrorCode::OBJECT_DOES_NOT_EXIST);
		}
	}
	return Result(ErrorCode::INTERFACE_DOES_NOT_EXIST);
}

Result ModularControlledNode::GetParameterCurrentName(const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition, std::string& parameterName)
{
	for (auto& interf : this->interfaceList)
	{
		if (interf->GetUniqueId() == interfaceId)
		{
			std::shared_ptr<Module> module;
			Result res = interf->GetModule(moduleId, modulePosition, module);
			if (!res.IsSuccessful())
				return res;

			if (module->GetParameterNameMapping().find(parameterName) != module->GetParameterNameMapping().end())
			{
				parameterName = module->GetParameterNameMapping().find(parameterName)->second;
			}
			else
				return Result(ErrorCode::PARAMETER_NOT_FOUND);
		}
	}
	return Result(ErrorCode::INTERFACE_DOES_NOT_EXIST);
}