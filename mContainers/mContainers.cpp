// Containers.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include "Vector.h"
#include "List.h"
#include "Dictionary.h"

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
        //std::cout << "Copied\n";
    }

    Vec3(const Vec3&& other) noexcept
        : x(other.x), y(other.y), z(other.z)
    {
        //std::cout << "Moved\n";
    }


    ~Vec3()
    {
        //std::cout << "Destroyed\n";
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
void PrintVec(const mContainers::Vector<T>& vec)
{
    for (auto& i : vec)
        std::cout << i << std::endl;
}

template<typename T>
void PrintVecSize(const mContainers::Vector<T>& vec)
{
    std::cout << "Vector Size: " << vec.size() << std::endl;
}

void print(int x)
{
    std::cout << x << std::endl;
}

int main()
{
    mContainers::Vector<Vec3> myVec;
    mContainers::List<Vec3> myList;
    mContainers::Dictionary<int, std::string> myDict;

    for (int i = 0; i < 10000; i++)
        myDict.emplace(i, std::to_string(i));

    myDict.printCollisionDistData();

}