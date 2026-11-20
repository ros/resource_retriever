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
/// Convert a package://package/file url into a file:// url.
/**
 * The package is looked up in the ament index, and the file is relative to its share directory.
 * Urls that do not start with package:// are returned unchanged.
 *
 * \param url The url to expand
 * \return The expanded url
 * \throws resource_retriever::Exception if the url is malformed or the package is not found
 */
RESOURCE_RETRIEVER_PUBLIC
std::string expand_package_url(const std::string & url);

/// Percent-encode a single URL component (e.g. one path segment).
RESOURCE_RETRIEVER_PUBLIC
std::string url_encode(const std::string & decoded);

/// Percent-decode a single URL component.
RESOURCE_RETRIEVER_PUBLIC
std::string url_decode(const std::string & encoded);

/// Percent-encode the path of a URL while leaving the scheme, authority and
/// '/' separators untouched, so that libcurl accepts paths containing spaces
/// or other special characters.
RESOURCE_RETRIEVER_PUBLIC
std::string encode_uri(const std::string & url);

/// Interface of the plugins that a Retriever uses to retrieve urls.
class RESOURCE_RETRIEVER_PUBLIC RetrieverPlugin
{
public:
  RetrieverPlugin();
  virtual ~RetrieverPlugin() = 0;

  /// Get a human readable name of the plugin.
  virtual std::string name() = 0;

  /// Check whether this plugin is able to retrieve a url, usually based on its scheme.
  virtual bool can_handle(const std::string & url) = 0;

  /// Retrieve a url and load it into memory.
  /**
   * \param url The url to retrieve, for which can_handle() returned true
   * \return The retrieved resource, or nullptr to let the Retriever try the next plugin
   * \throws resource_retriever::Exception if the url cannot be retrieved
   */
  virtual ResourceSharedPtr get_shared(const std::string & url) = 0;
};

}  //  namespace resource_retriever::plugins

#endif  // RESOURCE_RETRIEVER__PLUGINS__RETRIEVER_PLUGIN_HPP_
