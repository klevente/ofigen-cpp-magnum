#pragma once

#include <map>
#include <Magnum/Trade/MeshObjectData3D.h>
using namespace Magnum;

struct ObjectData {
    // std::vector<Containers::Pointer<Trade::ObjectData3D>> rootObjects;
    // std::vector<Containers::Pointer<Trade::ObjectData3D>> childrenObjects;

    std::map<unsigned int, Containers::Pointer<Trade::ObjectData3D>> rootObjects;
    std::map<unsigned int, Containers::Pointer<Trade::ObjectData3D>> childrenObjects;
};


