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
#include "uteis_grafo.h"

Grafo criar_grafo(FILE *arqBin){
  // criar e inicializar o grafo
  Grafo g;
  g.nroVertices = 0;
  g.vertices = NULL;

  fseek(arqBin, 17, SEEK_SET); // pula o cabeçalho

  // ler todo o arquivo de dados para criar os vértices vendo se são nomes únicas
  while (check_eof(arqBin)){
    RegistroDado *r = ler_reg_dado_bin(arqBin);
    if (r == NULL) break; // se não conseguiu ler

    // verifica se é logicamente removido
    if (r->removido == '1') {
      free_reg_dado(r);
      free(r);
      continue;
    }

    // só vai criar um vértice novo se o nome ainda não apareceu
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

  // com os vértices já criados, tem que montar as arestas dos vértices
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

    int indOrigem = buscar_indice_vertice(&g, r->nomeEstacao);

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

  1. Abrir o arquivo.bin para leitura
  2. Criar o grafo
  3. Impressão do grafo
*/

void funcionalidade10() {
  char nomeArqBin[100];
  char nomeArqInd[100];

  scanf("%s %s", nomeArqBin, nomeArqInd);

  // 1. Abrir o arquivo.bin para leitura
  FILE *arqBin = fopen(nomeArqBin, "rb");
  if (arqBin == NULL) {
    printf("Falha na execução da funcionalidade.\n");
    return;
  }

  RegistroCabecalho *h = ler_reg_cab_bin(arqBin); 
  // verifica se a alocação não ocorreu ou é inconsistente
  if (h == NULL || h->status == '0'){ 
    printf("Falha na execução da funcionalidade.\n");
    free_reg_cab(h);
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

  
  free_grafo(g);
  free(h);
  fclose(arqBin);
}




