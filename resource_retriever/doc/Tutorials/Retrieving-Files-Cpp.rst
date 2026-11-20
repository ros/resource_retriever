Retrieving files (C++)
----------------------

Overview
~~~~~~~~

This tutorial demonstrates how to retrieve a file from a URL into memory using the ``resource_retriever::Retriever`` class in C++.

Prerequisites
~~~~~~~~~~~~~

This tutorial assumes you have a working knowledge of ROS 2.

If you have not done so already `create a workspace <https://docs.ros.org/en/rolling/Tutorials/Beginner-Client-Libraries/Creating-A-Workspace/Creating-A-Workspace.html>`_ and `create a package <https://docs.ros.org/en/rolling/Tutorials/Beginner-Client-Libraries/Creating-Your-First-ROS2-Package.html>`_.

1. Write the code
~~~~~~~~~~~~~~~~~

Let's assume you have already created an empty ROS 2 package for C++ named ``retriever_tutorial``.
Create a new C++ file inside your package, e.g., ``src/retrieve_file.cpp``, with the following content:

.. code-block:: C++

  #include <cstdlib>
  #include <fstream>
  #include <iostream>
  #include <string>

  #include "resource_retriever/retriever.hpp"

  int main(int argc, char ** argv)
  {
    std::string url = "package://resource_retriever/package.xml";
    if (argc > 1) {
      url = argv[1];
    }

    resource_retriever::Retriever retriever;
    resource_retriever::ResourceSharedPtr resource;

    try {
      resource = retriever.get_shared(url);
    } catch (const resource_retriever::Exception & e) {
      std::cerr << "Failed to retrieve file: " << e.what() << std::endl;
      return EXIT_FAILURE;
    }

    if (resource == nullptr) {
      std::cerr << "No data could be retrieved from " << url << std::endl;
      return EXIT_FAILURE;
    }

    std::ofstream out("out.txt", std::ios::binary);
    out.write(
      reinterpret_cast<const char *>(resource->data.data()),
      static_cast<std::streamsize>(resource->data.size()));

    std::cout << "Wrote " << resource->data.size() << " bytes from " <<
      resource->expanded_url << " to out.txt" << std::endl;
    return EXIT_SUCCESS;
  }

2. Examine the code
~~~~~~~~~~~~~~~~~~~

.. code-block:: C++

  #include "resource_retriever/retriever.hpp"

This is the main header of ``resource_retriever``.
It declares the ``resource_retriever::Retriever`` class and it includes the headers that declare ``resource_retriever::Resource`` and ``resource_retriever::Exception``.

.. code-block:: C++

    std::string url = "package://resource_retriever/package.xml";
    if (argc > 1) {
      url = argv[1];
    }

The URL to retrieve.
Note the ``package://`` syntax, which is ROS specific: ``package://<package_name>/<path>`` refers to a file relative to the share directory of a package.
Here it refers to the ``package.xml`` file that every package installs.
Regular URLs like ``file:///etc/hostname`` or ``https://www.ros.org/`` work too, and one can be given as the first argument of the program.

.. code-block:: C++

    resource_retriever::Retriever retriever;
    resource_retriever::ResourceSharedPtr resource;

Create the ``Retriever`` object, which is the object used to retrieve files.
A default constructed ``Retriever`` handles ``package://``, ``file://``, ``http://`` and ``https://`` URLs.
Also declare a ``ResourceSharedPtr``, which is an alias of ``std::shared_ptr<resource_retriever::Resource>`` and the type returned by ``Retriever::get_shared()``.

.. code-block:: C++

    try {
      resource = retriever.get_shared(url);
    } catch (const resource_retriever::Exception & e) {
      std::cerr << "Failed to retrieve file: " << e.what() << std::endl;
      return EXIT_FAILURE;
    }

Attempt to retrieve the file.
If this fails, for example because the file or the package does not exist, a ``resource_retriever::Exception`` is thrown.

.. code-block:: C++

    if (resource == nullptr) {
      std::cerr << "No data could be retrieved from " << url << std::endl;
      return EXIT_FAILURE;
    }

``get_shared()`` can also return a null pointer.
This happens when the scheme of the URL is not supported (for example ``ftp://``), or when there was no data to retrieve.

.. code-block:: C++

    std::ofstream out("out.txt", std::ios::binary);
    out.write(
      reinterpret_cast<const char *>(resource->data.data()),
      static_cast<std::streamsize>(resource->data.size()));

Write the retrieved data to disk.
A ``Resource`` has three constant members:

* ``url``: the URL that was requested.
* ``expanded_url``: the location the data was actually read from, in this case the path of the ``package.xml`` file on disk.
* ``data``: a ``std::vector<uint8_t>`` with the content of the file.

The data stays valid for as long as a copy of the ``ResourceSharedPtr`` is alive.
It does not depend on the lifetime of the ``Retriever``.

Note that the result of writing the file is not checked, to simplify the code.

3. Update package.xml
~~~~~~~~~~~~~~~~~~~~~

Add the dependency to your ``package.xml``:

.. code-block:: xml

  <depend>resource_retriever</depend>

4. Add the node to CMakeLists.txt
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Now open your ``CMakeLists.txt`` and add the executable:

.. code-block:: cmake

  find_package(ament_cmake REQUIRED)
  find_package(resource_retriever REQUIRED)

  add_executable(retrieve_file src/retrieve_file.cpp)
  target_compile_features(retrieve_file PUBLIC cxx_std_17)
  target_link_libraries(retrieve_file PRIVATE resource_retriever::resource_retriever)

  install(TARGETS retrieve_file
    DESTINATION lib/${PROJECT_NAME})

  ament_package()

5. Build and run
~~~~~~~~~~~~~~~~

Build your package from the root of your workspace and source it:

.. code-block:: console

    $ colcon build --packages-select retriever_tutorial
    $ source install/setup.bash

Then run it:

.. code-block:: console

    $ ros2 run retriever_tutorial retrieve_file
    Wrote 1719 bytes from /opt/ros/rolling/share/resource_retriever/package.xml to out.txt

The path and the size depend on your installation.
You can also give another URL:

.. code-block:: console

    $ ros2 run retriever_tutorial retrieve_file https://www.ros.org/
    $ ros2 run retriever_tutorial retrieve_file package://resource_retriever/does_not_exist
    Failed to retrieve file: Error retrieving file [/opt/ros/rolling/share/resource_retriever/does_not_exist]: Failed to open file
