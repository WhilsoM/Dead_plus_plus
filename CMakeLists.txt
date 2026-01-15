cmake_minimum_required(VERSION 3.22)
project(MyHorrorGame)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ОБЯЗАТЕЛЬНО для статической сборки, чтобы убрать __imp__ ошибки
add_definitions(-DSFML_STATIC)

set(SFML_INSTALL_DIR "/Users/samuraiqwizlio/Downloads/SFML-3.0.2/build-mingw/install")

include_directories("${SFML_INSTALL_DIR}/include")
link_directories("${SFML_INSTALL_DIR}/lib")

add_executable(MyHorrorGame src/main.cpp)

# В SFML 3 статические либы называются без суффикса -s, если ты их так собрал, 
# но судя по твоему 'ls', они называются с '-s'. Проверь имена файлов в папке lib!
target_link_libraries(MyHorrorGame PRIVATE
    sfml-graphics-s
    sfml-window-s
    sfml-audio-s
    sfml-network-s
    sfml-system-s
    sfml-main
    freetype
    vorbisenc
    vorbisfile
    vorbis
    ogg
    FLAC
    opengl32
    winmm
    gdi32
    user32
    advapi32
    shell32
    shlwapi
    dwmapi
)

set_target_properties(MyHorrorGame PROPERTIES WIN32_EXECUTABLE TRUE)
