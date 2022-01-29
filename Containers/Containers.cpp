// Containers.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>
#include "LabVec.h"

struct Vec3
{
    int x;
    int y;
    int z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(int scalar) : x(scalar), y(scalar), z(scalar) { }
    Vec3(int x, int y, int z) : x(x), y(y), z(z) { }

    Vec3(const Vec3& other)
        : x(other.x), y(other.y), z(other.z)
    {
        std::cout << "Copied\n";
    }

    Vec3(const Vec3&& other)
        : x(other.x), y(other.y), z(other.z)
    {
        std::cout << "Moved\n";
    }


    ~Vec3()
    {
        std::cout << "Destroyed\n";
    }

    Vec3& operator= (const Vec3& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;

        return *this;
    }

};

std::ostream& operator<< (std::ostream& os, const Vec3& print)
{
    return os << print.x << ", " << print.y << ", " << print.z << "\n";
}

int main()
{
    Labyrinth::Vector<Vec3> a;

    a.emplace_back(1, 2, 5);
    a.emplace_back(4);
    a.emplace_back();
    a.emplace_back(6, 5, 7);
    a.emplace_back(10);

    for (auto& i : a)
    {
        std::cout << i;
    }

    std::cout << "Vector Size: " << a.size() << std::endl;

    //a.resize(8);

    //auto it = a.find(3);

    //if (it != a.end())
    //    a.erase(it);


    a.erase(a.begin() + 1, a.begin() + 3);

    for (auto& i : a)
    {
        std::cout << i;
    }

    std::cout << "Vector Size: " << a.size() << std::endl;

    std::cin.get();
}