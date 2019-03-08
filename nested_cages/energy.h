#ifndef ENERGY_H
#define ENERGY_H

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <igl/arap.h>

// Esta clase implementa funciones de energia 
//   - energy_displacement
//   - energy_surface_arap
//   - energy_volumetric_arap
//   - energy_volume
//   - energy

// calclua la norma cuadratica de la diferencia entre 2 mallas C y C_hat
//
// Inputs:
//   C  #C x 3 lista de posiciones de vertices la primera malla
//   C_hat  #C x 3 lista de posiciones de vertices la segunda malla
// Output:
//   energy   ((C-C_hat).rowwise().norm()).norm();
double energy_displacement(
  const Eigen::MatrixXi & C, 
  const Eigen::MatrixXd & C_hat);

// Dado un rest-pose (superfice) malla (V,F) calcula las energias ARAP  ('spokes-and-rims')
// de la deformacion de la malla (U,F)
//
// Inputs:
//   V  #V x 3 lista de posiciones by 3 list of vertex positions of the rest pose mesh mesh
//   F  #F by 3 list of vertex indices into V
//   U  #V by 3 list of vertex positions of the deforming mesh
// Output:
//   energy   Surface ARAP energy
double energy_surface_arap(
  const Eigen::MatrixXd & V, 
  const Eigen::MatrixXi & F,
  const Eigen::MatrixXd & U);

// Given rest-pose (tetrahedral) mesh (V,F) calculates the ARAP energy ('elements')
// of a deformed mesh (U,F)
//
// Inputs:
//   V  #V by 3 list of vertex positions of the rest pose mesh mesh
//   F  #F by 4 list of vertex indices into V
//   U  #V by 3 list of vertex positions of the deforming mesh
// Output:
//   energy   Volumetric ARAP energy
double energy_volumetric_arap(
  const Eigen::MatrixXd & V, 
  const Eigen::MatrixXi & F,
  const Eigen::MatrixXd & U);

// Calculates the volume of a mesh (V,F)
//
// Inputs:
//   V  #V by 3 list of vertex positions of the rest pose mesh mesh
//   F  #F by 3 list of vertex indices into V
// Output:
//   energy   Volume of (V,F)
double energy_volume(
  const Eigen::MatrixXd & V, 
  const Eigen::MatrixXi & F);

// Choose between energies above depending on char* Energy and returns 
// energy value
//
// Inputs:
//   C  #C by 3 list of vertex positions of current coarse mesh
//   C_hat  #C by 3 list of vertex positions of initial coarse mesh
//   C_hat  #C by 3 list of vertex positions of previous coarse mesh
//   F  #F by 3 list of vertex indices into V
//   Energy  char specifying the energy to be calculated
// Output:
//   energy value
double energy(
  const Eigen::MatrixXd & C, 
  const Eigen::MatrixXd & C_hat, 
  const Eigen::MatrixXd & C_prev, 
  const Eigen::MatrixXi & F,
  const char* Energy);

#endif 