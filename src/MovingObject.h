#pragma once

#include <random>
#include <memory>
#include <Magnum/SceneGraph/Scene.h>
#include "MagnumTypes.h"

class MovingObject : public Object3D {
public:
    MovingObject(Scene3D& scene, const Vector3& position, const Vector3& orientation);
    explicit MovingObject(MovingObject * parent);

    static void initCoefficients();

    void moveRandomly();
private:
    Vector3 _position;
    Vector3 _orientation;

    struct Coefficients {
        float moveCoefficient, rotationCoefficients;
        std::uniform_real_distribution<float> moveConstraint_x;
        std::uniform_real_distribution<float> moveConstraint_y;
        std::uniform_real_distribution<float> moveConstraint_z;
        std::uniform_real_distribution<float> rotationConstraint_x;
        std::uniform_real_distribution<float> rotationConstraint_y;
        std::uniform_real_distribution<float> rotationConstraint_z;
        std::mt19937 mt;

        Coefficients(float moveCoefficient, float rotationCoefficient, const Vector3 & moveConstraint, const Vector3 & rotationConstraint);
    };

    static std::unique_ptr<Coefficients> _coeffs;
};


