#pragma once

#include <vector>
#include <string>
#include <cmath>

struct Vector3D
{
    double x, y, z;

    Vector3D() : x(0), y(0), z(0) {}
    Vector3D(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    double norm() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vector3D normalized() const {
        double n = norm();
        if (n < 1e-10) return Vector3D(0, 0, 1);  // Default direction
        return Vector3D(x / n, y / n, z / n);
    }

    Vector3D operator-(const Vector3D& other) const {
        return Vector3D(x - other.x, y - other.y, z - other.z);
    }

    Vector3D operator+(const Vector3D& other) const {
        return Vector3D(x + other.x, y + other.y, z + other.z);
    }

    Vector3D operator*(double scalar) const {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }

    // Cross product
    Vector3D cross(const Vector3D& other) const {
        return Vector3D(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // Dot product
    double dot(const Vector3D& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
};

struct VectorFieldPoint
{
    Vector3D position;
    Vector3D vector;

    VectorFieldPoint() {}
    VectorFieldPoint(const Vector3D& pos, const Vector3D& vec)
        : position(pos), vector(vec) {}
};

struct LoadedVectorField3D
{
    std::string title;
    std::vector<VectorFieldPoint> vectors;

    double minMagnitude;
    double maxMagnitude;
    Vector3D minBounds;
    Vector3D maxBounds;

    LoadedVectorField3D() : minMagnitude(1e10), maxMagnitude(0) {}

    void calculateBounds() {
        if (vectors.empty()) return;

        minBounds = maxBounds = vectors[0].position;
        minMagnitude = maxMagnitude = vectors[0].vector.norm();

        for (const auto& v : vectors) {
            // Position bounds
            minBounds.x = std::min(minBounds.x, v.position.x);
            minBounds.y = std::min(minBounds.y, v.position.y);
            minBounds.z = std::min(minBounds.z, v.position.z);
            maxBounds.x = std::max(maxBounds.x, v.position.x);
            maxBounds.y = std::max(maxBounds.y, v.position.y);
            maxBounds.z = std::max(maxBounds.z, v.position.z);

            // Magnitude bounds
            double mag = v.vector.norm();
            minMagnitude = std::min(minMagnitude, mag);
            maxMagnitude = std::max(maxMagnitude, mag);
        }
    }
};
