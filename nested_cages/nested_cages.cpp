// libreria externa mesh fix
#define MESHFIX_WITH_EIGEN
#include <meshfix.h>

// nuestros headers basico
#include "io.h"
#include "cgal.h"
#include "flow.h"
#include "reinflate.h"

// librerias de libigl 
#include <igl/doublearea.h>
#include <igl/writeOBJ.h>
#include <igl/read_triangle_mesh.h>
#include <igl/copyleft/cgal/remesh_self_intersections.h>
#include <igl/copyleft/cgal/polyhedron_to_mesh.h>
#include <igl/copyleft/cgal/mesh_to_polyhedron.h>
#include <igl/copyleft/cgal/intersect_other.h>

using namespace Eigen;
using namespace igl;
using namespace std;



int main(int argc, char * argv[])
{

  using namespace igl::copyleft::cgal;


  // numero de capas
  int k = argc-6; // 8-6

  cout << "numero de capas = " << k << endl;


  // orden de cuadratura
  int quad_order = atoi(argv[2]); // 2

  // leer input mesh
  Surface_mesh M;
  MatrixXd V0;
  MatrixXi F0;

  if (!read_triangle_mesh(argv[1],V0,F0)){
    cout << "no se puede leer el archivo"  << endl;
    return 0;
  }

  // convertir al formato CGAL
  mesh_to_polyhedron(V0,F0,M); // vertices, faces y caras geometricas

  // mesh output input como nivel de output_0.off
  char* filename;
  char *suffix;
  if (asprintf(&filename, "%s%s", argv[argc-1], "_0.obj")!=-1){ // argv[argc-1] = test
    writeOBJ(filename,V0,F0);
  } else {
    cout << " no se puede reservar espacio de memoria para el output file"  << endl;
    return 0;
  }

  // la primera malla fina es la entrada
  MatrixXd V = V0;
  MatrixXi F = F0;

  // vector donde cada entrada es el numero de faces para cada  nivel j
  int L[k];
  //  decimacion standar CGAL adaptativo
  bool adaptativo = true;

  // declaramos la decimacion de entrada
  Surface_mesh M_hat;
  MatrixXd V_coarse;
  MatrixXi F_coarse;

/* =============================== LOOP PRINCIPAL PARA EL NESTED CAGE =================================== */
  // bucle sobre todos los niveles (lista de meshes de resolucion)
  for(int i = 0; i < k; i++){
    // check if argv[i+3]  es un arvhivo valido. Si es, usaremos como entrada de decimacion
    std::ifstream is_file(argv[i+3]); // ../gargo_1000.off
    if (is_file)
    {
      if (!read_triangle_mesh(argv[i+3],V_coarse,F_coarse)){
        cout << "error:  input decimacion es un archivo existente, pero no puede ser leido"  << endl;
        return 0;
      }
      // convertir al formato CGAL
      mesh_to_polyhedron(V_coarse,F_coarse,M_hat); //  Decimacion
    }

    // sino, la entrada decimacion es calculada por decimacion de la capa anterior
    // con decimacion CGAL(regular  o adaptativa)
    else
    {
      // primero hacemos check si el ultimo charcater de argv[i+2] es r. si es, entonces, eliminamos 'r'  y  adaptativo = false
      adaptativo = eliminar_allchars_cantidad(argv[i+3], 'r')==0;
      // check si argv[i+2] es un entero valido ( lanzamos un erro si no es)
      if (!es_int(argv[i+3])){
        cout << "tienes que pasarle un entero valido / input decimacion validos"  << endl;
        cout << "el valor que tu pasaste es " << argv[i+3] << endl;
        return 0;
      }
      // numero especificado de caras para el cage
      L[i] = atoi(argv[i+3]);
      // valor a pasar para el decimador
      float ratio = (1.*L[i])/(1.*M.size_of_facets());
      // el calculo anterior va ser decimado
      M_hat = M;
      // decimar ratio= porcentaje de decimacion  
      decimate_CGAL(&M_hat,ratio,adaptativo); // DECIMACION
    }

    // Convertir decimacion a formato LibIGL/Eigen
    polyhedron_to_mesh(M_hat,V_coarse,F_coarse);

    // Parámetros para llamar a la función para verificar las auto-intersecciones de la decimación
    RemeshSelfIntersectionsParam params;
    params.detect_only = true;
    params.first_only = true;
    MatrixXd tempV;
    MatrixXi tempF;
    MatrixXi IF;
    VectorXi J;
    VectorXi IM;

    // check si hay interseccion con CGAL
    remesh_self_intersections(V_coarse,F_coarse,params,tempV,tempF,IF,J,IM); 
    // si el input coarse mesh se interseca a si mismo, eliminar le interseccion con Meshfix (to-do)

    /* ----------------------------  -----------------------------*/
    if ( IF.rows() > 0 ){  // Intersection Face > 0

      cout << "Puliendo interseccion M" << i+1 << " ..." << endl;
      meshfix(MatrixXd(V_coarse),MatrixXi(F_coarse),V_coarse,F_coarse); // meshfix
      cout << "muy bien!" << endl;

      remesh_self_intersections(V_coarse,F_coarse,params,tempV,tempF,IF,J,IM);
      if (IF.rows()==0)
      {
          cout << "Meshfix  exitoso eliminar auto-inteersecciones" << endl;
      }
      else{
          cout << "Meshfix no fue posible eliminar toda la autointerseccion. salir..." << endl;
    	  return 0;
      }
    }

  /* =========================== FLOW SHRINK ===========================*/
	  
	  VectorXd area_0;
	  doublearea(V,F,area_0);  // calcular area_0 para la integral de area de triangulo t
	  area_0 = 0.5*area_0;   // inicialzamos la gradiente en cero

    // matriz precalculada que convierte gradientes en puntos cuadratiicos a gradientes en los vertices de malla
  	SparseMatrix<double> A_qv;
    gradQ_to_gradV(V, F, area_0, quad_order, A_qv); // GRADIENTE
  	// Flujo M dentro de M_hat  y guardar el resultado en un stack H de flujo de meshes
    stack<MatrixXd> H; // H stack meshes
    cout << "Fluyendo M" << i << " dentro de M" << i+1 << "..." << endl;
    if (!flow_fine_inside_coarse(V,F,V_coarse,F_coarse,A_qv,H)) // FLUJO DE F(V,F) Y C(V_coarser, F_coarser) H = history de shrink mesh fina
    {
      cout << "fallo de Flow al tomar la malla fina dentro  del coarse mesh despues de 1000 iteraciones. salir..." << endl;
      return 0;
    } 
    cout << "muy bien!" << endl;
/* =========================== REINFLATE MESH ===========================*/
    // Reinflar el output a cage hasta C
    MatrixXd C;
    cout << "Reinflando M" << i << ", pushing M" << i+1 << "..." << endl;
    reinflate(H,F,V_coarse,F_coarse,argv[argc-3],argv[argc-2],C); // REINFLADO CON H 
    cout << "muy bien!" << endl;

    // check : cage deberia nunca ser auto intersectado en este estado
    remesh_self_intersections(C,F_coarse,params,tempV,tempF,IF,J,IM);
    if (IF.rows()>0){
      cout << i+1 << "-th output cage self-intersects. ERROR! salir...  " << endl;
      return 0;
    }

    // sanity check: cage should never intersect input decimation
    intersect_other(C,F_coarse,V,F,true,IF);
    if (IF.rows()>0){
      cout << i+1 << "-th output cage intersecta al anterior cage. ERROR! Salir...  " << endl;
      return 0;
    }

    // output cage es el input para el siguiente nivel
    M.clear();
    mesh_to_polyhedron(C,F_coarse,M);
    V = C;
    F = F_coarse;

    // Output cage to file output_i.obj
    if ((asprintf(&suffix,"_%d.obj",i+1)!=-1) && (asprintf(&filename, "%s%s", argv[argc-1], suffix)!=-1)){
      writeOBJ(filename,C,F_coarse);
    } else {
      cout << "no se puede reservar espacio de memoria para el file output "  << endl;
      return 0;
    }
    // volver a la decimación adaptativa (estándar)
    adaptativo = true;
  }

  free(filename);
  return 1;
}
