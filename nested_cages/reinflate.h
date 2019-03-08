#ifndef REINFLATE_H
#define REINFLATE_H

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <stack>

// Dato un history  de  mallas finas fluidas H, reinflamos la malla fina
// empujando la malla gruesa (C_hat,F_hat) lejos a nuevas posiciones (C,F_hat).
// 
// Inputs:
//   H  history  de posicion de mallas fluidas 
//   T  conectividad de la malla fina ( igual para todas las mallas en H)
//   C_hat  #C_hat x 3 lista de posiciones iniciales de malla gruesa
//   F_hat  #F_hat x 3 lista de índices de triángulo de malla gruesa C_hat
//   EnergyInflation  ya sea "None", o "DispInitial", o "DispStep" o
//           "Volume" o "SurfARAP" o "VolARAP"
//   EnergyFinal  pueder ser: "None", o "DispInitial", o "DispStep" o
//           "Volume" o "SurfARAP" o "VolARAP"
// Output:
//  C  #C by 3 Lista de posiciones finales de vértices de malla gruesa (jaula óptima)
void reinflate(
  std::stack<Eigen::MatrixXd> & H, 
  const Eigen::MatrixXi & T, 
  const Eigen::MatrixXd & C_hat, 
  const Eigen::MatrixXi & F_hat, 
  const char* EnergyInflation,
  const char* EnergyFinal,
  Eigen::MatrixXd & C);

#endif 