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

#include "resource_retriever/retriever.hpp"

#include <cstring>
#include <memory>
#include <string>
#include <utility>

#include "resource_retriever/exception.hpp"
#include "resource_retriever/plugins/curl_retriever.hpp"
#include "resource_retriever/plugins/filesystem_retriever.hpp"


namespace resource_retriever
{

RetrieverVec default_plugins()
{
  return {
    std::make_shared<plugins::FilesystemRetriever>(),
    std::make_shared<plugins::CurlRetriever>(),
  };
}

Retriever::Retriever(RetrieverVec plugins)
:plugins(std::move(plugins))
{
}

Retriever::~Retriever() = default;

ResourceSharedPtr Retriever::get_shared(const std::string & url)
{
  for (auto & plugin : plugins) {
    if (plugin->can_handle(url)) {
      auto res = plugin->get_shared(url);

      if (res != nullptr) {
        return res;
      }
    }
  }
  return nullptr;
}

}  // namespace resource_retriever
