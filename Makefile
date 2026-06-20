.PHONY: help clean clean_all build rebuild release lib_static lib_shared \
        lib_header_only run brun test_ctest test_direct test_dir vcpkg_update

.DEFAULT_GOAL := help

# Colors
COLOR_RESET  := \033[0m
COLOR_GREEN  := \033[1;32m
COLOR_YELLOW := \033[1;33m
COLOR_RED    := \033[1;31m

# ==========================================================
# 📦 Configuration
# ==========================================================
PROJECT_NAME := ChattingApp
CMAKE_OPT_PREFIX := $(PROJECT_NAME)
BROWSER := python3 -c "$$BROWSER_PYSCRIPT"
INSTALL_LOCATION := ~/.local
# Select Compiler gcc/clang
COMPILER ?= clang

ifeq ($(COMPILER),clang)
  CC := clang
  CXX := clang++
else ifeq ($(COMPILER),gcc)
  CC := gcc
  CXX := g++
else
  $(error Unsupported COMPILER="$(COMPILER)". Use "gcc" or "clang")
endif
CFLAGS := -Wno-error=discarded-qualifiers -Wno-error=override-init -Wno-error=deprecated-declarations

# 🔧 Helpers:
# Sync compile_commands.json to .zed/ for clangd
# Usage: $(call SYNC_COMPILE_COMMANDS)
define SYNC_COMPILE_COMMANDS
	@mkdir -p .zed
	@if [ -f build/compile_commands.json ]; then \
		cp -f build/compile_commands.json .zed/compile_commands.json; \
		printf "$(COLOR_GREEN)✓ Synced compile_commands.json → .zed/$(COLOR_RESET)\n"; \
	else \
		printf "$(COLOR_YELLOW)⚠ build/compile_commands.json not found yet$(COLOR_RESET)\n"; \
	fi
endef

define BROWSER_PYSCRIPT
import os, webbrowser, sys
try:
    from urllib import pathname2url
except:
    from urllib.request import pathname2url
webbrowser.open("file://" + pathname2url(os.path.abspath(sys.argv[1])))
endef
export BROWSER_PYSCRIPT

define PRINT_HELP_PYSCRIPT
import re, sys
for line in sys.stdin:
    match = re.match(r'^([a-zA-Z0-9_.-]+):.*?## (.*)$$', line)
    if match:
        target, help = match.groups()
        print("%-25s %s" % (target, help))
endef
export PRINT_HELP_PYSCRIPT


# ==========================================================
# 🧭 General Help
# ==========================================================
help: ## default target: print this help message
	@python3 -c "$$PRINT_HELP_PYSCRIPT" < $(MAKEFILE_LIST)

# ==========================================================
# 🧹 Cleaning
# ==========================================================
clean: ## Clean build artifacts but preserve vcpkg toolchain
	@if [ -d build ]; then \
	  find build -mindepth 1 -maxdepth 1 \
	    ! -name vcpkg_installed \
	    ! -name vcpkg \
	    -exec rm -rf {} +; \
	fi

clean_all: ## Clean the entire build directory (including vcpkg)
	rm -rf build/ .zed/

# ==========================================================
# 🏗️ Build Modes
# ==========================================================
build: ## Build the project in DEBUG mode (no tests, no optimizations)
	cmake -B build \
	      -DCMAKE_C_COMPILER=$(CC) \
	      -DCMAKE_CXX_COMPILER=$(CXX) \
	      -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION) \
	      -D$(CMAKE_OPT_PREFIX)_BUILD_TYPE="executable" \
	      -D$(CMAKE_OPT_PREFIX)_ENABLE_UNIT_TESTING=OFF \
	      -DCMAKE_BUILD_TYPE=Debug
	$(call SYNC_COMPILE_COMMANDS)
	cmake --build build --config Debug

rebuild: ## Clean and rebuild the project in DEBUG mode
	$(MAKE) clean
	$(MAKE) build

release: clean_all ## Clean and rebuild for release in RELEASE mode
	cmake -B build \
	      -DCMAKE_C_COMPILER=$(CC) \
	      -DCMAKE_CXX_COMPILER=$(CXX) \
	      -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION) \
	      -D$(CMAKE_OPT_PREFIX)_BUILD_TYPE="executable" \
	      -D$(CMAKE_OPT_PREFIX)_ENABLE_UNIT_TESTING=OFF \
	      -DCMAKE_BUILD_TYPE=Release
	$(call SYNC_COMPILE_COMMANDS)
	cmake --build build --config Release

# ==========================================================
# 📚 Library Build Modes — Static & Shared & Header-Only
# ==========================================================

lib_static: ## Build as a STATIC library in DEBUG mode
	cmake -B build \
	      -DCMAKE_C_COMPILER=$(CC) \
	      -DCMAKE_CXX_COMPILER=$(CXX) \
	      -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION) \
	      -D$(CMAKE_OPT_PREFIX)_BUILD_TYPE="static" \
	      -D$(CMAKE_OPT_PREFIX)_ENABLE_UNIT_TESTING=OFF \
	      -DCMAKE_BUILD_TYPE=Debug
	$(call SYNC_COMPILE_COMMANDS)
	cmake --build build --config Debug

lib_shared: ## Build as a SHARED library in DEBUG mode
	cmake -B build \
	      -DCMAKE_C_COMPILER=$(CC) \
	      -DCMAKE_CXX_COMPILER=$(CXX) \
	      -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION) \
	      -D$(CMAKE_OPT_PREFIX)_BUILD_TYPE="shared" \
	      -D$(CMAKE_OPT_PREFIX)_ENABLE_UNIT_TESTING=OFF \
	      -DCMAKE_BUILD_TYPE=Debug
	$(call SYNC_COMPILE_COMMANDS)
	cmake --build build --config Debug

