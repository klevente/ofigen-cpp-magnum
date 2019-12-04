#include "TexturedDrawable.h"

void TexturedDrawable::draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) {
    _shader
            .setLightPosition(camera.cameraMatrix().transformPoint({-3.0f, 10.0f, 10.0f}))
            .setTransformationMatrix(transformationMatrix)
            .setNormalMatrix(transformationMatrix.normalMatrix())
            .setProjectionMatrix(camera.projectionMatrix())
            .bindDiffuseTexture(_texture);

    _mesh.draw(_shader);
}

TexturedDrawable::TexturedDrawable(Object3D &object, Shaders::Phong &shader, GL::Mesh &mesh, GL::Texture2D &texture,
                                   SceneGraph::DrawableGroup3D &group)
        : SceneGraph::Drawable3D{object, &group}, _shader{shader}, _mesh{mesh}, _texture{texture} {}
