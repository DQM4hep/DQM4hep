#.rst:
# FindMkDocs
# -----------
#
# This module looks for MkDocs.
# MkDocs is a static site generator that's geared towards building project documentation.
# http://www.mkdocs.org
#
# This modules defines the following variables:
#    MkDocs_EXECUTABLE     = The path to the MkDocs command.
#    MkDocs_FOUND          = Was MkDocs found or not?
#    MkDocs_VERSION        = The version reported by mkdocs --version
#

find_program(MkDocs_EXECUTABLE 
  NAMES mkdocs mkdocs.exe
  HINTS ${MkDocs_DIR} /usr/local /usr
  PATH_SUFFIXES bin
  DOC "MkDocs documentation generation tool (http://www.mkdocs.org)"
)

if(MkDocs_EXECUTABLE)
  execute_process(COMMAND ${MkDocs_EXECUTABLE} "--version" OUTPUT_VARIABLE MkDocs_VERSION_OUTPUT OUTPUT_STRIP_TRAILING_WHITESPACE)
  string(REGEX MATCH "[0-9*]\.*" MkDocs_VERSION ${MkDocs_VERSION_OUTPUT})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MkDocs DEFAULT_MSG MkDocs_EXECUTABLE MkDocs_VERSION)

mark_as_advanced(MkDocs_EXECUTABLE)
