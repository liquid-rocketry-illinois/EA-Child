#pragma once

#include <cmath>
#include <iostream>

class Vector3D {
protected:
    double x, y, z;

public:
    // Constructors
    Vector3D() : x(0), y(0), z(0) {}
    Vector3D(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    // Copy constructor
    Vector3D(const Vector3D& other) : x(other.x), y(other.y), z(other.z) {}

    // Accessors
    double getX() const { return x; }
    double getY() const { return y; }
    double getZ() const { return z; }

    void setX(double val) { x = val; }
    void setY(double val) { y = val; }
    void setZ(double val) { z = val; }

    // Magnitude and normalization
    double magnitude() const {
        return std::sqrt(x*x + y*y + z*z);
    }

    Vector3D normalized() const {
        double mag = magnitude();
        return (mag > 0) ? Vector3D(x/mag, y/mag, z/mag) : Vector3D(0.,0.,0.);
    }

    // Dot product
    double dot(const Vector3D& other) const {
        return x*other.x + y*other.y + z*other.z;
    }

    // Cross product
    Vector3D cross(const Vector3D& other) const {
        return Vector3D(
            y*other.z - z*other.y,
            z*other.x - x*other.z,
            x*other.y - y*other.x
        );
    }

    // Operator overloads
    Vector3D operator+(const Vector3D& rhs) const {
        return Vector3D(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Vector3D operator-(const Vector3D& rhs) const {
        return Vector3D(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    Vector3D operator*(double scalar) const {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }

    Vector3D operator/(double scalar) const {
        return (scalar != 0) ? Vector3D(x / scalar, y / scalar, z / scalar) : Vector3D(0.,0.,0.);
    }

    Vector3D& operator+=(const Vector3D& rhs) {
        x += rhs.x; y += rhs.y; z += rhs.z;
        return *this;
    }

    Vector3D& operator-=(const Vector3D& rhs) {
        x -= rhs.x; y -= rhs.y; z -= rhs.z;
        return *this;
    }

    Vector3D& operator*=(double scalar) {
        x *= scalar; y *= scalar; z *= scalar;
        return *this;
    }

    Vector3D& operator/=(double scalar) {
        if (scalar != 0) { x /= scalar; y /= scalar; z /= scalar; }
        return *this;
    }

    bool operator==(const Vector3D& rhs) const {
        return (x == rhs.x && y == rhs.y && z == rhs.z);
    }

    bool operator!=(const Vector3D& rhs) const {
        return !(*this == rhs);
    }

    // Stream output
    friend std::ostream& operator<<(std::ostream& os, const Vector3D& v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }
};


// -----------------------------
// Pointer version
// -----------------------------

class Vector3DPtr {
protected:
    double *x, *y, *z;

public:
    // Constructor with pointers
    Vector3DPtr(double* x_, double* y_, double* z_) : x(x_), y(y_), z(z_) {}

    // Accessors
    double getX() const { return *x; }
    double getY() const { return *y; }
    double getZ() const { return *z; }

    void setX(double val) { *x = val; }
    void setY(double val) { *y = val; }
    void setZ(double val) { *z = val; }

    // Magnitude and normalization
    double magnitude() const {
        return std::sqrt((*x)*(*x) + (*y)*(*y) + (*z)*(*z));
    }

    Vector3D normalized() const {
        double mag = magnitude();
        return (mag > 0) ? Vector3D(*x/mag, *y/mag, *z/mag) : Vector3D(0.,0.,0.);
    }

    // Dot product
    double dot(const Vector3DPtr& other) const {
        return (*x)*(*other.x) + (*y)*(*other.y) + (*z)*(*other.z);
    }

    // Cross product
    Vector3D cross(const Vector3DPtr& other) const {
        return Vector3D(
            (*y)*(*other.z) - (*z)*(*other.y),
            (*z)*(*other.x) - (*x)*(*other.z),
            (*x)*(*other.y) - (*y)*(*other.x)
        );
    }

    // Stream output
    friend std::ostream& operator<<(std::ostream& os, const Vector3DPtr& v) {
        os << "(" << *v.x << ", " << *v.y << ", " << *v.z << ")";
        return os;
    }
};

// -----------------------------
// Free functions (helpers)
// -----------------------------

inline double distance(const Vector3D& a, const Vector3D& b) {
    return (a - b).magnitude();
}

inline double angleBetween(const Vector3D& a, const Vector3D& b) {
    double magA = a.magnitude();
    double magB = b.magnitude();
    if (magA == 0 || magB == 0) return 0.0;
    return std::acos(a.dot(b) / (magA * magB));
}