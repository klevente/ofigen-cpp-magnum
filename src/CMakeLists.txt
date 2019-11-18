find_package(Corrade REQUIRED Main)
find_package(Magnum REQUIRED
        GL
        MeshTools
        Shaders
        SceneGraph
        Trade
        Sdl2Application)

set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

add_executable(OFIGEN WIN32 Ofigen.cpp)
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
