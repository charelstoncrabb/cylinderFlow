#include "Includes.hpp"

#ifndef _VECTOPS_HPP_
#define _VECTOPS_HPP_

std::vector<double> operator+(std::vector<double> lhs, std::vector<double> rhs);

std::vector<double> operator-(std::vector<double> lhs, std::vector<double> rhs);

double dot(std::vector<double> u, std::vector<double> v);

double norm(std::vector<double> u, double p = 2);

std::vector<double> scale(std::vector<double> u, double c);

std::vector<double> vectPow(std::vector<double> u, double p);

template<class T>
std::vector<T> range(std::vector<T> u, int min, int max);

template<class T>
std::vector<T> subind(std::vector<T> u, std::vector<int> inds);

#endif