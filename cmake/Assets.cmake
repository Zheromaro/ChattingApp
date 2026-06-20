# ==========================================================
# 🎨 Asset Copying
# ==========================================================
set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")

if(EXISTS "${ASSETS_SOURCE_DIR}")
  set(ASSETS_DEST_DIR "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")

  # Copy assets at build time
  add_custom_command(
      TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${ASSETS_SOURCE_DIR}"
          "${ASSETS_DEST_DIR}"
      COMMENT "📂 Copying assets to output directory"
      VERBATIM
  )
endif()
