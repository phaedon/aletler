# Look for the header file.
FIND_PATH(TINYXML_INCLUDE_DIR NAMES tinyxml2.h)
MARK_AS_ADVANCED(TINYXML_INCLUDE_DIR)

# Look for the library.
FIND_LIBRARY(TINYXML_LIBRARY NAMES tinyxml2)
MARK_AS_ADVANCED(TINYXML_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set TINYXML_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TINYXML DEFAULT_MSG TINYXML_LIBRARY TINYXML_INCLUDE_DIR)

IF (TINYXML_FOUND)
    SET(TINYXML_LIBRARIES ${TINYXML_LIBRARY})
    SET(TINYXML_INCLUDE_DIRS ${TINYXML_INCLUDE_DIR})
ENDIF (TINYXML_FOUND)