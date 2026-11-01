
#pragma once

// must be called once before using predicates
void exactinit();

// 2D orientation test
double orient2d(const double* pa, const double* pb, const double* pc);

// 3D orientation test
double orient3d(const double* pa, const double* pb, const double* pc,
                const double* pd);

// incircle test
double incircle(const double* pa, const double* pb, const double* pc,
                const double* pd);

// insphere test
double insphere(const double* pa, const double* pb, const double* pc,
                const double* pd, const double* pe);
