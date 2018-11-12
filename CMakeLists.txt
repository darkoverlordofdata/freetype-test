cmake_minimum_required ( VERSION 3.1 )
###################################################################################
#
#   Build freetype-test
#
###################################################################################
project ( freetype-test VERSION 0.0.1 )

#
set ( VERSION 0.0.1 )
set ( RELEASE_NAME freetype-test )

#
# enable features
#
include ( CMakeToolsHelpers OPTIONAL )

#
#   application source
#
list ( APPEND C_SOURCES src/main.c )
list ( APPEND C_SOURCES src/utils.c )
#
#   application includes
#
set ( INCLUDES "${INCLUDES};./include" )

# Libs: 
set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lglfw3" )
set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lglew32" )
set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lfreetype" )
set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lopengl32" )

# flags
set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w" )
set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99" )
set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3" )

# definitions 

include_directories(AFTER ${INCLUDES})

#
#   compile ccode & link executable
#
add_executable ( ${PROJECT_NAME} ${C_SOURCES} ${RESOURCES} )
add_custom_command(TARGET ${PROJECT_NAME} 
                   POST_BUILD COMMAND 
                   ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> .. )
target_link_libraries ( ${PROJECT_NAME} glfw3 glew32 freetype opengl32 )
install (TARGETS ${PROJECT_NAME} RUNTIME DESTINATION . )

