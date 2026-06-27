/*
Karen Nanamy Kamo - NUSP: 15495932 
Rebeca de Oliveira Silva - NUSP: 11963923
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ler_arq.h"
#include "escrever_arq.h"
#include "structs.h"
#include "funcionalidades.h"
#include "uteis.h"


int main(){
  int opcao;
  scanf("%d", &opcao);

  switch (opcao){
    case 1:
      criar_arq_bin();
      break;

    case 2:
      buscar_todos_reg_bin();
      break;
    
    case 3:
      buscar_reg_filtro();
      break;

    case 4:
      buscar_reg_RRN();
      break;

    case 5:
      criar_indice();
      break;

    case 6:
      busca_indexada();
      break;

    case 7:
      remover_registro();
      break;

    case 8:
      inserir_registro();
      break;
    
    case 9:
      atualizar_registro();
      break;

    case 10:
      funcionalidade10();
      break;

    default:
      printf("Opção inválida.\n");
  }
 

  return 0;
}