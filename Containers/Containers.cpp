// Containers.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include "LabVec.h"

int main()
{
    Labyrinth::Vector<int> a;

    a.emplace_back(5);
    a.emplace_back(4);
    a.emplace_back(3);
    a.emplace_back(6);

    for (auto i : a)
    {
        std::cout << i << " ";
    }

    std::cout << std::endl;

    a.resize(8);

    for (auto i : a)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cin.get();
}