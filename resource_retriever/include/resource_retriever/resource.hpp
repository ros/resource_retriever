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

#ifndef RESOURCE_RETRIEVER__RESOURCE_HPP_
#define RESOURCE_RETRIEVER__RESOURCE_HPP_

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "resource_retriever/visibility_control.hpp"

namespace resource_retriever
{
/**
 * \brief A retrieved resource, containing the url, expanded url, and binary data.
 */
struct Resource
{
  RESOURCE_RETRIEVER_PUBLIC
  explicit Resource(
    std::string url_in,
    std::string expanded_url_in,
    std::vector<uint8_t> data_in)
  : url(std::move(url_in)),
    expanded_url(std::move(expanded_url_in)),
    data(std::move(data_in))
  {
  }
  const std::string url;
  const std::string expanded_url;
  const std::vector<uint8_t> data;
};

using ResourceSharedPtr = std::shared_ptr<Resource>;

}  //  namespace resource_retriever

#endif  // RESOURCE_RETRIEVER__RESOURCE_HPP_
