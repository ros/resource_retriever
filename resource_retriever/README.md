# resource_retriever

`resource_retriever` loads the content of a URL into memory.
It supports `file://`, `http://` and `https://` URLs, as well as the ROS specific `package://` URLs, which refer to a file relative to the share directory of a ROS 2 package.

It was initially designed to load mesh files referenced from URDF files, but it can be used for any type of data.

## C++

```cpp
#include <iostream>

#include "resource_retriever/retriever.hpp"

int main()
{
  resource_retriever::Retriever retriever;
  try {
    // Returns nullptr if the URL is not supported or there is no data
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
```

```cmake
find_package(resource_retriever REQUIRED)
target_link_libraries(my_target PRIVATE resource_retriever::resource_retriever)
```

Support for other types of URLs can be added by implementing the `resource_retriever::plugins::RetrieverPlugin` interface.

## Python

```python
import resource_retriever

data = resource_retriever.get('package://resource_retriever/package.xml')
print(f'Read {len(data)} bytes')
```

## Documentation

The full documentation, including tutorials and the API reference, is in the [doc](https://github.com/ros/resource_retriever/blob/rolling/resource_retriever/doc/index.rst) directory and it is published at [docs.ros.org](https://docs.ros.org/en/rolling/p/resource_retriever/).

To build it locally, [install rosdoc2](https://github.com/ros-infrastructure/rosdoc2#installation) and run:

```console
rosdoc2 build --package-path .
```

The result is written to `docs_output/resource_retriever/index.html`.
