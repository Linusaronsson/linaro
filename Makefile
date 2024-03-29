# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.11

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/linaro/github_projects/linaro

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/linaro/github_projects/linaro

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/usr/bin/ccmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/linaro/github_projects/linaro/CMakeFiles /home/linaro/github_projects/linaro/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/linaro/github_projects/linaro/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named linaro

# Build rule for target.
linaro: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 linaro
.PHONY : linaro

# fast build rule for target.
linaro/fast:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/build
.PHONY : linaro/fast

src/ast/ast.o: src/ast/ast.cpp.o

.PHONY : src/ast/ast.o

# target to build an object file
src/ast/ast.cpp.o:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/ast/ast.cpp.o
.PHONY : src/ast/ast.cpp.o

src/ast/ast.i: src/ast/ast.cpp.i

.PHONY : src/ast/ast.i

# target to preprocess a source file
src/ast/ast.cpp.i:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/ast/ast.cpp.i
.PHONY : src/ast/ast.cpp.i

src/ast/ast.s: src/ast/ast.cpp.s

.PHONY : src/ast/ast.s

# target to generate assembly for a file
src/ast/ast.cpp.s:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/ast/ast.cpp.s
.PHONY : src/ast/ast.cpp.s

src/code_generator/chunk.o: src/code_generator/chunk.cpp.o

.PHONY : src/code_generator/chunk.o

# target to build an object file
src/code_generator/chunk.cpp.o:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/code_generator/chunk.cpp.o
.PHONY : src/code_generator/chunk.cpp.o

src/code_generator/chunk.i: src/code_generator/chunk.cpp.i

.PHONY : src/code_generator/chunk.i

# target to preprocess a source file
src/code_generator/chunk.cpp.i:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/code_generator/chunk.cpp.i
.PHONY : src/code_generator/chunk.cpp.i

src/code_generator/chunk.s: src/code_generator/chunk.cpp.s

.PHONY : src/code_generator/chunk.s

# target to generate assembly for a file
src/code_generator/chunk.cpp.s:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/code_generator/chunk.cpp.s
.PHONY : src/code_generator/chunk.cpp.s

src/code_generator/code_generator.o: src/code_generator/code_generator.cpp.o

.PHONY : src/code_generator/code_generator.o

# target to build an object file
src/code_generator/code_generator.cpp.o:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/code_generator/code_generator.cpp.o
.PHONY : src/code_generator/code_generator.cpp.o

src/code_generator/code_generator.i: src/code_generator/code_generator.cpp.i

.PHONY : src/code_generator/code_generator.i

# target to preprocess a source file
src/code_generator/code_generator.cpp.i:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/code_generator/code_generator.cpp.i
.PHONY : src/code_generator/code_generator.cpp.i

src/code_generator/code_generator.s: src/code_generator/code_generator.cpp.s

.PHONY : src/code_generator/code_generator.s

# target to generate assembly for a file
src/code_generator/code_generator.cpp.s:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/code_generator/code_generator.cpp.s
.PHONY : src/code_generator/code_generator.cpp.s

src/code_generator/scope.o: src/code_generator/scope.cpp.o

.PHONY : src/code_generator/scope.o

# target to build an object file
src/code_generator/scope.cpp.o:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/code_generator/scope.cpp.o
.PHONY : src/code_generator/scope.cpp.o

src/code_generator/scope.i: src/code_generator/scope.cpp.i

.PHONY : src/code_generator/scope.i

# target to preprocess a source file
src/code_generator/scope.cpp.i:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/code_generator/scope.cpp.i
.PHONY : src/code_generator/scope.cpp.i

src/code_generator/scope.s: src/code_generator/scope.cpp.s

.PHONY : src/code_generator/scope.s

# target to generate assembly for a file
src/code_generator/scope.cpp.s:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/code_generator/scope.cpp.s
.PHONY : src/code_generator/scope.cpp.s

src/linaro_utils/utils.o: src/linaro_utils/utils.cpp.o

.PHONY : src/linaro_utils/utils.o

# target to build an object file
src/linaro_utils/utils.cpp.o:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/linaro_utils/utils.cpp.o
.PHONY : src/linaro_utils/utils.cpp.o

src/linaro_utils/utils.i: src/linaro_utils/utils.cpp.i

