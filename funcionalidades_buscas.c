/*
Karen Nanamy Kamo - NUSP: 15495932 
Rebeca de Oliveira Silva - NUSP: 11963923
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "structs.h"
#include "ler_arq.h"
#include "escrever_arq.h"
#include "funcionalidades.h"
#include "uteis.h"

/* ================================================================================

******************************FUNCIONALIDADE 2************************************

===================================================================================*/ 

/* Funcionalidades: 

  1. Abrir o arquivo.bin para leitura
  2. Ler o arquivo até o final, registro por registro (Linha por linha)
  3. Caso o registro exista, deve imprimir campo a campo

*/

void buscar_todos_reg_bin(){
  char nomeArqBin[100];
  scanf("%s", nomeArqBin); // leitura do input do usuário

  FILE *arqBin = NULL; // inicializa o ponteiro
  RegistroCabecalho *h = abrir_e_validar_arq_bin(nomeArqBin, &arqBin, "rb");
  if (h == NULL) return; // se der errado, só para

  //Enquanto não terminar o arquivo.bin, continue lendo os valores dos registros de dados
  while (check_eof(arqBin)){
    RegistroDado *r = ler_reg_dado_bin(arqBin);

    if (r == NULL){ // se algo deu errado
      break;
    }

    // verificando se o registro está logicamente removido
    // apenas ignora o removido e continua
    if (r->removido == '1'){
      free_reg_dado(r);
      free(r);
      continue;
    } 
    
    imprimir_reg_dado(r);
    
    free_reg_dado(r);
    free(r);
  }

  free_reg_cab(h);
  fclose(arqBin);
}

//===============================================================================================================================//


// Função para buscar o Registro de Cabeçalho
//Não precisava para o trabalho 1
//Usado para debugar o código pelas estudantes 
void buscar_reg_cab_bin(){
  char nomeArqBin[100];
  scanf("%s", nomeArqBin);

  FILE *arqBin = NULL; // inicializa o ponteiro
  RegistroCabecalho *h = abrir_e_validar_arq_bin(nomeArqBin, &arqBin, "rb");
  if (h == NULL) return; // se der errado, só para

  // mover cursor para o início do arquivo
  fseek(arqBin, 0, SEEK_SET);

  imprimir_reg_cab(h);

  free_reg_cab(h);
  fclose(arqBin);
}

/* ================================================================================

******************************FUNCIONALIDADE 3************************************

===================================================================================*/ 

/* Funcionalidades: 

  1. Abrir o arquivo.bin para leitura
  2. Quantidade de buscas pelo usuário
  3. Quantidade de pares de busca pelo usuário 
  4. Entrada dos valores desejadas pelo usuário
  5. Compara os valores fornecido pelo usuário com os resgistros de dados
  6. Imprime, caso exista
*/
void buscar_reg_filtro(){
  char nomeArqBin[100];
  int quantBusca; 
  int quantPar; // armazena quantos pares o usuário quer
  
  // arrays para guardar os dados que devem ser filtrados
  char nomesCampos[100][500];
  char valoresCampos[100][500];

  scanf("%s %d", nomeArqBin, &quantBusca); // leitura dos inputs do usuário

  FILE *arqBin = NULL; // inicializa o ponteiro
  RegistroCabecalho *h = abrir_e_validar_arq_bin(nomeArqBin, &arqBin, "rb");
  if (h == NULL) return; // se der errado, só para

  // loop para buscar de acordo com a quantidade desejada
  for (int i = 0; i < quantBusca; i++){
    scanf("%d", &quantPar); // input de quantos pares vai desejar buscar

    int filtroCod = 0; // flag para saber se codEstacao é um filtro
    int codBuscado = -1; // guardar o codEstacao buscado

    // loop para a quantidade de pares desejadas
    for (int j = 0; j < quantPar; j++){
      scanf("%s", nomesCampos[j]); // lê o nome do campo desajado

      if (strcmp(nomesCampos[j], "nomeEstacao") == 0 ||
          strcmp(nomesCampos[j], "nomeLinha") == 0
      ) ScanQuoteString(valoresCampos[j]); // se for string, para ler "" 
      else scanf("%s", valoresCampos[j]);  // se for int, lê normalmente

      if (strcmp(nomesCampos[j], "codEstacao") == 0){
        filtroCod = 1; // se um dos filtros é pelo codEstacao
        codBuscado = verificar_nulo(valoresCampos[j]); // guardando o codEstacao buscado
      }
    }

    fseek(arqBin, 17, SEEK_SET); // mover para o byte 17 para buscar nos dados

    int naoEhInexistente = 0; // para ver se o registro foi printado ou não

    // fazer busca até o final do arquivo
    while (check_eof(arqBin)){
      RegistroDado *r = ler_reg_dado_bin(arqBin);

      if (r == NULL) break; // se algo deu errado

      // verificando se o registro está logicamente removido
      // se for removido, apenas ignora e continua
      if (r->removido == '1'){
        free_reg_dado(r);
        free(r);
        continue;
      }
      
      int achou = verificacao_filtros(r, nomesCampos, valoresCampos, quantPar);

      if (achou){
        imprimir_reg_dado(r);
        naoEhInexistente = 1;
      }

      // para verificar se a busca por cod deu certo
      int pararBusca = (filtroCod && r->codEstacao == codBuscado);

      free_reg_dado(r);
      free(r);

      if (pararBusca) break; // se foi encontrado pelo codEstacao e se é igual ao buscado, para
    }

    if (!naoEhInexistente) printf("Registro inexistente.\n");

    // pular linha entre as buscas, menos no final
    if (i < quantBusca - 1) printf("\n");
  }
  
  free_reg_cab(h);
  fclose(arqBin);
}



