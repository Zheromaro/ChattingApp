# ==========================================================
# 📦 External Libraries (Auto-Discovery)
# ==========================================================

set(EXTERNAL_LIBS_DIR "${PROJECT_SOURCE_DIR}/libs")

if(NOT IS_DIRECTORY "${EXTERNAL_LIBS_DIR}")
  verbose_message("External libs directory not found: ${EXTERNAL_LIBS_DIR}")
else()
  verbose_message("Scanning for external libraries in: ${EXTERNAL_LIBS_DIR}")
endif()

# Assumes layout:
#   libs/<library_name>/include/
#   libs/<library_name>/lib/

# ---- Auto-collect static/shared libraries ----
file(GLOB_RECURSE EXTERNAL_LIBS
    CONFIGURE_DEPENDS
    "${EXTERNAL_LIBS_DIR}/*.a"      # Linux/macOS static libraries
    "${EXTERNAL_LIBS_DIR}/*.so"     # Linux/macOS shared libraries
    "${EXTERNAL_LIBS_DIR}/*.dylib"  # macOS       shared libraries
    "${EXTERNAL_LIBS_DIR}/*.lib"    # Windows     static libraries
    "${EXTERNAL_LIBS_DIR}/*.dll"    # Windows     shared libraries
)

if(NOT EXTERNAL_LIBS)
  verbose_message("No external libraries (.a/.so/.dylib) found in ${EXTERNAL_LIBS_DIR}")
else()
  list(SORT EXTERNAL_LIBS)
  list(REMOVE_DUPLICATES EXTERNAL_LIBS)

  # for verbose output
  list(LENGTH EXTERNAL_LIBS EXTERNAL_LIBS_COUNT)
  verbose_message("Found ${EXTERNAL_LIBS_COUNT} external libraries:")
  foreach(lib IN LISTS EXTERNAL_LIBS)
    verbose_message("  - ${lib}")
  endforeach()
endif()

# ---- Auto-collect include directories ----
file(GLOB EXTERNAL_LIBS_ENTRIES
    LIST_DIRECTORIES TRUE
    "${EXTERNAL_LIBS_DIR}/*"
)

set(EXTERNAL_LIBS_INCLUDE "")

foreach(entry_path IN LISTS EXTERNAL_LIBS_ENTRIES)
  if(IS_DIRECTORY "${entry_path}/include")
    list(APPEND EXTERNAL_LIBS_INCLUDE "${entry_path}/include")
    verbose_message("Found external include dir: ${entry_path}/include")
  endif()
endforeach()

if(EXTERNAL_LIBS_INCLUDE)
  list(REMOVE_DUPLICATES EXTERNAL_LIBS_INCLUDE)
  list(SORT EXTERNAL_LIBS_INCLUDE)

  # for verbose output
  list(LENGTH EXTERNAL_LIBS_INCLUDE EXTERNAL_LIBS_INCLUDE_COUNT)
  verbose_message("Total external include directories: ${EXTERNAL_LIBS_INCLUDE_COUNT}")
else()
  verbose_message("No external include directories found in ${EXTERNAL_LIBS_DIR}")
endif()