.PHONY : src/linaro_utils/utils.i

# target to preprocess a source file
src/linaro_utils/utils.cpp.i:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/linaro_utils/utils.cpp.i
.PHONY : src/linaro_utils/utils.cpp.i

src/linaro_utils/utils.s: src/linaro_utils/utils.cpp.s

.PHONY : src/linaro_utils/utils.s

# target to generate assembly for a file
src/linaro_utils/utils.cpp.s:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/linaro_utils/utils.cpp.s
.PHONY : src/linaro_utils/utils.cpp.s

src/main.o: src/main.cpp.o

.PHONY : src/main.o

# target to build an object file
src/main.cpp.o:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/main.cpp.o
.PHONY : src/main.cpp.o

src/main.i: src/main.cpp.i

.PHONY : src/main.i

# target to preprocess a source file
src/main.cpp.i:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/main.cpp.i
.PHONY : src/main.cpp.i

src/main.s: src/main.cpp.s

.PHONY : src/main.s

# target to generate assembly for a file
src/main.cpp.s:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/main.cpp.s
.PHONY : src/main.cpp.s

src/parsing/lexer.o: src/parsing/lexer.cpp.o

.PHONY : src/parsing/lexer.o

# target to build an object file
src/parsing/lexer.cpp.o:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/parsing/lexer.cpp.o
.PHONY : src/parsing/lexer.cpp.o

src/parsing/lexer.i: src/parsing/lexer.cpp.i

.PHONY : src/parsing/lexer.i

# target to preprocess a source file
src/parsing/lexer.cpp.i:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/parsing/lexer.cpp.i
.PHONY : src/parsing/lexer.cpp.i

src/parsing/lexer.s: src/parsing/lexer.cpp.s

.PHONY : src/parsing/lexer.s

# target to generate assembly for a file
src/parsing/lexer.cpp.s:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/parsing/lexer.cpp.s
.PHONY : src/parsing/lexer.cpp.s

src/parsing/parser.o: src/parsing/parser.cpp.o

.PHONY : src/parsing/parser.o

# target to build an object file
src/parsing/parser.cpp.o:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/parsing/parser.cpp.o
.PHONY : src/parsing/parser.cpp.o

src/parsing/parser.i: src/parsing/parser.cpp.i

.PHONY : src/parsing/parser.i

# target to preprocess a source file
src/parsing/parser.cpp.i:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/parsing/parser.cpp.i
.PHONY : src/parsing/parser.cpp.i

src/parsing/parser.s: src/parsing/parser.cpp.s

.PHONY : src/parsing/parser.s

# target to generate assembly for a file
src/parsing/parser.cpp.s:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/parsing/parser.cpp.s
.PHONY : src/parsing/parser.cpp.s

src/parsing/token.o: src/parsing/token.cpp.o

.PHONY : src/parsing/token.o

# target to build an object file
src/parsing/token.cpp.o:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/parsing/token.cpp.o
.PHONY : src/parsing/token.cpp.o

src/parsing/token.i: src/parsing/token.cpp.i

.PHONY : src/parsing/token.i

# target to preprocess a source file
src/parsing/token.cpp.i:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/parsing/token.cpp.i
.PHONY : src/parsing/token.cpp.i

src/parsing/token.s: src/parsing/token.cpp.s

.PHONY : src/parsing/token.s

# target to generate assembly for a file
src/parsing/token.cpp.s:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/parsing/token.cpp.s
.PHONY : src/parsing/token.cpp.s

src/vm/objects.o: src/vm/objects.cpp.o

.PHONY : src/vm/objects.o

# target to build an object file
src/vm/objects.cpp.o:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/vm/objects.cpp.o
.PHONY : src/vm/objects.cpp.o

src/vm/objects.i: src/vm/objects.cpp.i

.PHONY : src/vm/objects.i

# target to preprocess a source file
src/vm/objects.cpp.i:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/vm/objects.cpp.i
.PHONY : src/vm/objects.cpp.i

src/vm/objects.s: src/vm/objects.cpp.s

.PHONY : src/vm/objects.s

# target to generate assembly for a file
src/vm/objects.cpp.s:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/vm/objects.cpp.s
.PHONY : src/vm/objects.cpp.s

