/*
Karen Nanamy Kamo - NUSP: 15495932 
Rebeca de Oliveira Silva - NUSP: 11963923
*/

#include <stdio.h>
#include <stdlib.h>
#include "structs.h"

// Liberar o Registro de Cabeçalho alocado
void free_reg_cab(RegistroCabecalho* h){
  free(h);
}

// Liberar os campos variáveis do Registro de Dados, alocados com malloc
void free_reg_dado(RegistroDado* r){
  if (r->nomeEstacao != NULL) {
    free(r->nomeEstacao); 
    r->nomeEstacao = NULL;
  }
  if (r->nomeLinha != NULL) {
    free(r->nomeLinha);
    r->nomeLinha = NULL;
  }
}