#ifndef FLOW_H
#define FLOW_H


/* *** Esta classe implementa los moludos del algoritmo Nested_cages *** */
//   - gradQ_to_gradV
//   - signed_distance_direction
//   - grad_energy
//   - flow_one_step
//   - flow_fine_inside_coarse

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <stack>

/* Funcion que calcula la matriz que convierte gradiente en cuadratura
* de puntos a gradiente en vertices de la malla
* Inputs:
*  V0  #V0 x 3 lista de posiciones de vertices en la malla
*  F0  #F0 x 3 lista de indices triangulares dentro de V0
*  area_0  #F0 lista de areas de triangulos (precalculada)
*  quad_order  orden de la cuadratura puede ser: 1, 2, or 3
* Output:
*  A   #V0 by #A  matriz con cuadratura de puntos a cuadratura de vertices en la malla
*  para quad_order=1,2,3 then #A=#F,3*#F,4*#F
*/

void gradQ_to_gradV(const Eigen::MatrixXd & V0, const Eigen::MatrixXi & F0, const Eigen::VectorXd & area_0,  const int quad_order, Eigen::SparseMatrix<double> & A);

/* Para un conjunto de puntos P calcula la direccion de decrecimiento de acuerdo al signo
campo de distancia de un mesh (V,F).

Inputs:
  P  #P x 3 lista de puntos
  V  #V x 3 lista de posiciones de vertices de la malla
  F  #F x 3 lista de indices triangulares dentro de V
Outputs:
  D  #P x 3 lista de direcciones
*/
void signed_distance_direction( const Eigen::MatrixXd & P,  const Eigen::MatrixXd & V,  const Eigen::MatrixXi & F,  Eigen::MatrixXd & D);

/*
Inputs:
  V  #V x 3 lista de posiciones de la fine mesh
  F  #F x 3 lista de índices de triángulo de malla fina en V
  V_coarse  #V_coarse x 3 lista de posiciones de la malla gruesa
  F_coarse  #F_coarse by 3 lista de índices de triángulo de malla 
  gruesa en  V_coarse
  A_qv   #V0 by #A_qv Matriz que toma puntos de cuadratura a gradientes en vértices.
  For quad_order=1,2,3 entonces #A_qv=#F,3*#F,4*#F
  M matriz massa de la input fine mesh MatrixXd
Outputs:
  grad  #V by 3  lista de coordenadas de gradiente
  */
void grad_energy( const Eigen::MatrixXd & V,  const Eigen::MatrixXi & F, const Eigen::MatrixXd & V_coarse, const Eigen::MatrixXi & F_coarse, const Eigen::SparseMatrix<double> & A_qv, const Eigen::SparseMatrix<double> & M_inv,  Eigen::MatrixXd & grad);

/*
Calcula el diámetro de la unión de las posiciones de vértice.
dentro V y V_coarse.

Inputs:
  V  #V by 3 lista de posicion de la malla fina
  V_coarse  #V_coarse by 3 lista de posiciones del coarse mesh
Outputs:
  diametro 
*/

double diameter( const Eigen::MatrixXd & V, const Eigen::MatrixXd & V_coarse);

/*
Moverla malla fina que se encoge un paso a lo largo de su flujo dentro de la malla gruesa
using a specified quadrature order.
Inputs:
  V  #V x 3 Posiciones anteriores de posiciones de vértices de malla fina.
  F  #F x 3 Indices de triangulos de la malla fina dentro de V
  V_coarse  #V_coarse x 3 lista de coarse mesh positions
  F_coarse  #F_coarse x 3 lista de índices de triángulo de malla gruesa en
  A   #V0 by #A matrix taking quadrature points to gradients at vertices.
  para quad_order=1,2,3 entonces #A=#F,3*#F,4*#F
  M_inv la inversa de la matriz masa de la malla fina
Output:
 V_new  #V by 3 lista de posiciones de vertices de la nueva malla fina
*/  
void flow_one_step(const Eigen::MatrixXd & V, const Eigen::MatrixXi & F, const Eigen::MatrixXd & V_coarse, const Eigen::MatrixXi & F_coarse, const Eigen::SparseMatrix<double> & A_qv, const double delta_t, 
  Eigen::MatrixXd & V_new);
/*
Fluye la malla fina dentro de la malla gruesa utilizando un orden de cuadratura específico
Inputs:
  V0  #V0 x 3 posicion previa de posicion de vertices de la malla fina
  F0  #F0 x 3 indices triangulares de malla fina dentro V0
  V_coarse  #V_coarse x 3 lista de posiciones de la malla gruesa
  F_coarse  #F_coarse x 3 lista de indices triangulares de la malla gruesa dentro de
  A  #V0 x #A (matriz) tomando puntos cuadraticos a gradientes en vertices
  para quad_order=1,2,3 entonces #A=#F0,3*#F,4*#F0
Output:
  H  es el history de posiciones de flujo de malla
  un boolean, que dice si el flujo falla o es succees
  despues de 1000 iteraciones
*/
bool flow_fine_inside_coarse(const Eigen::MatrixXd & V0, const Eigen::MatrixXi & F0, const Eigen::MatrixXd & V_coarse, const Eigen::MatrixXi & F_coarse, const Eigen::SparseMatrix<double> & A_qv, std::stack<Eigen::MatrixXd> & H);
#endif 