lib_header_only: ## Build as a HEADER-ONLY library
	cmake -B build \
	      -DCMAKE_C_COMPILER=$(CC) \
	      -DCMAKE_CXX_COMPILER=$(CXX) \
	      -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION) \
	      -D$(CMAKE_OPT_PREFIX)_BUILD_TYPE="header-only" \
	      -D$(CMAKE_OPT_PREFIX)_ENABLE_UNIT_TESTING=OFF \
	      -DCMAKE_BUILD_TYPE=Debug
	$(call SYNC_COMPILE_COMMANDS)
	cmake --build build --config Debug

# ==========================================================
# 🧪 Unit Testing
# ==========================================================
test_ctest: ## Configure, rebuild, and run CTest
	cmake -B build \
	      -DCMAKE_C_COMPILER=$(CC) \
	      -DCMAKE_CXX_COMPILER=$(CXX) \
	      -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION) \
	      -D$(CMAKE_OPT_PREFIX)_ENABLE_UNIT_TESTING=ON
	$(call SYNC_COMPILE_COMMANDS)
	cmake --build build --config Release
	cd build && ctest -C Release -VV

test_gtest: ## Configure, rebuild, and run all GTest executables directly
	cmake -B build \
	      -DCMAKE_C_COMPILER=$(CC) \
	      -DCMAKE_CXX_COMPILER=$(CXX) \
	      -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION) \
	      -D$(CMAKE_OPT_PREFIX)_ENABLE_UNIT_TESTING=ON
	$(call SYNC_COMPILE_COMMANDS)
	cmake --build build --config Release
	@for test in build/test/*_Test; do \
	  echo ">>> Running $$test"; \
	  $$test --gtest_color=yes || exit 1; \
	done

test_dir: ## Configure, rebuild, and run one GTest executable (use DIR=dir/name to select in test/)
	@if [ -z "$(DIR)" ]; then \
	  echo "Usage: make test_dir DIR=foo"; \
	  exit 1; \
	fi
	@executable="build/test/$(DIR)_Test"; \
	if [ -x "$$executable" ]; then \
	  echo ">>> Running $$executable"; \
	  $$executable --gtest_color=yes || exit 1; \
	else \
	  echo "Error: test executable $$executable not found."; \
	  exit 1; \
	fi

# ==========================================================
# 🚀 Run the project
# ==========================================================
run: ## Run the executable
	@printf "$(COLOR_YELLOW)▶️  Starting $(PROJECT_NAME)...$(COLOR_RESET)\n"
	@if [ -x ./build/bin/Release/$(PROJECT_NAME) ]; then \
		./build/bin/Release/$(PROJECT_NAME); \
		EXIT_CODE=$$?; \
	elif [ -x ./build/bin/Debug/$(PROJECT_NAME) ]; then \
		./build/bin/Debug/$(PROJECT_NAME); \
		EXIT_CODE=$$?; \
	else \
		printf "$(COLOR_RED)❌ No executable found in build/bin/Release or build/bin/Debug$(COLOR_RESET)\n"; \
		EXIT_CODE=1; \
	fi; \
	if [ $$EXIT_CODE -eq 0 ]; then \
		printf "$(COLOR_GREEN)✅ Finished $(PROJECT_NAME)$(COLOR_RESET)\n"; \
	else \
		printf "$(COLOR_RED)❌ Execution failed (exit $$EXIT_CODE)$(COLOR_RESET)\n"; \
	fi; \
	exit $$EXIT_CODE

brun: ## Build and run
	$(MAKE) build
	$(MAKE) run

# ==========================================================
# 📦 Vcpkg Management
# ==========================================================
vcpkg_update: ## Update the local vcpkg installation
	@if [ -d build/vcpkg ]; then \
	  echo ">>> Updating vcpkg..."; \
	  cd build/vcpkg && git pull && ./bootstrap-vcpkg.sh; \
	else \
	  echo "vcpkg directory not found. It will be auto-cloned on next configure."; \
	fi
