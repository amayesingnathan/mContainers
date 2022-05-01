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

int main()
{
    //std::list<Vec3> stdlist(3, 1);

    //std::vector<Vec3> stdver;

    mContainers::Dictionary<int, std::string> myDict;

    for (int i = 0; i < 9; i++)
        myDict.emplace(i, std::to_string(i));

    //auto it = a.find(3);

    //if (it != a.end())
    //    a.erase(it);


    //a.erase(a.begin() + 1, a.begin() + 3);
}