#include "MovingObject.h"
#include <Magnum/Math/Angle.h>

MovingObject::MovingObject(Object3D * parent) : Object{parent} {}

MovingObject::MovingObject(Scene3D &scene, const Vector3 &position, const Vector3 &orientation)
    : Object3D{}, _position{position}, _orientation(orientation) {
    this->setParent(&scene);
}

void MovingObject::initCoefficients() {
    // read config file
    _coefficients.moveCoefficient = 1.0f;
    _coefficients.rotationCoefficients = 1.0f;
    _coefficients.moveConstraint_x = std::uniform_real_distribution<float>{0.0f, 1.0f};
    _coefficients.moveConstraint_y = std::uniform_real_distribution<float>{0.0f, 1.0f};
    _coefficients.moveConstraint_z = std::uniform_real_distribution<float>{0.0f, 1.0f};
    _coefficients.rotationConstraint_x = std::uniform_real_distribution<float>{0.0f, 1.0f};
    _coefficients.rotationConstraint_y = std::uniform_real_distribution<float>{0.0f, 1.0f};
    _coefficients.rotationConstraint_z = std::uniform_real_distribution<float>{0.0f, 1.0f};

}

void MovingObject::moveRandomly() {
    Vector3 translateDelta{
        _coefficients.moveConstraint_x(_coefficients.mt),
        _coefficients.moveConstraint_y(_coefficients.mt),
        _coefficients.moveConstraint_z(_coefficients.mt)
    };

    Vector3 rotateDelta{
            _coefficients.rotationConstraint_x(_coefficients.mt),
            _coefficients.rotationConstraint_y(_coefficients.mt),
            _coefficients.rotationConstraint_z(_coefficients.mt)
    };

    _position += translateDelta;
    _orientation += rotateDelta;

    this->translate(translateDelta);
    this->rotateX(Rad{rotateDelta.x()});
    this->rotateY(Rad{rotateDelta.y()});
    this->rotateZ(Rad{rotateDelta.z()});
}
