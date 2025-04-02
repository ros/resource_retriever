^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package libcurl_vendor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

3.4.4 (2025-04-02)
------------------
* uniform  MinCMakeVersion (`#108 <https://github.com/ros/resource_retriever/issues/108>`_) (`#109 <https://github.com/ros/resource_retriever/issues/109>`_)
  Co-authored-by: Alejandro Hernandez Cordero <ahcorde@gmail.com>
  (cherry picked from commit 168300fba1c4f507d1e3bea0c4f97d468cbad477)
  Co-authored-by: mosfet80 <realeandrea@yahoo.it>
* Contributors: mergify[bot]

3.4.3 (2024-06-27)
------------------

3.4.2 (2024-05-13)
------------------
* Add "lib" to the Windows curl search path. (`#96 <https://github.com/ros/resource_retriever/issues/96>`_) (`#97 <https://github.com/ros/resource_retriever/issues/97>`_)
  In CMake 3.3, a commit made it so that the find_package
  module in CMake had a compatibility mode where it would
  automatically search for packages in a <prefix>/lib subdirectory.
  In CMake 3.6, this compatibility mode was reverted for all
  platforms *except* Windows.
  That means that since CMake 3.3, we haven't actually been
  using the path as specified in `curl_DIR`, but we have
  instead been inadvertently relying on that fallback behavior.
  In CMake 3.28, that compatibilty mode was also removed for
  Windows, meaning that we are now failing to find_package(curl)
  in downstream packages (like resource_retriever).
  Fix this by adding in the "lib" directory that always should
  have been there.  I'll note that this *only* affects our
  Windows builds, because this code is in a if(WIN32) block.
  (cherry picked from commit 1839d583190eb9dcf339eaaf6bebe632d94664a6)
  Co-authored-by: Chris Lalancette <clalancette@gmail.com>
* Contributors: mergify[bot]

3.4.1 (2024-04-16)
------------------

3.4.0 (2023-12-26)
------------------

3.3.1 (2023-07-11)
------------------
* Switch to ament_cmake_vendor_package (`#86 <https://github.com/ros/resource_retriever/issues/86>`_)
* Contributors: Scott K Logan

3.3.0 (2023-04-28)
------------------

3.2.2 (2022-11-02)
------------------
* merge libcurl_vendor build instructions (`#81 <https://github.com/ros/resource_retriever/issues/81>`_)
* Contributors: schrodinbug

3.2.1 (2022-09-13)
------------------
* Sets CMP0135 policy behavior to NEW (`#79 <https://github.com/ros/resource_retriever/issues/79>`_)
* Fixes policy CMP0135 warning for CMake >= 3.24
* Contributors: Cristóbal Arroyo, Crola1702

3.2.0 (2022-05-10)
------------------

3.1.0 (2022-03-01)
------------------
* Update to curl 7.81. (`#74 <https://github.com/ros/resource_retriever/issues/74>`_)
* Contributors: Chris Lalancette

3.0.0 (2022-01-14)
------------------
* Update maintainers (`#66 <https://github.com/ros/resource_retriever/issues/66>`_)
* Contributors: Audrow Nash

2.5.0 (2021-04-06)
------------------
* Update libcurl_vendor to the latest version (7.75.0). (`#60 <https://github.com/ros/resource_retriever/issues/60>`_)
* Contributors: Chris Lalancette

2.4.2 (2021-03-18)
------------------
* Add an override flag to force vendored build (`#58 <https://github.com/ros/resource_retriever/issues/58>`_)
* Contributors: Scott K Logan

2.4.1 (2020-12-08)
------------------
* Update maintainers (`#53 <https://github.com/ros/resource_retriever/issues/53>`_)
* Contributors: Alejandro Hernández Cordero

2.4.0 (2020-08-12)
------------------
* bump curl version to 7.68 (`#47 <https://github.com/ros/resource_retriever/issues/47>`_)
* Contributors: Dirk Thomas

2.3.2 (2020-06-03)
------------------
* export TARGETS in libcurl_vendor (`#46 <https://github.com/ros/resource_retriever/issues/46>`_)
* Contributors: Dirk Thomas

2.3.1 (2020-05-08)
------------------

2.3.0 (2020-04-30)
------------------
* Ignore broken curl-config.cmake (`#40 <https://github.com/ros/resource_retriever/issues/40>`_)
* Update curl reference from 7.57 to 7.58 (`#36 <https://github.com/ros/resource_retriever/issues/36>`_)
* Contributors: Nathan Brooks, Scott K Logan

2.2.0 (2019-09-26)
------------------
* add .dsv file beside custom environment hook (`#30 <https://github.com/ros/resource_retriever/issues/30>`_)
* Contributors: Dirk Thomas

2.1.1 (2019-05-08)
------------------

2.1.0 (2018-06-21)
------------------
* depend on curl (mapping to curl, libcurl4-openssl-dev) for packaging (`#25 <https://github.com/ros/resource_retriever/issues/25>`_)
* add missing dependency on pkg-config (`#19 <https://github.com/ros/resource_retriever/issues/19>`_)
* [libcurl_vendor] convert to ament and setup env hooks for library paths (`#14 <https://github.com/ros/resource_retriever/issues/14>`_)
* Contributors: Dirk Thomas, Mikael Arguedas, William Woodall

2.0.0 (2017-12-08)
------------------
* disable docs and tests for libcurl to avoid needing perl
* fix libcurl vendor package on Windows and warnings
* add temporary libcurl_vendor and use it to get curl
* Contributors: William Woodall

1.12.3 (2017-03-27)
-------------------

1.12.2 (2016-06-10 14:16)
-------------------------

1.12.1 (2016-06-10 10:28)
-------------------------

1.12.0 (2016-03-23)
-------------------

1.11.6 (2014-11-30)
-------------------
