## Code API

`resource_retriever` has an extremely simple C++ API, consisting of
two classes and one method.

The
[resource_retriever::Retriever](http://www.ros.org/doc/api/resource_retriever/html/classresource__retriever_1_1Retriever.html)
class is the gateway into downloading files. Its `get()` method
returns a
[resource_retriever::MemoryResource](http://www.ros.org/doc/api/resource_retriever/html/structresource__retriever_1_1MemoryResource.html)
which contains the file in memory.

The
[Retrieving Files](#retrieving-files)
tutorial details how to use both classes.

`resource_retriever` also has a very similar Python API.

## Retrieving Files

- **Note:** This tutorial assumes that you have completed the previous
  tutorials: [ROS Tutorials](ROS/Tutorials).
- **Description:** Demonstrates how to retrieve a file from a URL using
  the resource_retriever package
- **Keywords:** urdf, resource_retriever, url, curl
- **Tutorial Level:** BEGINNER

### Intro

The resource_retriever package lets
you download files from any [CURL](http://curl.haxx.se/)-supported URL,
as well as using a special `package:*` syntax. Here we'll show you
how to use it.*

### Retrieving a File Into Memory

Here we'll show you how to retrieve a file from a URL into memory,
which is currently the only supported method of retrieval.

#### The Code



```cplusplus
#include <resource_retriever/retriever.h>
#include <ros/console.h>

int main(int argc, char** argv)
{
  resource_retriever::Retriever r;
  resource_retriever::MemoryResource resource;

  try
  {
     resource = r.get("package://resource_retriever/test/test.txt"); 
  }
  catch (resource_retriever::Exception& e)
  {
    ROS_ERROR("Failed to retrieve file: %s", e.what());
    return 1;
  }

  FILE* f = fopen("out.txt", "w");
  fwrite(resource.data.get(), resource.size, 1, f);
  fclose(f);
  
  ROS_INFO("Wrote data from package://resource_retriever/test/test.txt to out.txt");
}
```



#### The Code Explained

This is the main include file for
resource_retriever. It includes the
two main classes, `resource_retriver::Retreiver` and
`resource_retriever::MemoryResource`. Create the
`resource_retriever::Retriever` object, which is the object used to
retrieve files. Also create a `resource_retriever::MemoryResource`
object, which is the object type returned from `Retriever`'s
`get()` method. Attempt to download the file. Note the `package:*`
syntax, which is ROS-specific. Generic urls like
`http:*www.google.com/` would also work here.

If this fails for any reason, a `resource_retriever::Exception` will
be thrown.

Write out the downloaded file to disk. `MemoryResource` contains two
members:

- data \-- a `boost::shared_ptr<uint8_t>`
- size \-- The size, in bytes, of the data

`MemoryResource`s can be copied. Once all references to
`MemoryResource::data` have left scope it will automatically be
deleted.

Note that we don't do any error checking on the `fopen` or `fwrite`
here, to simplify the code.

## Tutorials

- [Retrieving Files](#retrieving-files):
  Demonstrates how to retrieve a file from a URL using the
  resource_retriever package

## Troubleshooting

### Reporting Errors

If you encounter a problem that isn't addressed on this page, please
[file a ticket](https://code.ros.org/trac/ros-pkg/newticket?component=robot_model&keywords=resource_retriever&owner=jfaust)

The current list of open issues is available on
[trac](https://code.ros.org/trac/ros-pkg/query?status=assigned&status=new&status=reopened&group=milestone&order=priority&col=id&col=summary&col=status&col=type&col=priority&col=milestone&col=component&keywords=~resource_retriever)

### Common Problems When Using Package

(There are currently no recorded commonly occurring errors)
