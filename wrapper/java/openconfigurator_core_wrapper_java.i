/*################################################################################
#
# Project: openCONFIGURATOR core library Java wrapper interface
#
# (c) Bernecker + Rainer Industrie-Elektronik Ges.m.b.H.
#     B&R Strasse 1, A-5142 Eggelsberg
#     www.br-automation.com
#
#
# License:
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#
#   1. Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#
#   2. Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#
#   3. Neither the name of the copyright holders nor the names of its
#      contributors may be used to endorse or promote products derived
#      from this software without prior written permission. For written
#      permission, please contact office@br-automation.com.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
#   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
#   COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
#   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
#   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
#   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#   POSSIBILITY OF SUCH DAMAGE.
#
#   Severability Clause:
#
#       If a provision of this License is or becomes illegal, invalid or
#       unenforceable in any jurisdiction, that shall not affect:
#       1. the validity or enforceability in that jurisdiction of any other
#          provision of this License; or
#       2. the validity or enforceability in other jurisdictions of that or
#          any other provision of this License.
#
################################################################################*/
%module openconfigurator_core_wrapper_java

#define DLLEXPORT
#define __attribute__(x)
#if defined(_WIN32) && defined(_MSC_VER)
	 #undef DllExport
	 #define DllExport  __declspec(dllexport)
#endif

%include typemaps.i
%include std_map.i
%include std_except.i
%include std_string.i
%include std_vector.i
%include <std_shared_ptr.i>
%include "enums.swg"
%include windows.i
%include std_string_ref.i
%include stdint.i

%apply std::string &INOUT { std::string & configuration };
%apply std::string &INOUT { std::string & networkUuid };

%include "../Include/IBuildConfigurationSetting.h"
%include "../Include/BuildConfigurationSetting.h"
%include "../Include/ErrorCode.h"
%include "../Include/AccessType.h"
%include "../Include/ObjectType.h"
%include "../Include/PDOMapping.h"
%include "../Include/PlkDataType.h"
%include "../Include/IBuildConfigurationSetting.h"
%include "../Include/NodeAssignment.h"
%include "../Include/Result.h"
%include "../Include/PlkConfiguration.h"
%nodefaultctor BaseNode;
%include "../Include/BaseNode.h"
%include "../Include/BaseObject.h"
%nodefaultctor Object;
%include "../Include/Object.h"
%nodefaultctor SubObject;
%include "../Include/SubObject.h"
%include "../Include/Direction.h"
%include "../Include/NodeType.h"
%nodefaultctor ControlledNode;
%include "../Include/ControlledNode.h"
%include "../Include/ManagingNode.h"
%include "../Include/ProjectManager.h"
%include "../Include/Network.h"
%nodefaultctor PlkConfiguration;
%include "../Include/PlkConfiguration.h"

%{

#include "../Include/IBuildConfigurationSetting.h"
#include "../Include/BuildConfigurationSetting.h"
#include "../Include/ErrorCode.h"
#include "../Include/AccessType.h"
#include "../Include/ObjectType.h"
#include "../Include/PDOMapping.h"
#include "../Include/PlkDataType.h"
#include "../Include/IBuildConfigurationSetting.h"
#include "../Include/NodeAssignment.h"
#include "../Include/Result.h"
#include "../Include/PlkConfiguration.h"
#include "../Include/BaseNode.h"
#include "../Include/BaseObject.h"
#include "../Include/Object.h"
#include "../Include/SubObject.h"
#include "../Include/Direction.h"
#include "../Include/NodeType.h"
#include "../Include/ControlledNode.h"
#include "../Include/ManagingNode.h"
#include "../Include/ProjectManager.h"
#include "../Include/Network.h"
#include "../Include/PlkConfiguration.h"

%}

namespace std {
   %template(ObjectCollection) std::vector<IndustrialNetwork::POWERLINK::Core::ObjectDictionary::BaseObject>;
   %template(NetworkCollection) std::map<std::string, IndustrialNetwork::POWERLINK::Core::NetworkHandling::Network*>;
   %template(StringCollection) std::vector<std::string>;
   %template(SettingsCollection) std::vector<IndustrialNetwork::POWERLINK::Core::Configuration::BuildConfigurationSetting>;
   %template(NodeIdCollection) std::vector<std::uint8_t>;
   %template(ConfigurationCollection) std::vector<IndustrialNetwork::POWERLINK::Core::Configuration::PlkConfiguration>;
}
