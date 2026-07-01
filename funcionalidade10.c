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

Grafo criar_grafo(FILE *arqBin){
  // Criar e inicializar o grafo
  Grafo g;
  g.nroVertices = 0;
  g.vertices = NULL;

  fseek(arqBin, 17, SEEK_SET); // pula o cabeçalho

  // Ler todo o arquivo de dados para guardar os vértices
  while (check_eof(arqBin)){
    RegistroDado *r = ler_reg_dado_bin(arqBin);
    if (r == NULL) break; // se não conseguiu ler

    // verifica se é logicamente removido
    if (r->removido == '1') {
      free_reg_dado(r);
      free(r);
      continue;
    }

    // guardar novo nomeEstacao
    int flagExiste = 0; // p/ ver se tem mesmo nome

    // percorre todos os vertices que existem
    for (int i = 0; i < g.nroVertices; i++){
      if (strcmp(g.vertices[i].nomeEstacao, r->nomeEstacao) == 0){
        flagExiste = 1; // se o nome bater, quer dizer que já tem
        break;
      }
    }

    // se não ativou flag, é um novo nome
    if (!flagExiste){
      g.nroVertices++; // incrementa a quant de novos únicos

      // realoca para caber mais um nome
      g.vertices = realloc(g.vertices, g.nroVertices * sizeof(Vertice));

      // salva o nome na lista
      strcpy(g.vertices[g.nroVertices - 1].nomeEstacao, r->nomeEstacao);

      // inicializa a sua lista de caminhos vazia
      g.vertices[g.nroVertices - 1].inicioLista = NULL; 
    }

    free_reg_dado(r);
    free(r);
  }

  // ordena pelo nomeEstacao
  qsort(g.vertices, g.nroVertices, sizeof(Vertice), comparar_vertices);

  // Ler todo o arquivo de dados para guardar as arestas
  fseek(arqBin, 17, SEEK_SET); // pula o cabeçalho

  while (check_eof(arqBin)){
    RegistroDado *r = ler_reg_dado_bin(arqBin);
    if (r == NULL) break; // se não conseguiu ler

    // verifica se é logicamente removido
    if (r->removido == '1') {
      free_reg_dado(r);
      free(r);
      continue;
    }

    // procurando o índice da estação atual no vetor
    int indOrigem = -1; // para guardar o índice
    for (int i = 0; i < g.nroVertices; i++){
      if (strcmp(g.vertices[i].nomeEstacao, r->nomeEstacao) == 0){
        // se o nome bater, achou o índice
        indOrigem = i;
        break;
      }
    }

    // se acharmos o vértice de origem no vetor
    if (indOrigem != -1){

      // se tiver próxima estação, faz ligação
      if (r->codProxEstacao != -1){
        char *nomeProx = buscar_nome_por_codigo(arqBin, r->codProxEstacao);

        // se achou o nome
        if (nomeProx != NULL){
          inserir_aresta_ordenada(&g.vertices[indOrigem], nomeProx, r->distProxEstacao, r->nomeLinha);
          free(nomeProx);
        }
      }

      // se tiver estação de integração, faz a ligação
      if (r->codEstIntegra != -1){
        char *nomeIntegra = buscar_nome_por_codigo(arqBin, r->codEstIntegra);

        // só insere se o nome da integração for diferente da estação atual
        if (nomeIntegra != NULL && strcmp(r->nomeEstacao, nomeIntegra) != 0){
          inserir_aresta_ordenada(&g.vertices[indOrigem], nomeIntegra, 0, "Integração");
          free(nomeIntegra);
        }
      }
    }   

    free_reg_dado(r);
    free(r);
  }

  return g;
}

/* ================================================================================

******************************FUNCIONALIDADE 10************************************

===================================================================================*/ 
/* Funcionalidades: 
  1. Abrir arquivo de dados para leitura
  2. Criar e inicializar o grafo
  3. Ler todo o arquivo de dados para guardar os vértices
  4. Ler todo o arquivo de dados para guardar as arestas
  5. Impressão do grafo
  6. Liberação da memória e close
*/

void funcionalidade10() {
  // 1. Abrir arquivo de dados para leitura
  char nomeArqBin[100];
  char nomeArqInd[100];

  scanf("%s %s", nomeArqBin, nomeArqInd); // leitura dos inputs do usuário

  // abrir arquivo bin para leitura
  FILE *arqBin = fopen(nomeArqBin, "rb");
  if (arqBin == NULL) {
    printf("Falha na execução da funcionalidade.\n");
    return;
  }

  RegistroCabecalho *h = ler_reg_cab_bin(arqBin); // leitura do registro de cabeçalho
  if (h == NULL){ // verifica se a alocação não ocorreu
    printf("Falha na execução da funcionalidade.\n");
    fclose(arqBin);
    return;
  }

  // vendo se é inconsistente 
  if (h->status == '0'){
    printf("Falha na execução da funcionalidade.\n");
    free(h);
    fclose(arqBin);
    return;
  }

  // 2. Criar o grafo
  Grafo g = criar_grafo(arqBin);

  // 3. Impressão do grafo
  for (int i = 0; i < g.nroVertices; i++) {
    // só imprime a linha se a estação tiver caminhos saindo dela
    if (g.vertices[i].inicioLista != NULL) {
      printf("%s,", g.vertices[i].nomeEstacao);
      
      Aresta *aresta = g.vertices[i].inicioLista;
      while (aresta != NULL) {
        printf(" %s, %d, %s", aresta->nomeProxEst, aresta->distancia, aresta->nomesLinha);

        // se ainda tiver uma próxima aresta depois dessa
        if (aresta->prox != NULL) printf(",");

        aresta = aresta->prox;
      }
      printf("\n");
    }
  }

  // 4. Liberação da memória e close
  free_grafo(g);
  free(h);
  fclose(arqBin);
}


