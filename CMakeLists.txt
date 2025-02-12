cmake_minimum_required(VERSION 3.29.2)

project(ProtoText)

# Set Unicode definition
add_definitions(-DUNICODE -D_UNICODE)

#The /MANIFEST:NO flag tells the linker not to generate or embed a manifest.
if (WIN32)
    set(CMAKE_SHARED_LINKER_FLAGS /MANIFEST:NO)
endif()

file(GLOB SRC "src/*.cpp")

include_directories(include)
add_executable(${PROJECT_NAME} WIN32 ${SRC})

#Make application dpiAware
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND "mt.exe" -manifest \"${CMAKE_SOURCE_DIR}/dpiAware.manifest\" -outputresource:"${CMAKE_SOURCE_DIR}/out/build/x64-Debug/${PROJECT_NAME}.exe"\;\#1
    COMMENT "Adding manifest..." 
)

# Link necessary libraries
target_link_libraries(${PROJECT_NAME} 
    User32 
    Gdi32 
    Kernel32 
    Advapi32
    DWrite
    d2d1
    comctl32
)
