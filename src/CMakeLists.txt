find_package(Corrade REQUIRED Main)
find_package(Magnum REQUIRED
        GL
        MeshTools
        Shaders
        SceneGraph
        Trade
        Sdl2Application
        Primitives)

set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

add_library(zlib STATIC IMPORTED)
set_property(TARGET zlib PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/zlib/lib/zlib.lib)
add_library(libpng STATIC IMPORTED)
set_property(TARGET libpng PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/libpng/lib/libpng16_staticd.lib)

include_directories(../zlib/include)
include_directories(../libpng/include)
include_directories(../include)

add_executable(OFIGEN WIN32 Ofigen.cpp ColoredDrawable.cpp ColoredDrawable.h TexturedDrawable.cpp TexturedDrawable.h MagnumTypes.h MovingObject.cpp MovingObject.h ObjectData.h BackgroundDrawable.cpp BackgroundDrawable.h)
target_compile_features(OFIGEN PUBLIC cxx_std_17)
target_link_libraries(OFIGEN PRIVATE
    Corrade::Main
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
    Magnum::MeshTools
    Magnum::SceneGraph
    Magnum::Shaders
    Magnum::Trade
    Magnum::Primitives
    zlib
    libpng)

# Make the executable a default target to build & run in Visual Studio
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT OFIGEN)
