// mContainers.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "mpch.h"

#include "mContainers.h"
#include "OldDict.h"

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

#define ITERATIONS 10000

using namespace mContainers;

void TestStdDict()
{
    std::cout << "STD Unordered Map:\n";
    std::unordered_map<int, std::string> myDict;

    mTimer timer;
    for (int i = 0; i < ITERATIONS; i++)
        myDict.emplace(i, std::to_string(i));

    std::cout << "Add " + std::to_string(timer.elapsedMillis()) << std::endl;

    timer.reset();
    uint64_t count = 0;
    for (auto& kv : myDict)
        count++;

    std::cout << "Iteration " + std::to_string(timer.elapsedMillis()) << std::endl;

    timer.reset();
    for (int i = 0; i < ITERATIONS; i++)
        auto a = myDict[i];

    std::cout << "Access " + std::to_string(timer.elapsedMillis()) << std::endl;
}

void TestMyDict()
{
    std::cout << "mContainer Dictionary:\n";
    mDictionary<int, std::string> myDict;

    mTimer timer;
    for (int i = 0; i < ITERATIONS; i++)
        myDict.emplace(i, std::to_string(i));

    std::cout << "Add " + std::to_string(timer.elapsedMillis()) << std::endl;

    timer.reset();
    uint64_t count = 0;
    for (auto& kv : myDict)
        count++;

    std::cout << "Iteration " + std::to_string(timer.elapsedMillis()) << std::endl;

    timer.reset();
    for (int i = 0; i < ITERATIONS; i++)
        auto a = myDict[i];

    std::cout << "Access " + std::to_string(timer.elapsedMillis()) << std::endl;
}

void TestOldDict()
{
    std::cout << "mContainer Old Dictionary:\n";
    OldDictionary<int, std::string> myDict;

    mTimer timer;   
    for (int i = 0; i < ITERATIONS; i++)
        myDict.emplace(i, std::to_string(i));

    std::cout << "Add " + std::to_string(timer.elapsedMillis()) << std::endl;

    timer.reset();
    uint64_t count = 0;
    for (auto& kv : myDict)
        count++;

    std::cout << "Iteration " + std::to_string(timer.elapsedMillis()) << std::endl;

    timer.reset();
    for (int i = 0; i < ITERATIONS; i++)
        auto a = myDict[i];

    std::cout << "Access " + std::to_string(timer.elapsedMillis()) << std::endl;
}

int main()
{
    mLog::Init();

    TestMyDict();

    std::cout << std::endl;

    TestOldDict();
    
    std::cout << std::endl;

    TestStdDict();
}