#include "reinflate.h"
#include "filter.h"
#include "gradient.h"
#include "energy.h"
#include "flow.h"
#include <igl/arap.h>
#include <stdio.h>

// Need to include some IGL header to have igl namespace
#include <igl/signed_distance.h>
#include <igl/copyleft/tetgen/tetrahedralize.h>

void reinflate(
  std::stack<Eigen::MatrixXd> & H, 
  const Eigen::MatrixXi & T, 
  const Eigen::MatrixXd & C_hat, 
  const Eigen::MatrixXi & F_hat, 
  const char* EnergyInflation,
  const char* EnergyFinal,
  Eigen::MatrixXd & C)
{
  using namespace Eigen;
  using namespace std;
  using namespace igl;
  using namespace igl::copyleft::tetgen;

  MatrixXd F = H.top();
  H.pop();
  C = C_hat;
  MatrixXd C_prev = C; // necesario para las energías que dependen del paso anterior (por ejemplo, DispStep)
  int step = 1;
  int total_steps = H.size();

  // data necesaria para volumetric ARAP
  MatrixXd TV;
  MatrixXd TV0;
  MatrixXi TT;
  MatrixXi TF;
  ARAPData data;
  VectorXi b(C.rows());
  for (int k=0;k<C.rows();k++) b(k) = k;

  // reinflacion
  while (!H.empty()){

    // Inicializar energia como infinito.
    double current_energy = std::numeric_limits<double>::infinity();
    // tamaño de paso inicial
    double beta = 1e-2;

    // velocidad de malla fina
    MatrixXd Uf = H.top()-F;
    H.pop();

    // Calcular el diámetro a escala de separación entre las mallas.
    double diam = diameter(F,C);
    
    // Si no hay energía prescrita, encuentre solo un estado factible (al filtrar Uc = 0)
    if (strcmp(EnergyInflation,"None")==0)
    {
    	MatrixXd Uc = MatrixXd::Zero(C_hat.rows(), 3);
      // este eps=5e-3 Se elige para que haya espacio para la optimización final.
      filter(F,T,Uf,C,F_hat,diam*5e-3,Uc);
      // Actualizar la malla fina
      F = F+Uf;
      // Actualizar la malla gruesa con filtro de velocidades
      C = C+Uc;
      #ifdef VERBOSE_DEBUG
        cout << "Paso de reinflado" << step << "/" << total_steps << ": Energy = None (solo estado factible)" << endl;
      #endif
    }
    // Si la energia para re-inflation  es prescrita, minimizela
    else if (strcmp(EnergyInflation,"None")!=0)
    {
      // Si Volumetric ARAP, tetraedrica y  precalculado data ARAP
      if (strcmp(EnergyInflation,"VolARAP")==0)
      {
        string tetgen_flags ("q2Y");
        tetrahedralize(C_hat,F_hat,tetgen_flags,TV0,TT,TF); 
        if (!arap_precomputation(TV0,TT,3,b,data))
        {
          cout << "ARAP  pre calculo fallido" << endl;
        }
        // Inicializar malla tet deformada como inicial
        TV = TV0;
      }
      // Escalonando y proyectando
      while (true)
      {
        #ifdef VERBOSE_DEBUG
          cout << "paso de reinflacion " << step << "/" << total_steps << ": Energy = " << EnergyInflation  << endl;
        #endif
        MatrixXd grad;
        // para ARAP volumétrico, calcule el gradiente después de encontrar el óptimo
        // malla tet deformada (con local-globe solver)
        if (strcmp(EnergyInflation,"VolARAP")==0)
        {
          if (!arap_solve(C,data,TV))
          {
            cout << "fallo ARAP " << endl;
            return;
          }
          if (!gradient(TV,TV0,C_prev,TT,EnergyInflation,grad))
          {
            cout << "ERROR calculando gradiente " << endl;
            return;
          }

        }
        // Para otras energías, solo calcula el gradiente.
        else
        {
          if (!gradient(C,C_hat,C_prev,F_hat,EnergyInflation,grad))
          {
            cout << "ERROR calculando gradiente " << endl;
            return;
          }
        }
        // multiplicar gradiente por tamaño de paso actual
        MatrixXd Uc = -beta*grad;
        // filtrar velocidades de malla gruesa
        filter(F,T,Uf,C,F_hat,diam*1e-3,Uc);

        // actualizar malla fina (tiene que ser hueso aquí, porque
        // De lo contrario, el solucionador de colisiones comprobará las intersecciones.
        // entre la malla gruesa nueva y la malla fina vieja: rara vez se intersecan,
        // pero puede ser un problema)
        F = F+Uf;
        Uf = MatrixXd::Zero(F.rows(), 3);

        // actualizar valor de energia
        double new_energy;
        if (strcmp(EnergyInflation,"VolARAP")==0)
        {
          new_energy = energy(TV,TV0,C_prev,TT,EnergyInflation);
        }
        else
        {
          new_energy = energy(C+Uc,C_hat,C_prev,F_hat,EnergyInflation);
        }

        // Si aumenta la energía, corte el tamaño del escalón a la mitad
        if (new_energy>current_energy)
        {
          beta = 0.5*beta;
            cout << "energía incrementada, disminuyendo beta a " << beta << endl;
          // Si el tamaño del paso es demasiado pequeño, entonces ha convergido. Descanso
          if (beta<1e-3)
          {
              cout << "beta demasiado pequeño. Saliendo del bucle de búsqueda de línea " << endl;
            break;
          }
        }
        // Si la energía disminuye, actualice la malla gruesa, actualice el valor de energía y
        // y incremente beta 10%
        else{
          C = C+Uc;
          current_energy = new_energy;
          beta = 1.1*beta;
            cout << "energia disminuye a" << current_energy << ", aumenta beta a" << beta << endl;
        }
        // Si paso pequeño, entonces it ha convergido. Break
        if (((Uc).rowwise().norm()).maxCoeff()<diam*1e-5) 
        {
            cout << "Max cambio en posicion = " << ((Uc).rowwise().norm()).maxCoeff() << " demasiado pequeño,  " << endl;
          break;
        }
      }
    }

    // Paso de actualización (solo para fines de impresión)
    step = step+1;
    // Actualizar la malla gruesa anterior (actualmente se usa solo para energía DispStep)
    C_prev = C;

  }

  // minimización final (opcional): útil especialmente para minimizar el volumen,
  // cuando tiene poco sentido minimizar el volumen cuando la malla fina no está
  // completamente re-inflado
  if (strcmp(EnergyFinal,"None")!=0)
  {

    // Intialize energy as infinity
    double current_energy = std::numeric_limits<double>::infinity();
    // Tamaño de paso inicial
    double beta = 1e-2;

    // calcular el diametro para escalar separacion entre mallas
    double diam = diameter(F,C);

    // En este momento la fina malla vuelve a su original.
    // Posiciones (de entrada), por lo que su velocidad debe ser cero
    MatrixXd Uf = MatrixXd::Zero(F.rows(), 3);

    // Si el ARAP volumétrico, tetraédrica y precomputa los datos de ARAP
    if (strcmp(EnergyFinal,"VolARAP")==0){
      string tetgen_flags ("q2Y");
      tetrahedralize(C_hat,F_hat,tetgen_flags,TV0,TT,TF); 
      arap_precomputation(TV0,TT,3,b,data);
      TV = TV0;
    }
    // Escalonando y proyectando
    while (true)
      {
        // #ifdef VERBOSE_DEBUG
          cout << "Optimización final. Energía = " << EnergyFinal  << endl;
        // #endif
        MatrixXd grad;
        // para ARAP volumétrico, calcule el gradiente después de encontrar el óptimo
        // malla tet deformada (con local-globe solver)
        if (strcmp(EnergyFinal,"VolARAP")==0)
        {
          if (!arap_solve(C,data,TV))
          {
            cout << "fallo ARAP " << endl;
            return;
          }
          if (!gradient(TV,TV0,C_prev,TT,EnergyFinal,grad))
          {
            cout << "ERROR calculando el gradiente" << endl;
            return;
          }
        }
        // Para otras energías, solo calcula el gradiente.
        else
        {
          if (!gradient(C,C_hat,C_prev,F_hat,EnergyFinal,grad))
          {
            cout << "ERROR calculando el gradiente " << endl;
            return;
          }
        }
        // multiplicar gradiente por tamaño de paso actual
        MatrixXd Uc = -beta*grad;
        // filtrar velocidades de malla gruesa
        // esto eps=5e-4 Se elige para que haya espacio para la optimización final.
        filter(F,T,Uf,C,F_hat,diam*5e-4,Uc); 

        // update valor de energias
        double new_energy;
        if (strcmp(EnergyFinal,"VolARAP")==0)
        {
          new_energy = energy(TV,TV0,C_prev,TT,EnergyFinal);
        }
        else
        {
          new_energy = energy(C+Uc,C_hat,C_prev,F_hat,EnergyFinal);
        }
        // Si aumenta la energía, corte el tamaño del escalón a la mitad
        if (new_energy>current_energy)
        {
          beta = 0.5*beta;
            cout << "energy incrementada, disminuir beta a " << beta << endl;
          // Si el tamaño del paso es demasiado pequeño, entonces ha convergido. break
          if (beta<1e-3)
          {
              cout << "beta demasiado pequeño. sacar line de busqueda " << endl;
            break;
          }
        }
        // Si la energía disminuye, actualice la malla gruesa, actualice el valor de energía y
        // y incrementar beta 10%
        else{
          C = C+Uc;
          current_energy = new_energy;
          beta = 1.1*beta;
            cout << "energy disminuida a " << current_energy << ", incrementar beta a " << beta << endl;
        }
        // Si paso pequeño, entonces ha convergido. Break
        if (((Uc).rowwise().norm()).norm()<diam*1e-5) break;
      }

  }

  return;

}