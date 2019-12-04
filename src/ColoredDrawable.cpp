#include "ColoredDrawable.h"

void ColoredDrawable::draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) {
    _shader
            .setDiffuseColor(_color)
            .setLightPosition(camera.cameraMatrix().transformPoint({-3.0f, 10.0f, 10.0f}))
            .setTransformationMatrix(transformationMatrix)
            .setNormalMatrix(transformationMatrix.normalMatrix())
            .setProjectionMatrix(camera.projectionMatrix());

    _mesh.draw(_shader);
}

ColoredDrawable::ColoredDrawable(Object3D &object, Shaders::Phong &shader, GL::Mesh &mesh, const Color4 &color,
                                 SceneGraph::DrawableGroup3D &group) : SceneGraph::Drawable3D{object, &group}, _shader{shader}, _mesh{mesh}, _color{color} {}
