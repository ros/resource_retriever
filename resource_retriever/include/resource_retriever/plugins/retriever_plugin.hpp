// Copyright 2009, Willow Garage, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the  Willow Garage, Inc. nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef RESOURCE_RETRIEVER__PLUGINS__RETRIEVER_PLUGIN_HPP_
#define RESOURCE_RETRIEVER__PLUGINS__RETRIEVER_PLUGIN_HPP_

#include <string>

#include <resource_retriever/resource.hpp>
#include "resource_retriever/visibility_control.hpp"

namespace resource_retriever::plugins
{
std::string expand_package_url(const std::string & url);
std::string escape_spaces(const std::string & url);

class RESOURCE_RETRIEVER_PUBLIC RetrieverPlugin
{
public:
  RetrieverPlugin();
  virtual ~RetrieverPlugin() = 0;

  virtual std::string name() = 0;
  virtual bool can_handle(const std::string & url) = 0;
  virtual ResourceSharedPtr get_shared(const std::string & url) = 0;
};

}  //  namespace resource_retriever::plugins

#endif  // RESOURCE_RETRIEVER__PLUGINS__RETRIEVER_PLUGIN_HPP_
