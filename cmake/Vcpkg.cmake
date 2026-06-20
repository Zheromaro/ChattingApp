# ==========================================================
# 📦 Vcpkg Integration (Project-local, inside build dir)
# ==========================================================

if(DEFINED PROJECT_NAME)
  message(FATAL_ERROR "Vcpkg.cmake must be included BEFORE the first project() call.")
endif()

option(ENABLE_VCPKG "Auto-download and enable local Vcpkg toolchain." ON)

# If the user already passed a valid toolchain file on the command line, respect it.
if(ENABLE_VCPKG AND (NOT DEFINED CMAKE_TOOLCHAIN_FILE OR NOT EXISTS "${CMAKE_TOOLCHAIN_FILE}"))

  # Use CMAKE_SOURCE_DIR since CMAKE_BINARY_DIR isn't set yet before project()
  set(VCPKG_ROOT "${CMAKE_SOURCE_DIR}/build/vcpkg" CACHE PATH "Local vcpkg directory")
  set(VCPKG_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "Vcpkg toolchain file")

  if(NOT EXISTS "${VCPKG_ROOT}")
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
    message(STATUS "🌐 vcpkg not found. Cloning into ${VCPKG_ROOT}...")
    find_package(Git QUIET REQUIRED)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} clone "https://github.com/microsoft/vcpkg.git" "${VCPKG_ROOT}"
      RESULT_VARIABLE GIT_CLONE_RESULT
      OUTPUT_QUIET
    )
    if(NOT GIT_CLONE_RESULT EQUAL 0)
      message(FATAL_ERROR "❌ Failed to clone vcpkg.")
    endif()
  endif()

  if(EXISTS "${VCPKG_TOOLCHAIN_FILE}")
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_TOOLCHAIN_FILE}" CACHE FILEPATH "Vcpkg toolchain" FORCE)
    set(VCPKG_MANIFEST_MODE ON CACHE BOOL "" FORCE)
    message(STATUS "🔗 Local Vcpkg toolchain active: ${CMAKE_TOOLCHAIN_FILE}")
  else()
    message(FATAL_ERROR "❌ Vcpkg toolchain file missing: ${VCPKG_TOOLCHAIN_FILE}")
  endif()
  # Explicitly set manifest dir to ensure vcpkg.json is found
  set(VCPKG_MANIFEST_DIR "${CMAKE_SOURCE_DIR}" CACHE PATH "" FORCE)

endif()
