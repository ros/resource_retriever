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

#include "resource_retriever/plugins/filesystem_retriever.hpp"

#include <fstream>
#include <ios>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "resource_retriever/exception.hpp"

namespace resource_retriever::plugins
{

FilesystemRetriever::FilesystemRetriever() = default;

FilesystemRetriever::~FilesystemRetriever() = default;

std::string FilesystemRetriever::name()
{
  return "resource_retriever::plugins::FilesystemRetriever";
}

bool FilesystemRetriever::can_handle(const std::string & url)
{
  return url.starts_with("package://") || url.starts_with("file://");
}

ResourceSharedPtr FilesystemRetriever::get_shared(const std::string & url)
{
  // Expand package:// url into file://
  std::string mod_url = expand_package_url(url);

  constexpr std::string_view file_scheme = "file://";
  if (mod_url.starts_with(file_scheme)) {
    mod_url.erase(0, file_scheme.length());
  }

  std::ifstream file(mod_url, std::ios::binary);
  if (!file.is_open()) {
    throw Exception(mod_url, "Failed to open file");
  }

  std::vector<uint8_t> data(std::istreambuf_iterator<char>(file), {});
  return std::make_shared<Resource>(url, std::move(mod_url), std::move(data));
}

}  // namespace resource_retriever::plugins
