#pragma once

#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Shaders/Phong.h>
#include "MagnumTypes.h"

class TexturedDrawable: public SceneGraph::Drawable3D {
public:
    explicit TexturedDrawable(Object3D & object, Shaders::Phong & shader, GL::Mesh & mesh, GL::Texture2D & texture, SceneGraph::DrawableGroup3D & group);

private:
    void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) override;

    Shaders::Phong& _shader;
    GL::Mesh& _mesh;
    GL::Texture2D& _texture;
};


