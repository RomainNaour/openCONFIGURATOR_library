/************************************************************************
\file ConfigurationGenerator.cpp
\brief Implementation of the Class ConfigurationGenerator
\author rueckerc, Bernecker+Rainer Industrie Elektronik Ges.m.b.H.
\date 24-Feb-2015 13:06:38
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
#include "ConfigurationGenerator.h"

using namespace IndustrialNetwork::POWERLINK::Core::NetworkHandling;
using namespace IndustrialNetwork::POWERLINK::Core::ErrorHandling;
using namespace IndustrialNetwork::POWERLINK::Core::Node;
using namespace IndustrialNetwork::POWERLINK::Core::ObjectDictionary;
using namespace IndustrialNetwork::POWERLINK::Core::CoreConfiguration;
using namespace IndustrialNetwork::POWERLINK::Core::Utilities;

ConfigurationGenerator::ConfigurationGenerator()
{}

ConfigurationGenerator::~ConfigurationGenerator()
{}

ConfigurationGenerator& ConfigurationGenerator::GetInstance()
{
	static ConfigurationGenerator instance;
	return instance;
}

void ConfigurationGenerator::WriteHeader(const std::shared_ptr<Network>& net, std::stringstream& configurationOutput)
{
	std::ostringstream dateTime;
	const boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
	//No deletion needed : http://stackoverflow.com/questions/17779660/who-is-responsible-for-deleting-the-facet
	boost::posix_time::time_facet* const f = new boost::posix_time::time_facet("%d-%b-%Y %H:%M:%S");
	dateTime.imbue(std::locale(dateTime.getloc(), f));
	dateTime << now;

	std::map<std::uint8_t, std::shared_ptr<BaseNode>> nodes;
	net->GetNodes(nodes);

	configurationOutput << "////" << std::endl;
	configurationOutput << "//// This file was autogenerated by openCONFIGURATOR-";
	configurationOutput << kVersionMajor << "." << kVersionMinor << "." << kVersionFix << "_" << kReleaseType << " on " << dateTime.str() << std::endl;
	configurationOutput << "//// Project:\t" << net->GetNetworkId() << std::endl;
	configurationOutput << "//// NodeCount:\t" << nodes.size() - 1 << std::endl;
	configurationOutput << "////" << std::endl;
}

Result ConfigurationGenerator::GenerateNetworkConfiguration(const std::shared_ptr<Network>& net, std::stringstream& configurationOutput, std::stringstream& hexOutput)
{
	//Write Header
	WriteHeader(net, configurationOutput);

	//Write the active managing node object count including Assignments and 1F22 objects
	Result res = WriteManagingNodeObjectCount(net, configurationOutput, hexOutput);
	if (!res.IsSuccessful())
		return res;

	//Write the node assignments
	res = WriteNodeAssignment(net, configurationOutput, hexOutput, false, true);
	if (!res.IsSuccessful())
		return res;
	configurationOutput << std::endl;

	//Write the managing node configuration
	res = WriteManagingNodeConfiguration(net, configurationOutput, hexOutput);
	if (!res.IsSuccessful())
		return res;

	configurationOutput << std::endl;
	std::map<std::uint8_t, std::shared_ptr<BaseNode>> nodes;
	res = net->GetNodes(nodes);
	if (!res.IsSuccessful())
		return res;

	//Write CN configuration including RMNs
	for (auto& node : nodes)
	{
		if (node.first == 240) //skip MN already written
			continue;

		if (node.second->IsEnabled() == false)
			continue;

		//Write controlled node or RMN configuration
		res = WriteControlledNodeConfiguration(net, node.second, configurationOutput, hexOutput);
		if (!res.IsSuccessful())
			return res;
	}

	//Write the node reassignment
	res = WriteNodeAssignment(net, configurationOutput, hexOutput, true, true);
	return res;
}

Result ConfigurationGenerator::WriteManagingNodeObjectCount(const std::shared_ptr<Network>& net, std::stringstream& configurationOutput, std::stringstream& hexOutput)
{
	//Write Managing Node Object Count
	std::shared_ptr<ManagingNode> mn;
	Result res = net->GetManagingNode(mn);
	if (res.IsSuccessful())
	{
		configurationOutput << std::setfill('0') << std::setw(8) << std::hex << std::uppercase << mn->GetConfigurationObjectCount() << std::endl;
		hexOutput << ReverseHex(mn->GetConfigurationObjectCount(), 8);
	}
	LOG_INFO() << kMsgWriteManagingNodeObjectCount;
	return res;
}

Result ConfigurationGenerator::WriteNodeAssignment(const std::shared_ptr<Network>& net, std::stringstream& configurationOutput, std::stringstream& hexOutput, bool writeNodeValid, bool writeComments)
{
	std::map<std::uint8_t, std::shared_ptr<BaseNode>> nodes;
	Result res = net->GetNodes(nodes);
	if (!res.IsSuccessful())
		return res;

	for (auto& node : nodes)
	{
		if (node.second->IsEnabled() == false)
			continue;

		//Determine if Node is RMN
		if (std::dynamic_pointer_cast<ManagingNode>(node.second))
		{
			//Skip if node is RMN and write Reassignment or node is AMN
			if ((writeNodeValid && node.second->GetNodeId() != 240)
			        || node.second->GetNodeId() == 240) //Do not write Node Reassignment for RMNscontinue;
				continue;
		}

		//Write Assignment with no node valid bit therefore remove the bit first
		if (!writeNodeValid)
			node.second->RemoveNodeAssignment(NodeAssignment::MNT_NODEASSIGN_VALID);

		//Write comments according to writeComments flag
		if (!writeNodeValid && writeComments)
			configurationOutput << "//// NodeId Assignment" << std::endl;
		else if (writeNodeValid && writeComments)
			configurationOutput << "//// NodeId Reassignment" << std::endl;


		configurationOutput << std::hex << std::uppercase << 0x1F81;
		configurationOutput << "\t";
		configurationOutput << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (std::uint32_t) node.first;
		configurationOutput << "\t";
		configurationOutput << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << 0x4;
		configurationOutput << "\t";
		configurationOutput << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << node.second->GetNodeAssignmentValue() << std::endl;

		hexOutput << ReverseHex(0x1F81, 4);
		hexOutput << ReverseHex((std::uint32_t) node.first, 2);
		hexOutput << ReverseHex(0x4, 8);
		hexOutput << ReverseHex(node.second->GetNodeAssignmentValue(), 8);

		//If valid bit has been remove add it again
		if (!writeNodeValid)
			node.second->AddNodeAssignment(NodeAssignment::MNT_NODEASSIGN_VALID);
	}
	LOG_INFO() << kMsgWriteNodeAssignment;
	return res;
}

Result ConfigurationGenerator::WriteManagingNodeConfiguration(const std::shared_ptr<Network>& net, std::stringstream& configurationOutput, std::stringstream& hexOutput)
{
	//Write Managing Node Object Count
	std::shared_ptr<ManagingNode> mn;
	Result res = net->GetManagingNode(mn);
	if (!res.IsSuccessful())
		return res;

	//Reset Mapping objects
	res = WriteMappingNrOfEntriesZero(mn, configurationOutput, hexOutput);
	if (!res.IsSuccessful())
		return res;

	//Write Communication Profile area
	res = WriteCommunicationProfileArea(mn, configurationOutput, hexOutput);
	if (!res.IsSuccessful())
		return res;

	//Write Mapping objects
	res = WriteMappingObjects(mn, configurationOutput, hexOutput);
	if (!res.IsSuccessful())
		return res;

	//Write manufacturer specific objects
	res = WriteManufacturerSpecificProfileArea(mn, configurationOutput, hexOutput);
	if (!res.IsSuccessful())
		return res;

	//Write mapping number of entries
	res = WriteMappingNrOfEntries(mn, configurationOutput, hexOutput);

	boost::format formatter(kMsgWriteManagingNode);
	formatter
	% (std::uint32_t) mn->GetNodeId();
	LOG_INFO() << formatter.str();
	return res;
}

Result ConfigurationGenerator::WriteRedundantManagingNodeConfiguration(const std::shared_ptr<Network>& net, const std::shared_ptr<BaseNode>& node, std::stringstream& configurationOutput, std::stringstream& hexOutput)
{
	auto rmn = std::dynamic_pointer_cast<ManagingNode>(node); //This should not fail because it can either be MN or CN

	// BitSize / 8 + 7 Byte per Object (2 Byte Index / 1 Byte SubIndex / 4 Byte Size) + 4 Byte NrOfObjects
	std::uint32_t rmnDomainSize = (rmn->GetConfigurationObjectSize() / 8) + (rmn->GetConfigurationObjectCount() * 7) + 4;

	//Write the 1F22 object for RMN
	configurationOutput << "////Configuration Data for CN: " << node->GetName() << "(" << std::dec << (std::uint32_t) node->GetNodeId() << ")" << std::endl;
	configurationOutput << std::hex << std::uppercase << 0x1F22;
	configurationOutput << "\t";
	configurationOutput << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (std::uint32_t) node->GetNodeId();
	configurationOutput << "\t";
	configurationOutput << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << rmnDomainSize << std::endl;
	configurationOutput << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << rmn->GetConfigurationObjectCount() << std::endl;

	hexOutput << ReverseHex(0x1F22, 4);
	hexOutput << ReverseHex((std::uint32_t) node->GetNodeId(), 2);
	hexOutput << ReverseHex(rmnDomainSize, 8);
	hexOutput << ReverseHex(rmn->GetConfigurationObjectCount(), 8);

	Result res = WriteNodeAssignment(net, configurationOutput, hexOutput, false, false);
	if (!res.IsSuccessful())
		return res;

	res = WriteMappingNrOfEntriesZero(rmn, configurationOutput, hexOutput);
	if (!res.IsSuccessful())
		return res;

	res = WriteCommunicationProfileArea(rmn, configurationOutput, hexOutput);
	if (!res.IsSuccessful())
		return res;

	res = WriteMappingObjects(rmn, configurationOutput, hexOutput);
	if (!res.IsSuccessful())
		return res;

	res = WriteManufacturerSpecificProfileArea(rmn, configurationOutput, hexOutput);
	if (!res.IsSuccessful())
		return res;

	res = WriteMappingNrOfEntries(rmn, configurationOutput, hexOutput);
	if (!res.IsSuccessful())
		return res;

	res = WriteNodeAssignment(net, configurationOutput, hexOutput, true, false);
	configurationOutput << std::endl;

	boost::format formatter(kMsgWriteRedundantManagingNode);
	formatter
	% (uint32_t) rmn->GetNodeId();
	LOG_INFO() << formatter.str();
	return res;
}

Result ConfigurationGenerator::WriteControlledNodeConfiguration(const std::shared_ptr<Network>& net, const std::shared_ptr<BaseNode>& node, std::stringstream& configurationOutput, std::stringstream& hexOutput)
{
	auto cn = std::dynamic_pointer_cast<ControlledNode>(node);
	if (!cn) //If cast fails this is an RMN
		return WriteRedundantManagingNodeConfiguration(net, node, configurationOutput, hexOutput);

	// BitSize / 8 + 7 Byte per Object (2 Byte Index / 1 Byte SubIndex / 4 Byte Size) + 4 Byte NrOfObjects
	std::uint32_t cnDomainSize = (cn->GetConfigurationObjectSize() / 8) + (cn->GetConfigurationObjectCount() * 7) + 4;

	configurationOutput << "////Configuration Data for CN: " << node->GetName() << "(" << std::dec << (std::uint32_t) node->GetNodeId() << ")" << std::endl;
	configurationOutput << std::hex << std::uppercase << "1F22";
	configurationOutput << "\t";
	configurationOutput << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (std::uint32_t) node->GetNodeId();
	configurationOutput << "\t";
	configurationOutput << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << cnDomainSize << std::endl;
	configurationOutput << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << cn->GetConfigurationObjectCount() << std::endl;

	hexOutput << ReverseHex(0x1F22, 4);
	hexOutput << ReverseHex((std::uint32_t) node->GetNodeId(), 2);
	hexOutput << ReverseHex(cnDomainSize, 8);
	hexOutput << ReverseHex(cn->GetConfigurationObjectCount(), 8);

	Result res = WriteMappingNrOfEntriesZero(node, configurationOutput, hexOutput);
	if (!res.IsSuccessful())
		return res;

	res = WriteCommunicationProfileArea(node, configurationOutput, hexOutput);
	if (!res.IsSuccessful())
		return res;

	res = WriteManufacturerSpecificProfileArea(node, configurationOutput, hexOutput);
	if (!res.IsSuccessful())
		return res;

	res = WriteMappingObjects(node, configurationOutput, hexOutput);
	if (!res.IsSuccessful())
		return res;

	res = WriteMappingNrOfEntries(node, configurationOutput, hexOutput);
	configurationOutput << std::endl;

	boost::format formatter(kMsgWriteControlledNode);
	formatter
	% (std::uint32_t) cn->GetNodeId();
	LOG_INFO() << formatter.str();
	return res;
}

Result ConfigurationGenerator::WriteMappingNrOfEntriesZero(const std::shared_ptr<BaseNode>& node, std::stringstream& configurationOutput, std::stringstream& hexOutput)
{
	for (auto& object : node->GetObjectDictionary())
	{
		if ((object.first >= 0x1600 && object.first < 0x1700) || (object.first >= 0x1A00 && object.first < 0x1B00))
		{
			auto subobject = object.second->GetSubObjectDictionary().find((std::uint32_t) 0);
			if (subobject != object.second->GetSubObjectDictionary().end())
			{
				if (subobject->second->WriteToConfiguration())
				{
					if (subobject->second->GetTypedActualValue<std::uint16_t>() == 0)
						continue;
				}
				else
					continue;

				configurationOutput << std::hex << std::uppercase << object.first;
				configurationOutput << "\t";
				configurationOutput << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << subobject->first;
				configurationOutput << "\t";
				configurationOutput << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << (subobject->second->GetBitSize() / 8);
				configurationOutput << "\t";
				configurationOutput << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << 0 << std::endl;

				hexOutput << ReverseHex(object.first, 4);
				hexOutput << ReverseHex(subobject->first, 2);
				hexOutput << ReverseHex(subobject->second->GetBitSize() / 8, 8);
				hexOutput << ReverseHex(0x0, 2);
			}
		}
	}

	boost::format formatter(kMsgWriteMappingObjectsNrOfEntriesZero);
	formatter
	% (std::uint32_t) node->GetNodeId();
	LOG_INFO() << formatter.str();
	return Result();
}

Result ConfigurationGenerator::WriteMappingObjects(const std::shared_ptr<BaseNode>& node, std::stringstream& configurationOutput, std::stringstream& hexOutput)
{
	for (auto& object : node->GetObjectDictionary())
	{
		if ((object.first >= 0x1800 && object.first < 0x1900)
		        || (object.first >= 0x1400 && object.first < 0x1500))
		{
			for (auto& subobject : object.second->GetSubObjectDictionary())
			{
				if (subobject.second->WriteToConfiguration() && (subobject.first == 0x1 || subobject.first == 0x2))
				{
					configurationOutput << std::hex << std::uppercase << object.first;
					configurationOutput << "\t";
					configurationOutput << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << subobject.first;
					configurationOutput << "\t";
					configurationOutput << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << (subobject.second->GetBitSize() / 8);
					configurationOutput << "\t";
					configurationOutput << subobject.second->GetTypedActualValue<std::string>() << std::endl;

					hexOutput << ReverseHex(object.first, 4);
					hexOutput << ReverseHex(subobject.first, 2);
					hexOutput << ReverseHex(subobject.second->GetBitSize() / 8, 8);
					hexOutput << ReverseHex(subobject.second->GetTypedActualValue<std::string>());
				}
			}
		}
		else if ((object.first >= 0x1600 && object.first < 0x1700)
		         || (object.first >= 0x1A00 && object.first < 0x1B00))
		{
			std::uint16_t numberOfIndicesToWrite = 0;
			auto& nrOfEntriesObj = object.second->GetSubObjectDictionary().at((std::uint8_t) 0);
			if (nrOfEntriesObj->WriteToConfiguration())
				numberOfIndicesToWrite = nrOfEntriesObj->GetTypedActualValue<std::uint16_t>();

			if (numberOfIndicesToWrite != 0)
			{
				std::uint16_t count = 0;
				for (auto& subobject : object.second->GetSubObjectDictionary())
				{
					if (subobject.first == 0)
						continue;

					if (subobject.second->WriteToConfiguration())
					{
						if (subobject.second->GetTypedActualValue<std::uint64_t>() == 0)
							continue;
					}
					else
						continue;

					if (count < numberOfIndicesToWrite)
					{
						configurationOutput << std::hex << std::uppercase << object.first;
						configurationOutput << "\t";
						configurationOutput << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << subobject.first;
						configurationOutput << "\t";
						configurationOutput << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << (subobject.second->GetBitSize() / 8);
						configurationOutput << "\t";
						configurationOutput << subobject.second->GetTypedActualValue<std::string>() << std::endl;

						hexOutput << ReverseHex(object.first, 4);
						hexOutput << ReverseHex(subobject.first, 2);
						hexOutput << ReverseHex(subobject.second->GetBitSize() / 8, 8);
						hexOutput << ReverseHex(subobject.second->GetTypedActualValue<std::string>());

						count++;
					}
				}
			}
		}
	}

	boost::format formatter(kMsgWriteMappingObjects);
	formatter
	% (std::uint32_t) node->GetNodeId();
	LOG_INFO() << formatter.str();
	return Result();
}

Result ConfigurationGenerator::WriteMappingNrOfEntries(const std::shared_ptr<BaseNode>& node, std::stringstream& configurationOutput, std::stringstream& hexOutput)
{
	for (auto& object : node->GetObjectDictionary())
	{
		if ((object.first >= 0x1600 && object.first < 0x1700) || (object.first >= 0x1A00 && object.first < 0x1B00))
		{
			auto subobject = object.second->GetSubObjectDictionary().find((std::uint32_t) 0);
			if (subobject != object.second->GetSubObjectDictionary().end())
			{
				if (subobject->second->WriteToConfiguration())
				{
					if (subobject->second->GetTypedActualValue<std::uint16_t>() == 0)
						continue;
				}
				else
					continue;

				configurationOutput << std::hex << std::uppercase << object.first;
				configurationOutput << "\t";
				configurationOutput << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << subobject->first;
				configurationOutput << "\t";
				configurationOutput << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << (subobject->second->GetBitSize() / 8);
				configurationOutput << "\t";
				configurationOutput << std::hex << std::uppercase << subobject->second->GetTypedActualValue<std::string>() << std::endl;

				hexOutput << ReverseHex(object.first, 4);
				hexOutput << ReverseHex(subobject->first, 2);
				hexOutput << ReverseHex(subobject->second->GetBitSize() / 8, 8);
				hexOutput << ReverseHex(subobject->second->GetTypedActualValue<std::string>());
			}
		}
	}

	boost::format formatter(kMsgWriteMappingObjectsNrOfEntries);
	formatter
	% (std::uint32_t) node->GetNodeId();
	LOG_INFO() << formatter.str();
	return Result();
}

Result ConfigurationGenerator::WriteCommunicationProfileArea(const std::shared_ptr<BaseNode>& node, std::stringstream& configurationOutput, std::stringstream& hexOutput)
{
	for (auto& object : node->GetObjectDictionary())
	{
		if ((object.first < 0x1600 || object.first > 0x16FF)
		        && (object.first < 0x1A00 || object.first > 0x1AFF)
		        && (object.first < 0x1400 || object.first > 0x14FF)
		        && (object.first < 0x1800 || object.first > 0x18FF)
		        && object.first < 0x2000)
		{
			if (object.second->WriteToConfiguration())
			{
				configurationOutput << std::hex << std::uppercase << object.first;
				configurationOutput << "\t";
				configurationOutput << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << 0;
				configurationOutput << "\t";
				configurationOutput << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << (object.second->GetBitSize() / 8);
				configurationOutput << "\t";
				configurationOutput << object.second->GetTypedActualValue<std::string>() << std::endl;

				hexOutput << ReverseHex(object.first, 4);
				hexOutput << ReverseHex(0x0, 2);
				hexOutput << ReverseHex(object.second->GetBitSize() / 8, 8);
				hexOutput << ReverseHex(object.second->GetTypedActualValue<std::string>());
			}

			for (auto& subobject : object.second->GetSubObjectDictionary())
			{
				//Write MN node assignment only when PResMN is set
				if (object.first == 0x1F81 && subobject.first == 240 && node->GetNodeId() >= 240)
				{
					std::shared_ptr<ManagingNode> mn = std::dynamic_pointer_cast<ManagingNode>(node);
					if (mn)
					{
						if (std::find(mn->GetNodeAssignment().begin(), mn->GetNodeAssignment().end(), NodeAssignment::NMT_NODEASSIGN_MN_PRES) == mn->GetNodeAssignment().end())
							continue;
					}
				}
				else if (object.first == 0x1F81 && subobject.first != 240 && node->GetNodeId() >= 240)
					continue;

				if (subobject.second->WriteToConfiguration())
				{
					configurationOutput << std::hex << std::uppercase << object.first;
					configurationOutput << "\t";
					configurationOutput << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << subobject.first;
					configurationOutput << "\t";
					configurationOutput << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << (subobject.second->GetBitSize() / 8);
					configurationOutput << "\t";
					configurationOutput << subobject.second->GetTypedActualValue<std::string>() << std::endl;

					hexOutput << ReverseHex(object.first, 4);
					hexOutput << ReverseHex(subobject.first, 2);
					hexOutput << ReverseHex(subobject.second->GetBitSize() / 8, 8);
					hexOutput << ReverseHex(subobject.second->GetTypedActualValue<std::string>());
				}
			}
		}
	}

	boost::format formatter(kMsgWriteCommunicationRangeObjects);
	formatter
	% (std::uint32_t) node->GetNodeId();
	LOG_INFO() << formatter.str();
	return Result();
}

Result ConfigurationGenerator::WriteManufacturerSpecificProfileArea(const std::shared_ptr<BaseNode>& node, std::stringstream& configurationOutput, std::stringstream& hexOutput)
{
	for (auto& object : node->GetObjectDictionary())
	{
		if (object.first >= 0x2000 && object.first <= 0x9FFF)
		{
			if (object.second->WriteToConfiguration())
			{
				configurationOutput << std::hex << std::uppercase << object.first;
				configurationOutput << "\t";
				configurationOutput << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << 0;
				configurationOutput << "\t";
				configurationOutput << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << (object.second->GetBitSize() / 8);
				configurationOutput << "\t";
				configurationOutput << object.second->GetTypedActualValue<std::string>() << std::endl;

				hexOutput << ReverseHex(object.first, 4);
				hexOutput << ReverseHex(0x0, 2);
				hexOutput << ReverseHex(object.second->GetBitSize() / 8, 8);
				hexOutput << ReverseHex(object.second->GetTypedActualValue<std::string>());
			}

			for (auto& subobject : object.second->GetSubObjectDictionary())
			{
				if (subobject.second->WriteToConfiguration())
				{
					configurationOutput << std::hex << std::uppercase << object.first;
					configurationOutput << "\t";
					configurationOutput << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << subobject.first;
					configurationOutput << "\t";
					configurationOutput << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << (subobject.second->GetBitSize() / 8);
					configurationOutput << "\t";
					configurationOutput << subobject.second->GetTypedActualValue<std::string>() << std::endl;

					hexOutput << ReverseHex(object.first, 4);
					hexOutput << ReverseHex(subobject.first, 2);
					hexOutput << ReverseHex(subobject.second->GetBitSize() / 8, 8);
					hexOutput << ReverseHex(subobject.second->GetTypedActualValue<std::string>());
				}
			}
		}
	}

	boost::format formatter(kMsgWriteUserDefinedRangeObjects);
	formatter
	% (std::uint32_t) node->GetNodeId();
	LOG_INFO() << formatter.str();
	return Result();
}