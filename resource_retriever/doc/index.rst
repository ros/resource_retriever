Resource Retriever
==================

``resource_retriever`` loads the content of a URL into memory.
On top of regular URLs such as ``file://`` and ``http://``, it understands the ROS specific ``package://`` scheme, which refers to a file relative to the share directory of a ROS 2 package.

It was initially designed to load mesh files referenced from URDF files, but it can be used for any type of data.

The package provides:

* A C++ library built around :cpp:class:`resource_retriever::Retriever`, which delegates the work to a list of plugins.
* A small Python module, :mod:`resource_retriever`, with an equivalent :func:`resource_retriever.get` function.

.. toctree::
   :maxdepth: 2

   self
   tutorials
   references
   Package information </standards>

1. Supported URLs
-----------------

.. list-table::
   :header-rows: 1
   :widths: 20 40 20 20

   * - Scheme
     - Example
     - C++
     - Python
   * - ``package://``
     - ``package://my_package/meshes/base.stl``
     - Yes
     - Yes
   * - ``file://``
     - ``file:///tmp/base.stl``
     - Yes
     - Yes
   * - ``http://``, ``https://``
     - ``https://example.com/base.stl``
     - Yes
     - Yes

A ``package://<package_name>/<path>`` URL is resolved through the ament index: it is expanded to ``file://<share directory of package_name>/<path>``.
The file therefore has to be *installed* in the share directory of the package (for example with ``install(DIRECTORY meshes DESTINATION share/${PROJECT_NAME})``), and the workspace has to be sourced.

.. note::

   In C++ the set of supported schemes is defined by the plugins given to the ``Retriever``, see `3. Retriever plugins (C++)`_.
   The default plugins do not handle other schemes that libcurl supports, such as ``ftp://``.

2. C++ API
----------

To use the library, add the dependency to your ``package.xml``:

.. code-block:: xml

   <depend>resource_retriever</depend>

and link against the exported target in your ``CMakeLists.txt``:

.. code-block:: cmake

   find_package(resource_retriever REQUIRED)

   add_executable(my_node src/my_node.cpp)
   target_link_libraries(my_node PRIVATE resource_retriever::resource_retriever)

The API consists of three types, all of them available by including ``resource_retriever/retriever.hpp``:

* :cpp:class:`resource_retriever::Retriever` is the entry point. Its ``get_shared()`` method retrieves a URL.
* :cpp:struct:`resource_retriever::Resource` is the result. It is returned as a ``resource_retriever::ResourceSharedPtr`` (a ``std::shared_ptr<Resource>``) and it has three constant members:

  * ``url``: the URL that was requested.
  * ``expanded_url``: the location the data was actually read from. For example, the path of the file on disk for a ``package://`` URL.
  * ``data``: the content, as a ``std::vector<uint8_t>``.

* :cpp:class:`resource_retriever::Exception` is thrown when the resource cannot be retrieved. It derives from ``std::runtime_error``.

.. code-block:: C++

   #include <iostream>

   #include "resource_retriever/retriever.hpp"

   int main()
   {
     resource_retriever::Retriever retriever;
     try {
       auto resource = retriever.get_shared("package://resource_retriever/package.xml");
       if (resource != nullptr) {
         std::cout << "Read " << resource->data.size() << " bytes from " <<
           resource->expanded_url << std::endl;
       }
     } catch (const resource_retriever::Exception & e) {
       std::cerr << e.what() << std::endl;
       return 1;
     }
     return 0;
   }

2.1 Error handling
~~~~~~~~~~~~~~~~~~

``Retriever::get_shared()`` reports problems in two different ways, so both of them have to be checked:

* It throws a ``resource_retriever::Exception`` when a plugin fails to retrieve the resource. For example, when the file does not exist, when a ``package://`` URL is malformed or refers to a package that is not installed, or when libcurl reports an error (host not found, connection refused, ...).
* It returns ``nullptr`` when no resource is available: none of the plugins can handle the scheme of the URL (for example ``ftp://``), or the plugins that can handle it did not return any data (for example, an HTTP response with an empty body).

.. note::

   An HTTP error status is not treated as a failure.
   If a server answers with ``404 Not Found``, ``get_shared()`` returns a ``Resource`` that contains the body of the error page.

3. Retriever plugins (C++)
--------------------------

A ``Retriever`` does not retrieve anything by itself.
It owns a list of plugins, which are classes that implement the :cpp:class:`resource_retriever::plugins::RetrieverPlugin` interface:

* ``name()`` returns a human readable name of the plugin.
* ``can_handle(url)`` returns whether the plugin is able to retrieve the URL.
* ``get_shared(url)`` retrieves the URL and returns the ``Resource``, or ``nullptr``.

``Retriever::get_shared()`` asks every plugin, in order, whether it ``can_handle()`` the URL, and returns the first non-null result.

The default constructor uses the plugins returned by ``resource_retriever::default_plugins()``, in this order:

.. list-table::
   :header-rows: 1
   :widths: 40 20 40

   * - Plugin
     - Schemes
     - Description
   * - :cpp:class:`resource_retriever::plugins::FilesystemRetriever`
     - ``package://``, ``file://``
     - Reads the file directly from the local filesystem.
   * - :cpp:class:`resource_retriever::plugins::CurlRetriever`
     - ``package://``, ``file://``, ``http://``, ``https://``
     - Retrieves the URL with `libcurl <https://curl.se/libcurl/>`_. It keeps its CURL handle so that connections can be reused between calls.

A different list of plugins can be passed to the constructor of the ``Retriever``, which makes it possible to add support for other schemes, to change the priority of the plugins or to restrict what can be retrieved:

.. code-block:: C++

   // Only allow local files, never access the network
   resource_retriever::Retriever retriever({
     std::make_shared<resource_retriever::plugins::FilesystemRetriever>(),
   });

See the :doc:`Tutorials/Writing-A-Retriever-Plugin-Cpp` tutorial to implement your own plugin.

The ``resource_retriever/plugins/retriever_plugin.hpp`` header also provides helper functions for plugin authors: ``expand_package_url()`` to turn a ``package://`` URL into a ``file://`` URL, and ``url_encode()``, ``url_decode()`` and ``encode_uri()`` to deal with percent-encoding.

4. Python API
-------------

The Python module is independent from the C++ library.
It is implemented with :mod:`urllib` and it does not use the plugins.

* :func:`resource_retriever.get` retrieves a URL and returns its content as ``bytes``.
* :func:`resource_retriever.get_filename` converts a ``package://`` URL into a ``file://`` URL (or into a plain path with ``use_protocol=False``). Other URLs are returned unchanged.

.. code-block:: python

   import resource_retriever

   data = resource_retriever.get('package://resource_retriever/package.xml')
   print(f'Read {len(data)} bytes')

   # file:///.../share/resource_retriever/package.xml
   print(resource_retriever.get_filename('package://resource_retriever/package.xml'))

Both functions raise ``ValueError`` if a ``package://`` URL is malformed, and ``ament_index_python.PackageNotFoundError`` if the package cannot be found.
``get()`` also raises ``ValueError`` if the URL cannot be retrieved, including when the server answers with an HTTP error status.
