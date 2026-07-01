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
 
******************************FUNCIONALIDADE 12************************************
 
===================================================================================*/
/* Funcionalidades: 
  1. Abrir arquivo de dados para leitura
  2. Criar o grafo original a partir da malha existente
  3. Buscar o vértice de origem informado pelo usuário
  4. Construir a árvore geradora mínima (algoritmo de Prim) a partir da origem
  5. Percorrer a árvore geradora mínima em profundidade, imprimindo cada aresta
  6. Liberação da memória e close
*/
 
// Busca o índice de um vértice no grafo a partir do nome da estação.
// Usa busca binária pois o vetor de vértices já está ordenado por nome (ver criar_grafo).
static int buscar_indice_vertice(Grafo *g, char *nomeEstacao) {
  int ini = 0;
  int fim = g->nroVertices - 1;
 
  while (ini <= fim) {
    int meio = (ini + fim) / 2;
    int comp = strcmp(g->vertices[meio].nomeEstacao, nomeEstacao);
 
    if (comp == 0) return meio;
    else if (comp < 0) ini = meio + 1;
    else fim = meio - 1;
  }
 
  return -1;
}
 
// Constrói a árvore geradora mínima a partir do grafo original, usando o algoritmo de Prim ensinado em sala de aula,
// começando pelo vértice de índice origemInd.
//
/* 
  Desempates: a cada passo, percorremos os vértices já inseridos na árvore (u) em ordem
  crescente de índice, e para cada um suas arestas (v) também em ordem crescente, já que o
  vetor de vértices e as listas de adjacência (inserir_aresta_ordenada) já vêm ordenados
  alfabeticamente. Como só trocamos a melhor aresta quando a distância é ESTRITAMENTE menor,
  em caso de empate de peso fica valendo a primeira encontrada nessa varredura, ou seja, a de
  menor u e, em empate entre u's, a de menor v — exatamente as regras (i) e (ii) do enunciado. 
*/
static Grafo construir_mst(Grafo *g, int origemInd) {
  Grafo mst;
  mst.nroVertices = g->nroVertices;
  mst.vertices = malloc(mst.nroVertices * sizeof(Vertice));
 
  // inicializa os vértices da MST com os mesmos nomes do grafo original, ainda sem arestas
  for (int i = 0; i < mst.nroVertices; i++) {
    strcpy(mst.vertices[i].nomeEstacao, g->vertices[i].nomeEstacao);
    mst.vertices[i].inicioLista = NULL;
  }
 
  int *naArvore = calloc(g->nroVertices, sizeof(int));
  naArvore[origemInd] = 1;
  int qtdNaArvore = 1;
 
  // enquanto ainda faltar vértice para entrar na árvore
  while (qtdNaArvore < g->nroVertices) {
    int melhorDist = -1;
    int melhorU = -1;
    int melhorV = -1;
    char melhorLinha[200] = "";
 
    // percorre, em ordem crescente, os vértices já inseridos na árvore
    for (int u = 0; u < g->nroVertices; u++) {
      if (!naArvore[u]) continue;
 
      // percorre as arestas de u no grafo original (já em ordem crescente de destino)
      Aresta *a = g->vertices[u].inicioLista;
      while (a != NULL) {
        int v = buscar_indice_vertice(g, a->nomeProxEst);
 
        // só interessa quem ainda está fora da árvore
        if (v != -1 && !naArvore[v]) {
          // encontrou uma aresta menor
          if (melhorU == -1 || a->distancia < melhorDist) {
            melhorDist = a->distancia;
            melhorU = u;
            melhorV = v;
            strcpy(melhorLinha, a->nomesLinha);
          }

          // empate no peso da aresta
          else if (melhorU != -1 && a->distancia == melhorDist){
            // regra (ii): se u for menor que o melhorU anterior
            if (u < melhorU){
              melhorU = u;
              melhorV = v;
              strcpy(melhorLinha, a->nomesLinha);
            }

            // se u for o mesmo, escolhe o menor v alfabeticamente
            else if (u == melhorU && v < melhorV){
              melhorV = v;
              strcpy(melhorLinha, a->nomesLinha);
            }
          }
        }
        a = a->prox;
      }
    }
 
    // se não achou nenhuma aresta de saída, a malha é desconexa: não há mais como crescer
    if (melhorU == -1) {
      mst.nroVertices = qtdNaArvore;
      break;
    }
 
    naArvore[melhorV] = 1;
    qtdNaArvore++;
 
    // insere a aresta nos dois sentidos, já que as linhas permitem ida e volta
    inserir_aresta_ordenada(&mst.vertices[melhorU], mst.vertices[melhorV].nomeEstacao, melhorDist, melhorLinha);
    inserir_aresta_ordenada(&mst.vertices[melhorV], mst.vertices[melhorU].nomeEstacao, melhorDist, melhorLinha);
  }
 
  free(naArvore);
  return mst;
}
 
