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
    // índice no vetor do grafo que corresponde ao nome de destino
    int v = buscar_indice_vertice(g, a->nomeProxEst);

    // se não encontrou, vai p/ próxima aresta
    if (v == -1){
      a = a->prox;
      continue;
    }

    // vê se é a origem
    if (v == indOrigem){
      // se o tamanho que percorremos é >= a 1
      if (comprimento >= 1) (*totalCiclos)++;
    }

    // se ainda não foi visitado
    else if (!visitado[v]){
      visitado[v] = 1; // marca vértice antes de entrar na ramificação

      // recursão, sendo a estação atual o v
      contar_ciclos(g, v, indOrigem, visitado, comprimento + 1, totalCiclos);

      visitado[v] = 0; // p/ que possa ser visitada em rotas alternativas
    }

    a = a->prox;
  }
}
/* ================================================================================
 
******************************FUNCIONALIDADE 13************************************
 
===================================================================================*/
/* Funcionalidades: 
  1. leitura dos dados de entrada
  2. abrir arquivo de dados para leitura
  3. inicialização do Grafo
  4. conta os ciclos existentes
  5. impressão da mensagem
  6. liberação da memória e close
*/

void funcionalidade13(){
  char nomeArqBin[100];
  char nomeArqInd[100];
  char nomeCampo[100];
  char valorOrigem[100];

  // 1. leitura dos dados de entrada
  scanf("%s %s %s", nomeArqBin, nomeArqInd, nomeCampo);
  ScanQuoteString(valorOrigem);

  // 2. abrir arquivo de dados para leitura
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

  // 3. inicialização do Grafo
  Grafo g = criar_grafo(arqBin);

  // busca o vértice de origem 
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

  // 4. conta os ciclos existentes
  contar_ciclos(&g, origemInd, origemInd, visitado, 0, &totalCiclos);

  // 5. impressão da mensagem
  if (totalCiclos == 0) printf("Quantidade de ciclos: -1\n");
  else printf("Quantidade de ciclos: %d\n", totalCiclos);

  // 6. liberação da memória e close
  free(visitado);
  free_grafo(g);
  free(h);
  fclose(arqBin);
}