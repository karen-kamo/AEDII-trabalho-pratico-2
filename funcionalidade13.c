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

void contar_ciclos(Grafo *g, int indAtual, int indOrigem, int *visitado, int comprimento, int *totalCiclos){
  // percorre as arestas do vértice atual
  Aresta *a = g->vertices[indAtual].inicioLista;
  while (a != NULL){
    int v = buscar_indice_vertice(g, a->nomeProxEst);

    // se não encontrou, vai p/ próxima aresta
    if (v == -1){
      a = a->prox;
      continue;
    }

    if (v == indOrigem){
      // só conta como ciclo se já andou pelo menos uma aresta
      // senão a origem voltaria para si mesma
      if (comprimento >= 1) (*totalCiclos)++;
    }

    // se ainda não foi visitado
    else if (!visitado[v]){
      // marca vértice antes de descer na recursão e desmarca depois
      // para que ele possa ser usado em outros ciclos
      visitado[v] = 1; 

      // recursão, sendo a estação atual o v
      contar_ciclos(g, v, indOrigem, visitado, comprimento + 1, totalCiclos);

      visitado[v] = 0; 
    }

    a = a->prox;
  }
}
/* ================================================================================
 
******************************FUNCIONALIDADE 13************************************
 
===================================================================================*/
/* Funcionalidades: 

  1. Abrir arquivo de dados para leitura
  2. Inicialização do Grafo
  3. Contar os ciclos existentes
  4. Impressão da mensagem
*/

void funcionalidade13(){
  char nomeArqBin[100];
  char nomeArqInd[100];
  char nomeCampo[100];
  char valorOrigem[100];

  scanf("%s %s %s", nomeArqBin, nomeArqInd, nomeCampo);
  ScanQuoteString(valorOrigem);

  // 1. Abrir arquivo de dados para leitura
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

  // 2. Inicialização do Grafo
  Grafo g = criar_grafo(arqBin);

  // precisa saber se o de origem existe
  int origemInd = buscar_indice_vertice(&g, valorOrigem);
  if (origemInd == -1){
    printf("Falha na execução da funcionalidade.\n");
    free_grafo(g);
    free(h);
    fclose(arqBin);
    return;
  }

  // vetor para controlar quem está no caminho atual do ciclo
  int *visitado = calloc(g.nroVertices, sizeof(int));
  int totalCiclos = 0;
  
  // inicia a busca a partir da origem
  visitado[origemInd] = 1;

  // 3. Contar os ciclos existentes
  contar_ciclos(&g, origemInd, origemInd, visitado, 0, &totalCiclos);

  // 4. Impressão da mensagem
  if (totalCiclos == 0) printf("Quantidade de ciclos: -1\n");
  else printf("Quantidade de ciclos: %d\n", totalCiclos);

  free(visitado);
  free_grafo(g);
  free(h);
  fclose(arqBin);
}