/* ================================================================================

******************************FUNCIONALIDADE 4************************************

===================================================================================*/ 

/* Funcionalidades: 

  1.Abrir o arquivo.bin para leitura
  2. Calcula o byteoffset apartir do RRN de entrada
  3. Move o ponteiro para a posição desejada
  4. Tratamento de casos de registros removidos

*/
void buscar_reg_RRN(){
  char nomeArqBin[100];
  int rrn;

  scanf("%s %d", nomeArqBin, &rrn); // entrada dos dados desejados

  FILE *arqBin = NULL; // inicializa o ponteiro
  RegistroCabecalho *h = abrir_e_validar_arq_bin(nomeArqBin, &arqBin, "rb");
  if (h == NULL) return; // se der errado, só para
  
  // verificando se esse RRN existe nos registros
  if (rrn >= h->proxRRN || rrn < 0) {
    printf("Registro inexistente.\n");
    fclose(arqBin);
    return;
  }

  // calculando o byte que deve ser buscado
  int byteOffSet = 80 * rrn + 17;
  fseek(arqBin, byteOffSet, SEEK_SET);

  RegistroDado *r = ler_reg_dado_bin(arqBin);

  // verificando se o registro está logicamente removido
  if (r == NULL || r->removido == '1') printf("Registro inexistente.\n");
  else imprimir_reg_dado(r);

  free_reg_dado(r);
  free(r);
  free_reg_cab(h);

  fclose(arqBin);
}


/* ================================================================================

******************************FUNCIONALIDADE 6************************************

===================================================================================*/ 

/* Funcionalidades: 

  1. Abrir o arquivo de dados para leitura e status
  2. Carrega o índice para a RAM
  3. Quantidade de buscas pelo usuário
  4. Chama executar_busca_indexada 
    4.1 Quantidade de pares de busca pelo usuário 
    4.2 Entrada dos valores desejadas pelo usuário
    4.3 Verifica se uma das buscas é por codEstacao
    4.4 Se for, carrega o índice na RAM e busca pelo índice
    4.5 Se não, faz busca sequencial
  5. Imprime, caso exista
*/
void busca_indexada(){
  char nomeArqBin[100];
  char nomeArqInd[100];
  int quantBusca; 

  scanf("%s %s %d", nomeArqBin, nomeArqInd, &quantBusca); // leitura dos inputs do usuário

  // 1. Abrir o arquivo de dados para leitura e status
  FILE *arqBin = NULL; // inicializa o ponteiro
  RegistroCabecalho *h = abrir_e_validar_arq_bin(nomeArqBin, &arqBin, "rb");
  if (h == NULL) return; // se der errado, só para

  // 2. Carrega o índice para a RAM
  int nRegistrosIndice = 0; // para guardar quant de regs
  RegistroDadoIndice *listaIndice = carregamento(nomeArqInd, &nRegistrosIndice);

  if (listaIndice == NULL){
    free_reg_cab(h);
    fclose(arqBin);
    return; // quer dizer que não tem os regs 
  }

  // 3. Quantidade de buscas pelo usuário
  // loop para buscar de acordo com a quantidade desejada
  for (int i = 0; i < quantBusca; i++){
    int posSequencial = 17; // pula o registro de cabeçalho
    int flagEncontrou = 0;
    RegistroDado *rEncontrado = NULL; // ponteiro para receber o resultado

    // enquanto existir um rrn válido (p/ buscas com vários resultados)
    while ((rEncontrado = executar_busca_indexada(listaIndice, nRegistrosIndice, h, arqBin, &posSequencial)) != NULL){
      flagEncontrou = 1;

      // 5. Imprime, caso exista
      imprimir_reg_dado(rEncontrado);
      free_reg_dado(rEncontrado);
      free(rEncontrado);
      

      // se busca por codEstacao, não precisa continuar
      if (posSequencial == 17) break;
    }

    // se não encontrou ou está logicamente removido
    if (!flagEncontrou) printf("Registro inexistente.\n");
    
    if (i < quantBusca - 1) printf("\n"); // pula linha
  }

  // dar free e close
  free(listaIndice);
  free(h);
  fclose(arqBin);
}


