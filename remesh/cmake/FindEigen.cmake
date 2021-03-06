# - Find Eigen
# This module searches for the Eigen C++ library.
# 
# The module defines the following variables:
#  Eigen_INCLUDE_DIR, where to find Eigen/Core, etc.
#  Eigen_FOUND, if false, do not try to use EIGEN.
#
# Variables used by this module, they can change the default behavior and need
# to be set before calling find_package:
#
#  EIGEN_ROOT - The prefered installation prefix when searching for Eigen
#


# Finds the include files directory
find_path(Eigen_INCLUDE_DIR Eigen/Core
  HINTS "${EIGEN_ROOT}"
  PATH_SUFFIXES "eigen3"
  DOC "The directory where Eigen/Core resides"
)

mark_as_advanced(Eigen_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Eigen DEFAULT_MSG Eigen_INCLUDE_DIR)
