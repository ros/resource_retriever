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

#ifndef RESOURCE_RETRIEVER__RETRIEVER_HPP_
#define RESOURCE_RETRIEVER__RETRIEVER_HPP_

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "resource_retriever/exception.hpp"
#include "resource_retriever/plugins/retriever_plugin.hpp"
#include "resource_retriever/resource.hpp"
#include "resource_retriever/visibility_control.hpp"

namespace resource_retriever
{

/// Shared pointer to a retriever plugin
using RetrieverPluginSharedPtr = std::shared_ptr<plugins::RetrieverPlugin>;
/// List of retriever plugins, ordered by priority
using RetrieverVec = std::vector<RetrieverPluginSharedPtr>;

/**
 * \brief Create the plugins that a Retriever uses by default
 * \return A plugins::FilesystemRetriever (package:// and file:// urls) followed by a
 * plugins::CurlRetriever (package://, file://, http:// and https:// urls)
 */
RetrieverVec RESOURCE_RETRIEVER_PUBLIC default_plugins();

/**
 * \brief Retrieves files from a url, delegating the work to a list of plugins.
 *
 * With the default plugins, package://, file://, http:// and https:// urls are supported, and a
 * CURL handle is cached so multiple accesses to a single url will keep connections open.
 */
class Retriever
{
public:
  /**
   * \brief Construct a retriever
   * \param plugins The plugins used to retrieve the urls, which are tried in order
   */
  RESOURCE_RETRIEVER_PUBLIC
  explicit Retriever(RetrieverVec plugins = default_plugins());

  RESOURCE_RETRIEVER_PUBLIC
  ~Retriever();

  /**
   * \brief Get a file and store it in memory
   *
   * The url is given to the first plugin that can handle it. If that plugin returns nullptr, the
   * next plugin that can handle the url is tried.
   *
   * \param url The url to retrieve. package://package/file will be turned into the correct file:// invocation
   * \return The file, loaded into memory, or nullptr if none of the plugins can handle the url
   * or none of them returned any data.
   * \throws resource_retriever::Exception if a plugin fails to retrieve the url.
   */
  RESOURCE_RETRIEVER_PUBLIC
  ResourceSharedPtr get_shared(const std::string & url);

private:
  RetrieverVec plugins;
};

}  //  namespace resource_retriever

#endif  //  RESOURCE_RETRIEVER__RETRIEVER_HPP_
