# Copyright 2008, Willow Garage, Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#
#    * Neither the name of the {copyright_holder} nor the names of its
#      contributors may be used to endorse or promote products derived from
#      this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

from pathlib import PurePosixPath, PureWindowsPath
import sys
from urllib.error import URLError
from urllib.request import urlopen

from ament_index_python import get_package_share_directory

PACKAGE_PREFIX = 'package://'


def get_filename(url: str, use_protocol: bool = True) -> str:
    """
    Convert a package:// URL to a file:// (or plain) path.

    Parameters
    ----------
    url:
        The URL to convert. Non-package:// URLs are returned unchanged.
    use_protocol:
        When *True* (default) the returned string is prefixed with ``file://``.

    Returns
    -------
    str
        The converted URL or the original URL if no conversion was needed.

    Raises
    ------
    ValueError
        If the ``package://`` URL is malformed (missing path separator).

    """
    if not url.startswith(PACKAGE_PREFIX):
        return url

    rest = url[len(PACKAGE_PREFIX):]
    pos = rest.find('/')
    if pos == -1:
        raise ValueError(
            f"Could not parse package:// URL into file:// URL: '{url}'. "
            'Expected format: package://<package_name>/<path>'
        )

    package = rest[:pos]
    rel_path = rest[pos:]
    package_path = get_package_share_directory(package)

    protocol = 'file://' if use_protocol else ''

    if sys.platform.startswith('win'):
        # On Windows, build a proper Windows path then re-express it with
        # forward slashes and a leading '/' so it forms a valid file URI.
        full_path = str(PureWindowsPath(package_path) / PurePosixPath(rel_path).relative_to('/'))
        mod_url = protocol + '/' + full_path.replace('\\', '/')
    else:
        mod_url = protocol + package_path + rel_path

    return mod_url


def get(url: str) -> bytes:
    """
    Retrieve the resource at *url* and return its contents as bytes.

    Parameters
    ----------
    url:
        A ``package://``, ``file://``, ``http://``, or ``https://`` URL.

    Returns
    -------
    bytes
        The raw content of the resource.

    Raises
    ------
    ValueError
        If the URL cannot be fetched.

    """
    filename = get_filename(url)
    try:
        return urlopen(filename).read()
    except URLError as e:
        raise ValueError(f'Could not retrieve resource at URL: {filename}') from e
