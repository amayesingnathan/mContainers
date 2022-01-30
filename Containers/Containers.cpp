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
    return os << print.x << ", " << print.y << ", " << print.z;
}

template<typename T>
void PrintVec(const Labyrinth::Vector<T>& vec)
{
    for (auto& i : vec)
        std::cout << i << std::endl;
}

template<typename T>
void PrintVecSize(const Labyrinth::Vector<T>& vec)
{
    std::cout << "Vector Size: " << vec.size() << std::endl;
}

int main()
{
    std::vector<Vec3> stdver;
    Labyrinth::Vector<Vec3> a;
    a.reserve(4);

    Vec3 b(2, 5, 7);
    a.emplace(a.begin(), 5);
    a.insert(a.begin(), Vec3(2, 5, 7));
    a.insert(a.begin(), b);

    //stdver.emplace(stdver.begin(), 5);
    //stdver.insert(stdver.begin(), Vec3(2, 5, 7));
    //stdver.insert(stdver.begin(), b);

    PrintVec(a);
    PrintVecSize(a);



    //auto it = a.find(3);

    //if (it != a.end())
    //    a.erase(it);


    //a.erase(a.begin() + 1, a.begin() + 3);

    std::cin.get();
}