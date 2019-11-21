#pragma once

#include <random>
#include <Magnum/SceneGraph/Scene.h>
#include "MagnumTypes.h"

class MovingObject : public Object3D {
public:
    MovingObject(Scene3D& scene, const Vector3& position, const Vector3& orientation);
    explicit MovingObject(Object3D * parent);

    static void initCoefficients();

    void moveRandomly();
private:
    Vector3 _position;
    Vector3 _orientation;

    static struct Coefficients {
        float moveCoefficient{0.0f}, rotationCoefficients{0.0f};
        std::uniform_real_distribution<float> moveConstraint_x;
        std::uniform_real_distribution<float> moveConstraint_y;
        std::uniform_real_distribution<float> moveConstraint_z;
        std::uniform_real_distribution<float> rotationConstraint_x;
        std::uniform_real_distribution<float> rotationConstraint_y;
        std::uniform_real_distribution<float> rotationConstraint_z;
        std::random_device rd;
        std::mt19937 mt{rd};
    } _coefficients;
};