src/vm/value.o: src/vm/value.cpp.o

.PHONY : src/vm/value.o

# target to build an object file
src/vm/value.cpp.o:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/vm/value.cpp.o
.PHONY : src/vm/value.cpp.o

src/vm/value.i: src/vm/value.cpp.i

.PHONY : src/vm/value.i

# target to preprocess a source file
src/vm/value.cpp.i:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/vm/value.cpp.i
.PHONY : src/vm/value.cpp.i

src/vm/value.s: src/vm/value.cpp.s

.PHONY : src/vm/value.s

# target to generate assembly for a file
src/vm/value.cpp.s:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/vm/value.cpp.s
.PHONY : src/vm/value.cpp.s

src/vm/vm.o: src/vm/vm.cpp.o

.PHONY : src/vm/vm.o

# target to build an object file
src/vm/vm.cpp.o:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/vm/vm.cpp.o
.PHONY : src/vm/vm.cpp.o

src/vm/vm.i: src/vm/vm.cpp.i

.PHONY : src/vm/vm.i

# target to preprocess a source file
src/vm/vm.cpp.i:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/vm/vm.cpp.i
.PHONY : src/vm/vm.cpp.i

src/vm/vm.s: src/vm/vm.cpp.s

.PHONY : src/vm/vm.s

# target to generate assembly for a file
src/vm/vm.cpp.s:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/vm/vm.cpp.s
.PHONY : src/vm/vm.cpp.s

src/vm/vm_context.o: src/vm/vm_context.cpp.o

.PHONY : src/vm/vm_context.o

# target to build an object file
src/vm/vm_context.cpp.o:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/vm/vm_context.cpp.o
.PHONY : src/vm/vm_context.cpp.o

src/vm/vm_context.i: src/vm/vm_context.cpp.i

.PHONY : src/vm/vm_context.i

# target to preprocess a source file
src/vm/vm_context.cpp.i:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/vm/vm_context.cpp.i
.PHONY : src/vm/vm_context.cpp.i

src/vm/vm_context.s: src/vm/vm_context.cpp.s

.PHONY : src/vm/vm_context.s

# target to generate assembly for a file
src/vm/vm_context.cpp.s:
	$(MAKE) -f CMakeFiles/linaro.dir/build.make CMakeFiles/linaro.dir/src/vm/vm_context.cpp.s
.PHONY : src/vm/vm_context.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... rebuild_cache"
	@echo "... linaro"
	@echo "... edit_cache"
	@echo "... src/ast/ast.o"
	@echo "... src/ast/ast.i"
	@echo "... src/ast/ast.s"
	@echo "... src/code_generator/chunk.o"
	@echo "... src/code_generator/chunk.i"
	@echo "... src/code_generator/chunk.s"
	@echo "... src/code_generator/code_generator.o"
	@echo "... src/code_generator/code_generator.i"
	@echo "... src/code_generator/code_generator.s"
	@echo "... src/code_generator/scope.o"
	@echo "... src/code_generator/scope.i"
	@echo "... src/code_generator/scope.s"
	@echo "... src/linaro_utils/utils.o"
	@echo "... src/linaro_utils/utils.i"
	@echo "... src/linaro_utils/utils.s"
	@echo "... src/main.o"
	@echo "... src/main.i"
	@echo "... src/main.s"
	@echo "... src/parsing/lexer.o"
	@echo "... src/parsing/lexer.i"
	@echo "... src/parsing/lexer.s"
	@echo "... src/parsing/parser.o"
	@echo "... src/parsing/parser.i"
	@echo "... src/parsing/parser.s"
	@echo "... src/parsing/token.o"
	@echo "... src/parsing/token.i"
	@echo "... src/parsing/token.s"
	@echo "... src/vm/objects.o"
	@echo "... src/vm/objects.i"
	@echo "... src/vm/objects.s"
	@echo "... src/vm/value.o"
	@echo "... src/vm/value.i"
	@echo "... src/vm/value.s"
	@echo "... src/vm/vm.o"
	@echo "... src/vm/vm.i"
	@echo "... src/vm/vm.s"
	@echo "... src/vm/vm_context.o"
	@echo "... src/vm/vm_context.i"
	@echo "... src/vm/vm_context.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