RegistroDado *executar_busca_indexada(RegistroDadoIndice *listaIndice, int nRegistrosIndice, RegistroCabecalho *h, FILE *arqBin, int *posSequencial){
  // foi usado static p/ os valores ficarem guardados após fazer várias buscar
  static int quantPar;
  // arrays para guardar os dados que devem ser filtrados
  static char nomesCampos[100][500];
  static char valoresCampos[100][500];
  static int filtroCod; // flag para saber se codEstacao é um filtro
  static int codBuscado; // para guardar o codEstacao buscado

  // só vai ler teclado se for primeira chamada para a busca, se não fica loop infinito
  if (*posSequencial == 17){
    filtroCod = 0;
    codBuscado = -1;

    scanf("%d", &quantPar); // input de quantos pares vai desejar buscar

    // loop para a quantidade de pares desejadas
    for (int j = 0; j < quantPar; j++){

      // entrada dos valores desejadas pelo usuário
      scanf("%s", nomesCampos[j]); // lê o nome do campo desajado

      if (strcmp(nomesCampos[j], "nomeEstacao") == 0 ||
          strcmp(nomesCampos[j], "nomeLinha") == 0
      ) ScanQuoteString(valoresCampos[j]); // se for string, para ler "" 
      else scanf("%s", valoresCampos[j]);  // se for int, lê normalmente

      if (strcmp(nomesCampos[j], "codEstacao") == 0){
        filtroCod = 1; // codEstacao é uma das buscas
        codBuscado = verificar_nulo(valoresCampos[j]); // pega o codEstacao
      }
    }
  }

  // verifica se uma das buscas é por codEstacao
  if (filtroCod && listaIndice != NULL){
    int rrnBuscado = busca_binaria_indice(listaIndice, nRegistrosIndice, codBuscado);

    if (rrnBuscado != -1){ // achou um rrn
      // verificando se esse RRN é válido
      if (rrnBuscado >= h->proxRRN || rrnBuscado < 0) return NULL;

      int byteOffSet = 80 * rrnBuscado + 17; // calculando o byte que deve ser buscado
      fseek(arqBin, byteOffSet, SEEK_SET);

      RegistroDado *r = ler_reg_dado_bin(arqBin);
      if (r == NULL) return NULL; // se algo deu errado
      
      // verificando se o registro está logicamente removido
      if (r->removido == '1'){
        free_reg_dado(r);
        free(r);
        return NULL;
      } 
      
      // verificando nos outros campos buscados
      if (verificacao_filtros(r, nomesCampos, valoresCampos, quantPar)){
        filtroCod = 0; // p/ próxima chamada, já saber que já foi
        return r;
      }

      free_reg_dado(r);
      free(r);
    } 
    return NULL; // não achou registro
  } 
  
  else { // se não, faz busca sequencial
    // mover para o byte de onde parou
    fseek(arqBin, *posSequencial, SEEK_SET);

    // fazer busca até o final do arquivo
    while (check_eof(arqBin)){
      RegistroDado *r = ler_reg_dado_bin(arqBin);
      if (r == NULL) break; // caso falte memória

      // verificando se o registro está logicamente removido
      // se for removido, apenas ignora e continua
      if (r->removido == '1'){
        free_reg_dado(r);
        free(r);
        continue;
      }
      
      if(verificacao_filtros(r, nomesCampos, valoresCampos, quantPar)){
        *posSequencial = ftell(arqBin); // atualiza a pos p/ salvar onde parou
        return r;
      }

      free_reg_dado(r);
      free(r);
    }

    return NULL; // se não achou
  }
}