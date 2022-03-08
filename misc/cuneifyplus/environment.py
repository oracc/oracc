""" Environment-specific commands """

import os
import socket

from cuneify_interface import FileCuneiformCache


if not "oracc" in socket.gethostname().lower():
    raise RuntimeError(
        "Unrecognised environment: {}".format(socket.gethostname().lower())
    )


MY_URL = "http://oracc.ub.uni-muenchen.de/cuneifyplus"


def get_font_directory(environ):
    return os.path.join(environ["DOCUMENT_ROOT"], "cuneifyplus", "fonts")


def get_cache(environ):
    """ Return the standard cuneiform cache """
    cache_file_path = os.path.normpath(
        os.path.join(environ["DOCUMENT_ROOT"], "cuneifyplus", "cuneiform_cache.pickle")
    )
    return FileCuneiformCache(cache_file_path=cache_file_path, read_only=True)
