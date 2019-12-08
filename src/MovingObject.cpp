#include "MovingObject.h"

#include <json.hpp>

std::unique_ptr<MovingObject::Coefficients> MovingObject::_coeffs = nullptr;

MovingObject::MovingObject(MovingObject *parent) : Object{parent}, _position{parent->_position},
                                                   _orientation{parent->_orientation} {
    this->setTransformation(parent->transformation());
}

MovingObject::MovingObject(Scene3D &scene, const Vector3 &position, const Vector3 &orientation)
        : Object3D{}, _position{position}, _orientation(orientation) {
    this->setParent(&scene);
}

void MovingObject::setName(const std::string &name) {
    this->_name = name;
}

void MovingObject::initCoefficients(float moveCoeff, float rotCoeff, const Vector3 &moveConstraint,
                                    const Vector3 &rotConstraint) {
    _coeffs = std::make_unique<Coefficients>(moveCoeff, rotCoeff, moveConstraint, rotConstraint);
}

void MovingObject::moveRandomly(json &objectArray) {
    Vector3 translateDelta{
            _coeffs->moveConstraint_x(_coeffs->mt),
            _coeffs->moveConstraint_y(_coeffs->mt),
            _coeffs->moveConstraint_z(_coeffs->mt)
    };

    Vector3 rotateDelta{
            _coeffs->rotationConstraint_x(_coeffs->mt),
            _coeffs->rotationConstraint_y(_coeffs->mt),
            _coeffs->rotationConstraint_z(_coeffs->mt)
    };

    Vector3 oldPosition = _position;
    Vector3 oldOrientation = _orientation;

    translateDelta *= _coeffs->moveCoefficient;
    rotateDelta *= _coeffs->rotationCoefficient;

    _position += translateDelta;
    _orientation += rotateDelta;

    this->translate(translateDelta);
    this->rotateX(Rad{rotateDelta.x()});
    this->rotateY(Rad{rotateDelta.y()});
    this->rotateZ(Rad{rotateDelta.z()});

    writeData(objectArray, oldPosition, oldOrientation);
}

void MovingObject::move(const Vector3 &v) {
    _position += v;
    this->translate(v);
}

void MovingObject::writeData(json & objectArray, const Vector3 & oldPos, const Vector3 & oldOrient) {
    auto dP = _position - oldPos;
    auto dO = _orientation - oldOrient;

    objectArray.push_back(
            {
                {"name", _name},
                {"position_delta",  {
                      {"x", dP.x()},
                      {"y", dP.y()},
                      {"z", dP.z()},}
                },
                {"orientation_delta", {
                      {"x", dO.x()},
                      {"y", dO.y()},
                      {"z", dO.z()}, }
                }
        }
    );
}

MovingObject::Coefficients::Coefficients(float moveCoefficient, float rotationCoefficient,
                                         const Vector3 &moveConstraint, const Vector3 &rotationConstraint)
        : moveCoefficient{moveCoefficient}, rotationCoefficient{rotationCoefficient},
          moveConstraint_x{0.0f, moveConstraint.x()}, moveConstraint_y{0.0f, moveConstraint.y()},
          moveConstraint_z{0.0f, moveConstraint.z()},
          rotationConstraint_x{0.0f, rotationConstraint.x()}, rotationConstraint_y{0.0f, rotationConstraint.y()},
          rotationConstraint_z{0.0f, rotationConstraint.z()},
          mt{std::random_device{}()} {}
