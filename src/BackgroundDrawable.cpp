#include "BackgroundDrawable.h"

BackgroundDrawable::BackgroundDrawable(Object3D &object, Shaders::Flat3D &shader, GL::Mesh &mesh,
                                       GL::Texture2D &texture, SceneGraph::DrawableGroup3D &group)
        : SceneGraph::Drawable3D{object, &group}, _shader{shader}, _mesh{mesh}, _texture{texture} {}

void BackgroundDrawable::draw(const Matrix4 & transformationMatrix, SceneGraph::Camera3D & camera) {
    _shader
        .setTransformationProjectionMatrix(camera.projectionMatrix() * transformationMatrix)
        .bindTexture(_texture);

    _mesh.draw(_shader);
}
