find_package(Corrade REQUIRED Main)
find_package(Magnum REQUIRED
        GL
        MeshTools
        Shaders
        SceneGraph
        Trade
        Sdl2Application)

set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

include_directories(../include)

add_executable(OFIGEN WIN32 Ofigen.cpp ColoredDrawable.cpp ColoredDrawable.h TexturedDrawable.cpp TexturedDrawable.h MagnumTypes.h MovingObject.cpp MovingObject.h)
target_compile_features(OFIGEN PUBLIC cxx_std_17)
target_link_libraries(OFIGEN PRIVATE
    Corrade::Main
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
    Magnum::MeshTools
    Magnum::SceneGraph
    Magnum::Shaders
    Magnum::Trade)

# Make the executable a default target to build & run in Visual Studio
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT OFIGEN)
