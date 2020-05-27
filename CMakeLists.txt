cmake_minimum_required(VERSION 3.1)

project(invaders
  LANGUAGES C
  DESCRIPTION "A Space Invaders emulator"
  HOMEPAGE_URL "https://github.com/superzazu/invaders"
)

set(SOURCES
  deps/8080/i8080.c
  deps/SDL_nmix/SDL_nmix.c
  deps/SDL_nmix/SDL_nmix_file.c
  src/main.c
  src/invaders.c
)
set(ROMS_DIR "./roms/" CACHE STRING "Path to directory containing rom files")

add_executable(invaders ${SOURCES})
set_target_properties(invaders PROPERTIES C_STANDARD 99)
if (MSVC)
  target_compile_options(invaders PRIVATE /W4)
else()
  target_compile_options(invaders PRIVATE -Wall -Wextra -pedantic -Wno-gnu-binary-literal)
endif()

if (EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_SDL=2")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --preload-file ${ROMS_DIR}@/roms")
  # stub file to avoid errors on "find_package(SDL2 REQUIRED)" in SDL_sound
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/src/cmake")
else()
  find_package(SDL2 REQUIRED)
  target_include_directories(invaders PRIVATE ${SDL2_INCLUDE_DIRS})
  target_link_libraries(invaders PRIVATE ${SDL2_LIBRARIES})
endif()

# SDL2_sound
option(SDLSOUND_DECODER_WAV "" ON)
option(SDLSOUND_DECODER_AIFF "" OFF)
option(SDLSOUND_DECODER_AU "" OFF)
option(SDLSOUND_DECODER_VOC "" OFF)
option(SDLSOUND_DECODER_FLAC "" OFF)
option(SDLSOUND_DECODER_VORBIS "" OFF)
option(SDLSOUND_DECODER_RAW "" OFF)
option(SDLSOUND_DECODER_SHN "" OFF)
option(SDLSOUND_DECODER_MODPLUG "" OFF)
option(SDLSOUND_DECODER_MP3 "" OFF)
option(SDLSOUND_DECODER_COREAUDIO "" OFF)
option(SDLSOUND_BUILD_STATIC "" ON)
option(SDLSOUND_BUILD_SHARED "" OFF)
option(SDLSOUND_BUILD_TEST "" OFF)
add_subdirectory(deps/SDL_sound EXCLUDE_FROM_ALL)

target_link_libraries(invaders PRIVATE SDL2_sound-static)
target_include_directories(invaders PRIVATE
  src/
  deps/
  deps/SDL_nmix
  deps/SDL_sound/src
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" AND NOT EMSCRIPTEN)
  set_target_properties(invaders PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "invaders"
    MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
    MACOSX_BUNDLE_COPYRIGHT "superzazu"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.nicolasallemand.invaders"
  )

  file(COPY ${ROMS_DIR} DESTINATION
    "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Resources")

  # add NSHighResolutionCapable property in Info.plist for Retina screens
  add_custom_command(TARGET invaders POST_BUILD
    COMMAND plutil -replace NSHighResolutionCapable -bool true
    "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Info.plist"
  )

  # fixup the bundle to make it distributable
  install(CODE "
    include(BundleUtilities)
    set(BU_CHMOD_BUNDLE_ITEMS TRUE)
    fixup_bundle(${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app \"\" \"\")
    " COMPONENT Runtime)
endif()