// Percorre a árvore geradora mínima em profundidade a partir do vértice atual.
// A cada aresta descida (pai -> filho), imprime: estação atual, estação filha, distância.
// Como a lista de adjacência de cada vértice já está em ordem alfabética, os filhos de um
// mesmo vértice são naturalmente visitados/impressos em ordem crescente de nome.
static void imprimir_dfs_mst(Grafo *mst, int idxAtual, int *visitado) {
  visitado[idxAtual] = 1;
 
  Aresta *a = mst->vertices[idxAtual].inicioLista;
  while (a != NULL) {
    int idxFilho = buscar_indice_vertice(mst, a->nomeProxEst);
 
    // só desce se ainda não foi visitado, para não voltar pelo pai (a MST é uma árvore)
    if (idxFilho != -1 && !visitado[idxFilho]) {
      printf("%s, %s, %d\n", mst->vertices[idxAtual].nomeEstacao, a->nomeProxEst, a->distancia);
      imprimir_dfs_mst(mst, idxFilho, visitado);
    }
 
    a = a->prox;
  }
}


// Deixa o grafo totalmente bidirecional inserindo as voltas que faltam
void tornar_grafo_bidirecional(Grafo *g) {
    // Percorre cada estação 'u' do grafo
    for (int u = 0; u < g->nroVertices; u++) {
        Aresta *a = g->vertices[u].inicioLista;
        
        // Percorre todas as arestas que saem de 'u'
        while (a != NULL) {
            // Encontra o índice numérico 'v' da estação de destino (usando busca binária ou linear)
            int v = buscar_indice_vertice(g, a->nomeProxEst);
            
            if (v != -1) {
                // Verifica se a aresta de volta (v -> u) já existe na lista de 'v'
                int ja_existe = 0;
                Aresta *volta = g->vertices[v].inicioLista;
                while (volta != NULL) {
                    if (strcmp(volta->nomeProxEst, g->vertices[u].nomeEstacao) == 0) {
                        ja_existe = 1;
                        break;
                    }
                    volta = volta->prox;
                }
                
                // Se a volta não existir, insere ela de forma ordenada na lista de 'v'!
                if (!ja_existe) {
                    inserir_aresta_ordenada(&g->vertices[v], g->vertices[u].nomeEstacao, a->distancia, a->nomesLinha);
                }
            }
            a = a->prox;
        }
    }
}


void funcionalidade12() {
  // 1. Abrir arquivo de dados para leitura
  char nomeArqBin[100];
  char nomeArqInd[100];
  char nomeCampo[100];
  char valorOrigem[200];
 
  scanf("%s %s %s", nomeArqBin, nomeArqInd, nomeCampo); // leitura dos inputs do usuário
  ScanQuoteString(valorOrigem); // nomeEstacaoOrigem é campo de tamanho variável, lido entre aspas
 
  // abrir arquivo bin para leitura
  FILE *arqBin = fopen(nomeArqBin, "rb");
  if (arqBin == NULL) {
    printf("Falha na execução da funcionalidade.\n");
    return;
  }
 
  RegistroCabecalho *h = ler_reg_cab_bin(arqBin); // leitura do registro de cabeçalho
  if (h == NULL) { // verifica se a alocação não ocorreu
    printf("Falha na execução da funcionalidade.\n");
    fclose(arqBin);
    return;
  }
 
  // vendo se é inconsistente
  if (h->status == '0') {
    printf("Falha na execução da funcionalidade.\n");
    free(h);
    fclose(arqBin);
    return;
  }
 
  // 2. Criar o grafo original a partir da malha existente
  Grafo g = criar_grafo(arqBin);

  tornar_grafo_bidirecional(&g);
 
  // 3. Buscar o vértice de origem informado pelo usuário
  int origemInd = buscar_indice_vertice(&g, valorOrigem);
  if (origemInd == -1) { // estação de origem não existe na malha
    printf("Falha na execução da funcionalidade.\n");
    free_grafo(g);
    free(h);
    fclose(arqBin);
    return;
  }
 
  // 4. Construir a árvore geradora mínima a partir da origem
  Grafo mst = construir_mst(&g, origemInd);
 
  // 5. Percorrer a árvore geradora mínima em profundidade, imprimindo as arestas
  int *visitado = calloc(mst.nroVertices, sizeof(int));
  imprimir_dfs_mst(&mst, origemInd, visitado);
  free(visitado);
 
  // 6. Liberação da memória e close
  free_grafo(g);
  free_grafo(mst);
  free(h);
  fclose(arqBin);
}
