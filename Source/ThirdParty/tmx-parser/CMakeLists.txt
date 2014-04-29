# Define target name
set (TARGET_NAME tmx-parser)

# Define source files
file (GLOB CPP_FILES base64/*.cpp tinyxml/*.cpp zlib/*.c *.cpp)
file (GLOB H_FILES base64/*.h tinyxml/*.h zlib/*.h *.h)
set (SOURCE_FILES ${CPP_FILES} ${H_FILES})

# Define dependency libs
set (INCLUDE_DIRS_ONLY base64 tinyxml zlib)

# Setup target
setup_library ()
