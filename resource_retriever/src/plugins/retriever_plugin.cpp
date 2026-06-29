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

#include <curl/curl.h>

#include <cstddef>
#include <filesystem>
#include <format>  // NOLINT(build/include_order) cpplint <C++20 misclassifies as C header
#include <string>
#include <string_view>

#include "resource_retriever/exception.hpp"

#include "ament_index_cpp/get_package_prefix.hpp"
#include "ament_index_cpp/get_package_share_path.hpp"

namespace resource_retriever::plugins
{

std::string url_encode(const std::string & decoded)
{
  char * encoded = curl_easy_escape(nullptr, decoded.c_str(), static_cast<int>(decoded.length()));
  if (encoded == nullptr) {
    return decoded;
  }
  std::string result(encoded);
  curl_free(encoded);
  return result;
}

std::string url_decode(const std::string & encoded)
{
  int output_length = 0;
  char * decoded = curl_easy_unescape(
    nullptr, encoded.c_str(), static_cast<int>(encoded.length()), &output_length);
  if (decoded == nullptr) {
    return encoded;
  }
  std::string result(decoded, static_cast<size_t>(output_length));
  curl_free(decoded);
  return result;
}

std::string encode_uri(const std::string & url)
{
  // curl_easy_escape() percent-encodes every reserved character, including the
  // "scheme://" delimiters and the '/' path separators, so it cannot be applied
  // to a whole URL. Instead keep the scheme and authority verbatim and encode
  // each path segment individually, preserving the '/' separators.
  constexpr std::string_view scheme_separator = "://";
  const size_t scheme_pos = url.find(scheme_separator);
  if (scheme_pos == std::string::npos) {
    return url;
  }

  const size_t authority_pos = scheme_pos + scheme_separator.length();
  const size_t path_pos = url.find('/', authority_pos);
  if (path_pos == std::string::npos) {
    return url;
  }

  // Keep "scheme://authority" untouched, then percent-encode each '/'-delimited
  // path segment.
  std::string result = url.substr(0, path_pos);
  for (size_t segment_start = path_pos; segment_start < url.length(); ) {
    size_t segment_end = url.find('/', segment_start + 1);
    if (segment_end == std::string::npos) {
      segment_end = url.length();
    }
    result += '/';
    result += url_encode(url.substr(segment_start + 1, segment_end - segment_start - 1));
    segment_start = segment_end;
  }
  return result;
}

std::string expand_package_url(const std::string & url)
{
  constexpr std::string_view package_url_prefix = "package://";
  std::string mod_url = url;
  if (url.starts_with(package_url_prefix)) {
    mod_url.erase(0, package_url_prefix.length());
    size_t pos = mod_url.find('/');
    if (pos == std::string::npos) {
      throw Exception(
        url,
        std::format("Could not parse {} format into file:// format", package_url_prefix));
    }

    std::string package = mod_url.substr(0, pos);
    if (package.empty()) {
      throw Exception(url, "Package name must not be empty");
    }
    mod_url.erase(0, pos);
    std::filesystem::path package_path;
    try {
      package_path = ament_index_cpp::get_package_share_path(package);
    } catch (const ament_index_cpp::PackageNotFoundError &) {
      throw Exception(url, std::format("Package [{}] does not exist", package));
    }

    mod_url = "file://" + package_path.string() + mod_url;
  }
  return mod_url;
}

RetrieverPlugin::RetrieverPlugin() = default;

RetrieverPlugin::~RetrieverPlugin() = default;

}  // namespace resource_retriever::plugins
