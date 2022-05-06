// mContainers.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "mpch.h"

#include "mContainers.h"

#include <unordered_map>

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
void PrintVec(const mContainers::mDynArray<T>& vec)
{
    for (auto& i : vec)
        std::cout << i << std::endl;
}

template<typename T>
void PrintVecSize(const mContainers::mDynArray<T>& vec)
{
    std::cout << "Vector Size: " << vec.size() << std::endl;
}

void print(int x)
{
    std::cout << x << std::endl;
}

using namespace mContainers;

void TestStdDict()
{
    std::cout << "STD Unordered Map:\n";
    mTimer timer;
    std::unordered_map<int, std::string> myDict;

    for (int i = 0; i < 1000000; i++)
        myDict.emplace(i, std::to_string(i));

    std::cout << "Add Time:\t" + std::to_string(timer.elapsedMillis()) << std::endl;

    timer.reset();
    size_t count = 0;
    for (auto& kv : myDict)
        count++;

    std::cout << "Iteration Time:\t" + std::to_string(timer.elapsedMillis()) << std::endl;
}

void TestMyDict()
{
    std::cout << "mContainer Dictionary:\n";
    mTimer timer;
    Dictionary<int, std::string> myDict;

    for (int i = 0; i < 1000000; i++)
        myDict.emplace(i, std::to_string(i));

    std::cout << "Add Time:\t" + std::to_string(timer.elapsedMillis()) << std::endl;

    timer.reset();

    size_t count = 0;
    for (auto& kv : myDict)
        count++;

    std::cout << "Iteration Time:\t" + std::to_string(timer.elapsedMillis()) << std::endl;
}

int main()
{
    mLog::Init();

    TestMyDict();

    std::cout << std::endl;

    TestStdDict();
}