
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


/* ================================================================================

******************************FUNCIONALIDADE 11************************************

===================================================================================*/ 
/* Funcionalidades: 

  1. Abrir arquivo bin para leitura
  2. Inicialização do grafo
  3. Aplicando Dijkstra
  4. Impressão do menor caminho
*/

// struct para auxiliar no controle do processo de buscar o menor caminho, é uma posição para cada vértice que tem no grafo
typedef struct {
  int distancia; // vai guardar menor distância total acumulada
  int antecessor; // guarda o índice da estaçãi anterior mais curto
  int visitado; // representa o conjunto S (se 1, pertence a S - sabe o menor caminho até ele. Se 0, pertence a V - S - ainda analisando)
} ControleDijkstra;

void dijkstra() {
  char nomeArqBin[100];
  char nomeArqInd[100];
  char campoOrigem[50];
  char campoDestino[50];
  char estOrigem[100];
  char estDestino[100];

  scanf("%s %s %s", nomeArqBin, nomeArqInd, campoOrigem);
  ScanQuoteString(estOrigem); 
  scanf("%s", campoDestino);
  ScanQuoteString(estDestino);

  // 1. Abrir arquivo bin para leitura
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

  // pegando os índices da origem e do destino no vetor ordenado do grafo
  int indOrigem = buscar_indice_vertice(&g, estOrigem);
  int indDestino = buscar_indice_vertice(&g, estDestino);

  // se alguma das estações não existir no grafo
  if (indOrigem == -1 || indDestino == -1) {
    printf("Não existe caminho entre as estações solicitadas.\n");
    free_grafo(g);
    free(h);
    fclose(arqBin);
    return;
  }

  // 4. Aplicando Dijkstra
  ControleDijkstra *controle = (ControleDijkstra *) malloc(g.nroVertices * sizeof(ControleDijkstra));

  // inicialização para todos os vértices do grafo
  for (int i = 0; i < g.nroVertices; i++) {
    controle[i].distancia = 99999999; // representa o infinito, usamos esse valor para não ter risco de dar overflow
    controle[i].antecessor = -1; 
    controle[i].visitado = 0; // significa que está em (V - S)
  }
  controle[indOrigem].distancia = 0;

  // loop para achar menor caminho para cada estação do grafo
  for (int cont = 0; cont < g.nroVertices; cont++) {
      
    int w = -1; // guarda índice do melhor vértice
    int menorDist = 99999999; // guarda distancia até essa melhor estação

    for (int i = 0; i < g.nroVertices; i++) {
      // verifica se pertence ao conjunto (V - S) e se distancia é menor do que já tem
      if (!controle[i].visitado && controle[i].distancia < menorDist) {
        menorDist = controle[i].distancia;
        w = i; 
      }
    }

    // se o mínimo for infinito, os vértices restantes são inacessíveis
    if (w == -1 || menorDist == 99999999) break;

    // coloca w no conjunto S
    controle[w].visitado = 1;

    // analise da lista de adjacência de w
    Aresta *aresta = g.vertices[w].inicioLista;

    while (aresta != NULL) {
      int v = buscar_indice_vertice(&g, aresta->nomeProxEst);

      // se foi encontrado e ainda não foi visitado
      if (v != -1 && !controle[v].visitado) {
        int peso = aresta->distancia;
        
        if (controle[w].distancia + peso < controle[v].distancia) {
          controle[v].distancia = controle[w].distancia + peso;
          controle[v].antecessor = w;
        }
        // se tiver empate na distância, escolhe o menor w alfabeticamente
        else if (controle[w].distancia + peso == controle[v].distancia) {
          int w_antigo = controle[v].antecessor;
          if (w < w_antigo) {
            controle[v].antecessor = w;
          }
        }
      }
      aresta = (Aresta *) aresta->prox;
    }
  }

  // 5. Impressão do menor caminho
  // se distancia for infinito, não encontrou o menor 
  if (controle[indDestino].distancia == 99999999) printf("Não existe caminho entre as estações solicitadas.\n");
  else {
    // reconstrói o caminho de trás pra frente seguindos os antecessores
    int *caminho = (int *) malloc(g.nroVertices * sizeof(int));
    int tamCaminho = 0;
    
    int atual = indDestino;
    while (atual != -1) {
      caminho[tamCaminho++] = atual;
      atual = controle[atual].antecessor;
    }

    // print do número de estações percorridas
    printf("Numero de estacoes que serao percorridas: %d\n", tamCaminho - 1);

    //print da distância percorrida
    printf("Distancia que sera percorrida: %d\n", controle[indDestino].distancia);

    // print dos nomes na ordem certa
    for (int i = tamCaminho - 1; i >= 0; i--) {
      printf("%s", g.vertices[caminho[i]].nomeEstacao);
      if (i > 0) printf(", ");
    }
    printf("\n");

    free(caminho);
  }


  free(controle);
  free_grafo(g);
  free(h);
  fclose(arqBin);
}
