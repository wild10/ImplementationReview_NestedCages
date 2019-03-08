#include "io.h"

#include <iostream>
#include <set>
#include <algorithm>
#include <cstdlib>

// Eliminar los caracteres 'c' del string 'str'.
// Tambien   devuelve el numero de caracteres eliminado
int eliminar_allchars_cantidad( char* str,  const char c) {
  
  int removed = 0;
  char *pr = str, *pw = str;

  while (*pr) {
    *pw = *pr++;
    removed += (*pw == c);
    pw += (*pw != c);
  }

  *pw = '\0';
  return removed;
}

/*
* check si un string dado es un numero
*/

bool es_int(const char *str){

  while (*str)
    if (!isdigit(*str++))
      return false;
  return true;

}