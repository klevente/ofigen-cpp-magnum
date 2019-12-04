#pragma once

#include <Magnum/GL/Mesh.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Math/Color.h>
#include "MagnumTypes.h"

class ColoredDrawable : public SceneGraph::Drawable3D {
public:
    ColoredDrawable(Object3D& object, Shaders::Phong& shader, GL::Mesh& mesh, const Color4& color, SceneGraph::DrawableGroup3D& group);

private:
    void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) override;

    Shaders::Phong& _shader;
    GL::Mesh& _mesh;
    Color4 _color;
};


