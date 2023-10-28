# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(PROJECT_NAME "helloWorld")

#-------------------------------------------------------MY-------------------------------------------------------

option(AUTO_LOCATE_VULKAN "AUTO_LOCATE_VULKAN" ON)

if(AUTO_LOCATE_VULKAN)
	message(STATUS "Attempting auto locate Vulkan using CMake......")
	
	# Find Vulkan Path using CMake's Vulkan Module. This will return: 
	# Boolean 'Vulkan_FOUND' - the status of find as success(ON) or fail(OFF),
	# 'Vulkan_INCLUDE_DIRS' - Include directory path,
	# 'Vulkan_LIBRARY' - required libraries.
	find_package(Vulkan)

	if(${Vulkan_LIBRARY} STREQUAL "Vulkan_LIBRARY-NOTFOUND")
		message(STATUS "Vulkan_LIBRARY not found.")
	endif()
	
	# Try extracting VulkanSDK path from ${Vulkan_INCLUDE_DIRS}
	if(${Vulkan_INCLUDE_DIRS} STREQUAL "Vulkan_INCLUDE_DIRS-NOTFOUND")
		message(STATUS "Vulkan_INCLUDE_DIRS not found.")
	else()
		message(STATUS "Vulkan_INCLUDE_DIRS found: ${Vulkan_INCLUDE_DIRS} .")
		set(Vulkan_PATH ${Vulkan_INCLUDE_DIRS})
		STRING(REGEX REPLACE "/Include" "" Vulkan_PATH ${Vulkan_PATH})
	endif()
		 
	if(NOT Vulkan_FOUND)
		# CMake may fail to locate the libraries but could be able to 
		# provide some path in Vulkan SDK include directory variable
		# 'Vulkan_INCLUDE_DIRS', try to extract path from this.
		message(STATUS "Failed to locate Vulkan SDK, retrying again...")
		if(EXISTS "${Vulkan_PATH}")
			message(STATUS "Successfully located the Vulkan SDK from Vulkan_INCLUDE_DIRS: ${Vulkan_PATH}")
		else()
			message("Error: Unable to locate Vulkan SDK. Please turn off auto locate option by specifying 'AUTO_LOCATE_VULKAN' as 'OFF'")
			message("and specify manually path using 'VULKAN_SDK' and 'VULKAN_VERSION' variables in the CMakeLists.txt.")
			return()
		endif()
	endif()
else()
	message(STATUS "Attempting to locate Vulkan SDK using manual path......")
	set(Vulkan_SDK "D:/Programs/VulkanSDK")
	set(Vulkan_VERSION "1.3.250.1")
	set(Vulkan_PATH "${Vulkan_SDK}/${Vulkan_VERSION}")
	message(STATUS "Using manual specified path: ${Vulkan_PATH}")

	# Check if manual set path exists
	if(NOT EXISTS "${Vulkan_PATH}")
		message("Error: Unable to locate this Vulkan SDK path Vulkan_PATH: ${Vulkan_PATH}, please specify correct path.")
	   return()
	endif()
endif()

#-------------------------------------------------------MY-------------------------------------------------------


project (${PROJECT_NAME})

# Include sub-projects.
# add_subdirectory (${PROJECT_NAME})


#-------------------------------------------------------MY-------------------------------------------------------

# Add any required preprocessor definitions here.
# WSI needs the VK_KHR_WIN32_SURFACE_EXTENSION_NAME extension API,
# for this, we need to define the VK_USE_PLATFORM_WIN32_KHR preprocessor directives.
add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)

# Specify required libraries in the Vulkan_LIB_LINK_LIST variable,
# and later link it to the building project. Also, specify the path
# from where the linked libraries are located.
set(Vulkan_LIB_LIST "vulkan-1")
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	# header files
	include_directories(AFTER ${Vulkan_PATH}/Include)
	# .exe/.dll
 	link_directories(${Vulkan_PATH}/Bin; ${VULKAN_PATH}/Lib)
endif()

# Group the header and source files together in respective
# folder for better visualization and management.
if (WIN32)
 	source_group("include" REGEX "include/*")
 	source_group("source" REGEX "source/*")
endif(WIN32)

# Specify the sample's header file path. Read and store all the
# header and source files in the sample using file() API and store
# them in the CPP_LISTS and HPP_LISTS variables. These lists are to
# inform that they need to be compiled.
# Define inlcude directories.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
# Gather list of header and source files for compilation.
file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
file(GLOB_RECURSE HPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.*)
# Build the project, provide name and cpp/hpp files to be compiled.
add_executable(${PROJECT_NAME} ${CPP_FILES} ${HPP_FILES} "include/Headers.h" "include/VulkanInstance.h" "include/VulkanApplication.h" "source/VulkanApplication.cpp" "source/VulkanDevice.cpp" "include/CommandBufferManager.h" "source/CommandBufferManager.cpp")
# Link the debug and release libraries to the project.
target_link_libraries(${PROJECT_NAME} ${Vulkan_PATH}/Lib/${Vulkan_LIB_LIST}.lib)

# Define the project properties.
# Speciy the path of the binary executable
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
# Define the correct C/C++ standard versions.
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD_REQUIRED ON)

#-------------------------------------------------------MY-------------------------------------------------------
