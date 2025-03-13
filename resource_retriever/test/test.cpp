// Copyright 2008, Willow Garage, Inc. All rights reserved.
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

#include <exception>
#include <memory>
#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "resource_retriever/memory_resource.hpp"
#include "resource_retriever/plugins/retriever_plugin.hpp"
#include "resource_retriever/retriever.hpp"

TEST(Retriever, defaultPlugins)
{
  auto plugins = resource_retriever::default_plugins();

  EXPECT_EQ(plugins.size(), 2u);
}

TEST(Retriever, getByPackage)
{
  try {
    resource_retriever::Retriever r;
    auto res = r.get_shared("package://resource_retriever/test/test.txt");

    ASSERT_NE(nullptr, res);
    ASSERT_EQ(res->data.size(), 1u);
    ASSERT_EQ(*res->data.data(), 'A');
  } catch (const std::exception & e) {
    FAIL() << "Exception caught: " << e.what() << '\n';
  }
}

TEST(Retriever, http)
{
  try {
    resource_retriever::Retriever r;
    auto res = r.get_shared("http://packages.ros.org/ros.key");

    ASSERT_NE(nullptr, res);
    ASSERT_GT(res->data.size(), 0u);
  } catch (const std::exception & e) {
    FAIL() << "Exception caught: " << e.what() << '\n';
  }
}

TEST(Retriever, invalidFiles)
{
  resource_retriever::Retriever r;

  EXPECT_THROW(r.get_shared("file://fail"), resource_retriever::Exception);
  EXPECT_THROW(r.get_shared("package://roscpp"), resource_retriever::Exception);
  EXPECT_THROW(r.get_shared("package://invalid_package_blah/test.xml"),
    resource_retriever::Exception);
  EXPECT_THROW(r.get_shared("package:///test.xml"), resource_retriever::Exception);
}

class TestRetrieverPlugin : public resource_retriever::plugins::RetrieverPlugin
{
public:
  TestRetrieverPlugin() = default;
  ~TestRetrieverPlugin() override = default;

  std::string name() override
  {
    return "TestRetrieverPlugin";
  }

  bool can_handle(const std::string & url) override
  {
    return url.find("test://") == 0;
  }

  resource_retriever::ResourceSharedPtr get_shared(const std::string & url) override
  {
    return std::make_shared<resource_retriever::Resource>(
        url, url, std::vector<uint8_t>{0, 1, 2, 3, 4, 5});
  }
};

TEST(Retriever, customPlugin)
{
  resource_retriever::RetrieverVec plugins {
    std::make_shared<TestRetrieverPlugin>()
  };

  resource_retriever::Retriever r(plugins);
  EXPECT_EQ(nullptr, r.get_shared("package://resource_retriever/test/text.txt"));
  EXPECT_EQ(nullptr, r.get_shared("file://foo/bar/text.txt"));
  EXPECT_NE(nullptr, r.get_shared("test://foo"));

  auto res = r.get_shared("test://foo");
  EXPECT_EQ(res->data.size(), 6u);
  EXPECT_EQ(res->data[0], 0u);
  EXPECT_EQ(res->data[1], 1u);
}

int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
