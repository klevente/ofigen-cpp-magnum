#pragma once
#include <Magnum/GL/Mesh.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Math/Color.h>
#include "MagnumTypes.h"

class BackgroundDrawable : public SceneGraph::Drawable3D {
public:
    BackgroundDrawable(Object3D & object, Shaders::Flat3D & shader, GL::Mesh & mesh, GL::Texture2D & texture, SceneGraph::DrawableGroup3D & group);
private:
    void draw(const Matrix4 &transformationMatrix, SceneGraph::Camera3D & camera) override;

    Shaders::Flat3D & _shader;
    GL::Mesh & _mesh;
    GL::Texture2D & _texture;

};


