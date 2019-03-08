# Implementacion Nested Cages

Implementacion del proyecto en c++:

[Nested Cages] (http://www.cs.columbia.edu/cg/nested-cages/)  
ACM Transactions on Graphics, vol. 34, no. 6 (SIGGRAPH Asia 2015).  
Leonardo Sacht, Etienne Vouga and Alec Jacobson

> Iniciamos con :
>
```bash
git clone --recursive https://github.com
```

[![Ejemplo de mallas anidadas "Nested Cages"](http://www.cs.columbia.edu/cg/nested-cages/bunny-shelf-teaser.jpg)](http://www.cs.columbia.edu/cg/nested-cages/)

## Compilacion

Para compilar este proyecto primero debemos instalar algunas dependencias y librerias como :

 1. _**Install CGAL**_. descargamos del repositorio oficial y lo guardamos en la carpeta home, donde tambien estara 
 localizado nuestro proyecto, asi que luego ejecutaremos y llamaremos a esta libreria(mas abajo decimos como compilar).


```bash
cd /home/yourUser/NCGrafica
mkdir build
cd build
cgal_create_CMakeLists -s executable (opcional)
cmake -DCGAL_DIR=$HOME/CGAL-4.13 . 
cmake -DCMAKE_BUILD_TYPE=Release  .. (si no funciona el de arriba)
make
```

Esto de arriba va crear todas la dependencias y crear el ejecutable`nested_cages`. 

### Dependencias

todas las dependencias son incluidas Excepto CGAL , que tiene que incluir como libreria externa

## Ejecucion

Primero hacemos `cd buid` donde esta el ejecutable `./nested_cages` y para ejecutar hay que tener encuenta que 
primero necesitamos un file mesh de input ya sea .obj .off, etc... y tendremos que generar dos files .obj que seran
las mallas anidadas mas cercanas osea por ejemplo para `../gargo.off`: 

```bash
./nested_cages ../gargo.off 2 1000r 500r None Volume ../test
```
con esto obtenemos de salida dos volumenes regulares `../test_1.off` y `../test_2.off` con 1000 faces  y el otro con  with 500 faces respectivamente.

Del mismo modo a la decimacion adaptativa para `../gargo.off` hacemos la minimizacion de superfice ARAP con energia, y haciendo uso con entrada decimaciones `../gargo_1000.off` and `gargo_500.off`, teniendo finalmente como salida test_1.obj y test_2.obj como mallas anidadas para test_0.obj.

```bash
./nested_cages ../gargo.off 2 ../gargo_1000.off ../gargo_500.off SurfARAP None ../test
```
descripcion de compilacion es:
`./nested_cages` : es el binario del programa principal ,`input.off`: es el archivo de entrada(.off, .obj,.ply, .wrl, .mesh), `q`:  es el orden de cuadratura para el flow shrink, `L(1),L(2),L(3)...`: numero de cada para cada cage, `surfArap`: expansion de la energia a minimizar para el reinflado(None, DispStep, DispInitial, Volume, SurfARAP, VolARAP ), `Ǹone` : energia final para ser minimizada despues del re-inflado. `../test`: cages seran guardados como output_1.obj, output_2.obj, ..., output_k.obj.
