#pragma once

#include <string>
#include <random>
#include <memory>
#include <Magnum/SceneGraph/Scene.h>
#include <json.hpp>
#include "MagnumTypes.h"

class MovingObject : public Object3D {
    using json = nlohmann::json;
public:
    explicit MovingObject(Scene3D & scene, const Vector3 & position = {}, const Vector3 & orientation = {});
    explicit MovingObject(MovingObject * parent);

    static void initCoefficients(float moveCoeff, float rotCoeff, const Vector3 & moveConstraint, const Vector3 & rotConstraint);

    void setName(const std::string & name);
    void moveRandomly(json & objectArray);
    void move(const Vector3 & v);
    void writeData(json & objectArray, const Vector3 & oldPos, const Vector3 & oldOrient);
private:
    Vector3 _position;
    Vector3 _orientation;

    std::string _name;

    struct Coefficients {
        float moveCoefficient, rotationCoefficient;
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