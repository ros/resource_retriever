Writing a retriever plugin (C++)
--------------------------------

Overview
~~~~~~~~

A ``resource_retriever::Retriever`` delegates the retrieval of a URL to a list of plugins.
This tutorial demonstrates how to implement your own plugin to support a new URL scheme, and how to use it together with the default plugins.

As an example, the plugin of this tutorial handles ``memory://<name>`` URLs, which refer to resources that the application has stored in memory.

Prerequisites
~~~~~~~~~~~~~

This tutorial assumes you have completed the :doc:`Retrieving-Files-Cpp` tutorial, and it reuses its ``retriever_tutorial`` package.

1. Write the code
~~~~~~~~~~~~~~~~~

Create a new C++ file inside your package, e.g., ``src/memory_plugin.cpp``, with the following content:

.. code-block:: C++

  #include <cstdint>
  #include <cstdlib>
  #include <iostream>
  #include <map>
  #include <memory>
  #include <string>
  #include <utility>
  #include <vector>

  #include "resource_retriever/plugins/retriever_plugin.hpp"
  #include "resource_retriever/retriever.hpp"

  class MemoryRetriever : public resource_retriever::plugins::RetrieverPlugin
  {
  public:
    void add(const std::string & name, std::vector<uint8_t> data)
    {
      resources_[name] = std::move(data);
    }

    std::string name() override
    {
      return "MemoryRetriever";
    }

    bool can_handle(const std::string & url) override
    {
      return url.rfind(prefix_, 0) == 0;
    }

    resource_retriever::ResourceSharedPtr get_shared(const std::string & url) override
    {
      const auto it = resources_.find(url.substr(prefix_.size()));
      if (it == resources_.end()) {
        throw resource_retriever::Exception(url, "No such resource in memory");
      }
      return std::make_shared<resource_retriever::Resource>(url, url, it->second);
    }

  private:
    const std::string prefix_ = "memory://";
    std::map<std::string, std::vector<uint8_t>> resources_;
  };

  int main()
  {
    auto memory_plugin = std::make_shared<MemoryRetriever>();
    memory_plugin->add("greeting", {'h', 'e', 'l', 'l', 'o'});

    resource_retriever::RetrieverVec plugins = resource_retriever::default_plugins();
    plugins.insert(plugins.begin(), memory_plugin);

    resource_retriever::Retriever retriever(plugins);

    for (const std::string url : {
        "memory://greeting",
        "package://resource_retriever/package.xml",
        "memory://unknown"})
    {
      try {
        auto resource = retriever.get_shared(url);
        if (resource == nullptr) {
          std::cout << url << ": nothing retrieved" << std::endl;
        } else {
          std::cout << url << ": " << resource->data.size() << " bytes" << std::endl;
        }
      } catch (const resource_retriever::Exception & e) {
        std::cout << e.what() << std::endl;
      }
    }
    return EXIT_SUCCESS;
  }

2. Examine the code
~~~~~~~~~~~~~~~~~~~

.. code-block:: C++

  class MemoryRetriever : public resource_retriever::plugins::RetrieverPlugin

A plugin is a class that derives from ``resource_retriever::plugins::RetrieverPlugin`` and implements its three pure virtual methods.

.. code-block:: C++

    std::string name() override
    {
      return "MemoryRetriever";
    }

``name()`` returns a human readable name that identifies the plugin.

.. code-block:: C++

    bool can_handle(const std::string & url) override
    {
      return url.rfind(prefix_, 0) == 0;
    }

``can_handle()`` tells the ``Retriever`` whether this plugin is able to retrieve a URL.
It should be cheap, and it usually only checks the scheme of the URL.
This plugin handles the URLs that start with ``memory://``.

.. code-block:: C++

    resource_retriever::ResourceSharedPtr get_shared(const std::string & url) override
    {
      const auto it = resources_.find(url.substr(prefix_.size()));
      if (it == resources_.end()) {
        throw resource_retriever::Exception(url, "No such resource in memory");
      }
      return std::make_shared<resource_retriever::Resource>(url, url, it->second);
    }

``get_shared()`` does the actual work.
It is only called with URLs for which ``can_handle()`` returned ``true``, and it has three possible outcomes:

* Return a ``Resource``, which is built from the requested URL, the expanded URL (the location the data was actually read from, which is the same URL for this plugin) and the data.
* Throw a ``resource_retriever::Exception`` to report an error. The ``Retriever`` does not catch it, so the remaining plugins are not tried.
* Return ``nullptr`` to let the ``Retriever`` try the next plugin that can handle the URL.

.. note::

   A plugin that handles ``package://`` URLs can use ``resource_retriever::plugins::expand_package_url()`` to convert them into ``file://`` URLs.

.. code-block:: C++

    resource_retriever::RetrieverVec plugins = resource_retriever::default_plugins();
    plugins.insert(plugins.begin(), memory_plugin);

    resource_retriever::Retriever retriever(plugins);

The plugins are given to the constructor of the ``Retriever`` as a ``RetrieverVec``, which is a ``std::vector`` of shared pointers to plugins.
Here the new plugin is added in front of the default ones, so the ``Retriever`` still supports ``package://``, ``file://``, ``http://`` and ``https://`` URLs.

The order matters: the plugins are tried from first to last.
A ``Retriever`` that should only handle ``memory://`` URLs could be created from a vector that only contains ``memory_plugin``.

3. Add the node to CMakeLists.txt
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Add the new executable to your ``CMakeLists.txt``:

.. code-block:: cmake

  add_executable(memory_plugin src/memory_plugin.cpp)
  target_compile_features(memory_plugin PUBLIC cxx_std_17)
  target_link_libraries(memory_plugin PRIVATE resource_retriever::resource_retriever)

  install(TARGETS memory_plugin
    DESTINATION lib/${PROJECT_NAME})

4. Build and run
~~~~~~~~~~~~~~~~

Build your package from the root of your workspace, source the workspace and run the executable:

.. code-block:: console

    $ colcon build --packages-select retriever_tutorial
    $ source install/setup.bash
    $ ros2 run retriever_tutorial memory_plugin
    memory://greeting: 5 bytes
    package://resource_retriever/package.xml: 1719 bytes
    Error retrieving file [memory://unknown]: No such resource in memory
