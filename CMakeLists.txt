cmake_minimum_required(VERSION 3.4)
project(OFIGEN)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules/" ${CMAKE_MODULE_PATH})

add_subdirectory(corrade EXCLUDE_FROM_ALL)

set(CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/SDL2-2.0.10 ${CMAKE_PREFIX_PATH})
set(WITH_SDL2APPLICATION ON CACHE BOOL "" FORCE)
# OBJ import
set(WITH_ANYIMAGEIMPORTER ON CACHE BOOL "" FORCE)
set(WITH_ANYSCENEIMPORTER ON CACHE BOOL "" FORCE)
set(WITH_OBJIMPORTER ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)

# glTF import
set(WITH_STBIMAGEIMPORTER ON CACHE BOOL "" FORCE)
set(WITH_TINYGLTFIMPORTER ON CACHE BOOL "" FORCE)
add_subdirectory(magnum-plugins EXCLUDE_FROM_ALL)

add_custom_target(plugins ALL DEPENDS
        Magnum::AnyImageImporter
        Magnum::AnySceneImporter
        Magnum::ObjImporter
        MagnumPlugins::StbImageImporter
        MagnumPlugins::TinyGltfImporter)

add_subdirectory(src)
