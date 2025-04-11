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

#ifndef RESOURCE_RETRIEVER__MEMORY_RESOURCE_HPP_
#define RESOURCE_RETRIEVER__MEMORY_RESOURCE_HPP_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "resource_retriever/visibility_control.hpp"

namespace resource_retriever
{
/**
 * \brief A combination of a pointer to data in memory along with the data's size.
 */
struct
// Not using [[deprecated("use resource_retriever::Resource")]] here becuase of:
//   https://github.com/ros/resource_retriever/pull/103#issuecomment-2718515266
#if defined(_MSC_VER)
__declspec(deprecated("use resource_retriever::Resource"))
#elif defined(__GNUC__) || defined(__clang__)
__attribute__((deprecated("use resource_retriever::Resource")))
#else
  // do nothing, no deprecated attribute
#endif
MemoryResource
{
  std::shared_ptr<uint8_t> data;
  size_t size {0};
};

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

using MemoryResourceSharedPtr [[deprecated("use resource_retriever::ResourceSharedPtr")]] =
  std::shared_ptr<MemoryResource>;

#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

}  //  namespace resource_retriever

#endif  // RESOURCE_RETRIEVER__MEMORY_RESOURCE_HPP_
