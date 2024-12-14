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

#include "resource_retriever/plugins/retriever_plugin.hpp"

#include <string>
#include <string_view>

#include "resource_retriever/exception.hpp"

#include "ament_index_cpp/get_package_prefix.hpp"
#include "ament_index_cpp/get_package_share_directory.hpp"

namespace resource_retriever::plugins
{

std::string escape_spaces(const std::string & url)
{
  std::string new_mod_url;
  new_mod_url.reserve(url.length());

  std::string::size_type last_pos = 0;
  std::string::size_type find_pos;

  while (std::string::npos != (find_pos = url.find(' ', last_pos))) {
    new_mod_url.append(url, last_pos, find_pos - last_pos);
    new_mod_url += "%20";
    last_pos = find_pos + std::string(" ").length();
  }

  // Take care for the rest after last occurrence
  new_mod_url.append(url, last_pos, url.length() - last_pos);
  return new_mod_url;
}

std::string expand_package_url(const std::string & url)
{
  constexpr std::string_view package_url_prefix = "package://";
  std::string mod_url = url;
  if (url.find(package_url_prefix) == 0) {
    mod_url.erase(0, package_url_prefix.length());
    size_t pos = mod_url.find('/');
    if (pos == std::string::npos) {
      throw Exception(
        url,
        "Could not parse " + std::string(package_url_prefix) + " format into file:// format");
    }

    std::string package = mod_url.substr(0, pos);
    if (package.empty()) {
      throw Exception(url, "Package name must not be empty");
    }
    mod_url.erase(0, pos);
    std::string package_path;
    try {
      package_path = ament_index_cpp::get_package_share_directory(package);
    } catch (const ament_index_cpp::PackageNotFoundError &) {
      throw Exception(url, "Package [" + package + "] does not exist");
    }

    mod_url = "file://" + package_path + mod_url;
  }
  return mod_url;
}

RetrieverPlugin::RetrieverPlugin() = default;

RetrieverPlugin::~RetrieverPlugin() = default;

}  // namespace resource_retriever::plugins
