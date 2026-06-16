# ==========================================================
# 🛠️ Utility Functions
# ==========================================================

function(verbose_message content)
  if(${PROJECT_NAME}_VERBOSE_OUTPUT)
    message(STATUS ${content})
  endif()
endfunction()

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)
