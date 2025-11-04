#pragma once

#include <cmath>
#include <iostream>
#include <array>

template <typename T, std::size_t N>
class Vector {
    private:
    std::array<T, N> data;

    public:
    // Constructors
    Vector() {
    data.fill(static_cast<T>(0));
    }

    explicit Vector(const std::array<T, N>& values) : data(values) {}

    Vector(std::initializer_list<T> list) {
        if (list.size() != N)
            throw std::invalid_argument("Initializer list size must match vector dimension.");
        std::copy(list.begin(), list.end(), data.begin());
    }

    // Accessors
    T& operator[](std::size_t i) { return data[i]; }
    const T& operator[](std::size_t i) const { return data[i]; }

    std::size_t size() const { return N; }

    // Magnitude and normalization
    T magnitude() const {
        T sum = 0;
        for (const auto& v : data) sum += v * v;
        return std::sqrt(sum);
    }

    Vector normalized() const {
        T mag = magnitude();
        if (mag == 0) return Vector();
        Vector result;
        for (std::size_t i = 0; i < N; ++i)
            result[i] = data[i] / mag;
        return result;
    }

    // Dot product
    T dot(const Vector& other) const {
        T sum = 0;
        for (std::size_t i = 0; i < N; ++i)
            sum += data[i] * other.data[i];
        return sum;
    }

    // Cross product (only defined for 3D)
    template <std::size_t M = N>
    typename std::enable_if<M == 3, Vector>::type cross(const Vector& other) const {
        return Vector({
            data[1] * other[2] - data[2] * other[1],
            data[2] * other[0] - data[0] * other[2],
            data[0] * other[1] - data[1] * other[0]
        });
    }

    // Operator overloads
    Vector operator+(const Vector& rhs) const {
        Vector result;
        for (std::size_t i = 0; i < N; ++i)
            result[i] = data[i] + rhs[i];
        return result;
    }

    Vector operator-(const Vector& rhs) const {
        Vector result;
        for (std::size_t i = 0; i < N; ++i)
            result[i] = data[i] - rhs[i];
        return result;
    }

    Vector operator*(T scalar) const {
        Vector result;
        for (std::size_t i = 0; i < N; ++i)
            result[i] = data[i] * scalar;
        return result;
    }

    Vector operator/(T scalar) const {
        Vector result;
        if (scalar == 0) return result;
        for (std::size_t i = 0; i < N; ++i)
            result[i] = data[i] / scalar;
        return result;
    }

    Vector& operator+=(const Vector& rhs) {
        for (std::size_t i = 0; i < N; ++i)
            data[i] += rhs[i];
        return *this;
    }

    Vector& operator-=(const Vector& rhs) {
        for (std::size_t i = 0; i < N; ++i)
            data[i] -= rhs[i];
        return *this;
    }

    Vector& operator*=(T scalar) {
        for (auto& v : data)
            v *= scalar;
        return *this;
    }

    Vector& operator/=(T scalar) {
        if (scalar != 0)
            for (auto& v : data)
                v /= scalar;
        return *this;
    }

    bool operator==(const Vector& rhs) const {
        for (std::size_t i = 0; i < N; ++i)
            if (data[i] != rhs[i]) return false;
        return true;
    }

    bool operator!=(const Vector& rhs) const { return !(*this == rhs); }

    // Stream output
    friend std::ostream& operator<<(std::ostream& os, const Vector& v) {
        os << "(";
        for (std::size_t i = 0; i < N; ++i) {
            os << v[i];
            if (i != N - 1) os << ", ";
        }
        os << ")";
        return os;
    }


};