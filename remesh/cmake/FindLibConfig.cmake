# - Find library libConfig++ 
IF ( LIBCONFIG_ROOT )
    FIND_PATH(LIBCONFIG_INCLUDE_DIR libconfig.h++
        PATHS ${LIBCONFIG_ROOT}/include)
    FIND_LIBRARY(LIBCONFIG_LIBRARY config++
        PATHS ${LIBCONFIG_ROOT}/lib)
ELSE ( LIBCONFIG_ROOT )
    FIND_PATH(LIBCONFIG_INCLUDE_DIR libconfig.h++
        PATHS ${SYSTEM_INC_PATH}
        PATHS $ENV{INCLUDE})
    FIND_LIBRARY(LIBCONFIG_LIBRARY config++
        PATHS ${SYSTEM_LIB_PATH}
        PATHS $ENV{INCLUDE})
ENDIF ( LIBCONFIG_ROOT )

IF (LIBCONFIG_INCLUDE_DIR AND LIBCONFIG_LIBRARY)
    IF (NOT LIBCONFIG_FIND_QUIETLY)
        MESSAGE(STATUS "Found libConfig: ${LIBCONFIG_LIBRARY}")
    ENDIF (NOT LIBCONFIG_FIND_QUIETLY)
ELSE (LIBCONFIG_INCLUDE_DIR AND LIBCONFIG_LIBRARY)
    IF (LIBCONFIG_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find libConfig")
    ENDIF(LIBCONFIG_FIND_REQUIRED)
ENDIF (LIBCONFIG_INCLUDE_DIR AND LIBCONFIG_LIBRARY)

