Retrieving files (Python)
-------------------------

Overview
~~~~~~~~

This tutorial demonstrates how to retrieve a file from a URL into memory using the ``resource_retriever`` Python module.

Prerequisites
~~~~~~~~~~~~~

This tutorial assumes you have a working knowledge of ROS 2.

If you have not done so already `create a workspace <https://docs.ros.org/en/rolling/Tutorials/Beginner-Client-Libraries/Creating-A-Workspace/Creating-A-Workspace.html>`_ and `create a package <https://docs.ros.org/en/rolling/Tutorials/Beginner-Client-Libraries/Creating-Your-First-ROS2-Package.html>`_.

1. Write the code
~~~~~~~~~~~~~~~~~

Let's assume you have already created an empty ROS 2 package for Python named ``retriever_tutorial``.
Create a new Python file inside your package, e.g., ``retriever_tutorial/retrieve_file.py``, with the following content:

.. code-block:: python

  import sys

  from ament_index_python import PackageNotFoundError
  import resource_retriever


  def main():
      url = 'package://resource_retriever/package.xml'
      if len(sys.argv) > 1:
          url = sys.argv[1]

      try:
          data = resource_retriever.get(url)
      except (ValueError, PackageNotFoundError) as e:
          print(f'Failed to retrieve file: {e!r}', file=sys.stderr)
          return 1

      with open('out.txt', 'wb') as f:
          f.write(data)

      print(f'Wrote {len(data)} bytes from {resource_retriever.get_filename(url)} to out.txt')
      return 0


  if __name__ == '__main__':
      sys.exit(main())

2. Examine the code
~~~~~~~~~~~~~~~~~~~

.. code-block:: python

      url = 'package://resource_retriever/package.xml'
      if len(sys.argv) > 1:
          url = sys.argv[1]

The URL to retrieve.
Note the ``package://`` syntax, which is ROS specific: ``package://<package_name>/<path>`` refers to a file relative to the share directory of a package.
Here it refers to the ``package.xml`` file that every package installs.
Regular ``file://``, ``http://`` and ``https://`` URLs work too, and one can be given as the first argument of the program.

.. code-block:: python

      try:
          data = resource_retriever.get(url)
      except (ValueError, PackageNotFoundError) as e:
          print(f'Failed to retrieve file: {e!r}', file=sys.stderr)
          return 1

``resource_retriever.get()`` retrieves the URL and returns its content as a ``bytes`` object.
It raises:

* ``ValueError`` if the URL cannot be retrieved, or if a ``package://`` URL is malformed.
* ``ament_index_python.PackageNotFoundError`` if a ``package://`` URL refers to a package that cannot be found.

.. code-block:: python

      with open('out.txt', 'wb') as f:
          f.write(data)

Write the retrieved data to disk.
The file is opened in binary mode because ``data`` is a ``bytes`` object.
Use ``data.decode()`` if you need the content of a text file as a ``str``.

.. code-block:: python

      print(f'Wrote {len(data)} bytes from {resource_retriever.get_filename(url)} to out.txt')

``resource_retriever.get_filename()`` converts a ``package://`` URL into the corresponding ``file://`` URL without retrieving it.
Other URLs are returned unchanged.
Use ``get_filename(url, use_protocol=False)`` to get a plain path instead, for example to give it to a library that loads the file by itself.

3. Update package.xml
~~~~~~~~~~~~~~~~~~~~~

Add the dependencies to your ``package.xml``:

.. code-block:: xml

  <exec_depend>ament_index_python</exec_depend>
  <exec_depend>resource_retriever</exec_depend>

4. Add the entry point to setup.py
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Update the ``console_scripts`` section of your ``setup.py``:

.. code-block:: python

    entry_points={
        'console_scripts': [
            'retrieve_file = retriever_tutorial.retrieve_file:main',
        ],
    },

5. Build and run
~~~~~~~~~~~~~~~~

Build your package from the root of your workspace and source it:

.. code-block:: console

    $ colcon build --packages-select retriever_tutorial
    $ source install/setup.bash

Then run it:

.. code-block:: console

    $ ros2 run retriever_tutorial retrieve_file
    Wrote 1719 bytes from file:///opt/ros/rolling/share/resource_retriever/package.xml to out.txt

The path and the size depend on your installation.
