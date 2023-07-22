

#include <ctime>
#include <random>
#include <iostream>
#include "utils.hpp"

double RandExp(int random , double l)
{    
    double U = 1.0/random;

    return (-1)*l*(log(U));
}

bool calculate_outcome(const float probability)
{
    bool outcome;
    return outcome = (rand()% 100) < (probability*100);
}