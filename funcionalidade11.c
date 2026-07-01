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
  1. Leitura das entradas conforme a especificação
  2. abrir arquivo bin para leitura
  3. inicialização do Grafo
  4. encontrar os índices numéricos da origem e do destino no vetor ordenado
  5. aplicando Dijkstra
  6. Impressão do menor caminho
  7. Liberações e close
*/

// Struct para auxiliar no controle do processo de buscar o menor caminho
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

    // 1. Leitura das entradas conforme a especificação
    scanf("%s %s %s", nomeArqBin, nomeArqInd, campoOrigem);
    ScanQuoteString(estOrigem); 
    scanf("%s", campoDestino);
    ScanQuoteString(estDestino);

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

    // 4. encontrar os índices da origem e do destino no vetor ordenado
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

    // 5. aplicando Dijkstra
    ControleDijkstra *controle = (ControleDijkstra *) malloc(g.nroVertices * sizeof(ControleDijkstra));

    // inicialização para todos os vértices do grafo
    for (int i = 0; i < g.nroVertices; i++) {
        controle[i].distancia = 99999999; // representa o infinito
        controle[i].antecessor = -1; // inicializa com -1
        controle[i].visitado = 0; // significa que está em (V - S)
    }

    // como é a origem, deve ter distância 0
    controle[indOrigem].distancia = 0;

    // loop para achar menor caminho para cada estação do grafo
    for (int cont = 0; cont < g.nroVertices; cont++) {
        
        // encontre um vértice w pertencente a (V - S) tal que D[w] é mínimo
        int w = -1; // guarda índice do melhor vértice
        int menorDist = 99999999; // guarda distancia até essa melhor estação

        // loop para percorrer toda lista de controle
        for (int i = 0; i < g.nroVertices; i++) {
            // verifica se pertence ao conjunto (V - S) e se distancia é menor do que já tem
            if (!controle[i].visitado && controle[i].distancia < menorDist) {
                menorDist = controle[i].distancia;
                w = i; // salva índice
            }
        }

        // se o mínimo for infinito, os vértices restantes são inacessíveis
        if (w == -1 || menorDist == 99999999) break;

        // coloca w no conjunto S
        controle[w].visitado = 1;

        // analise da lista de adjacência de w
        Aresta *aresta = g.vertices[w].inicioLista;

        while (aresta != NULL) {
            int v = buscar_indice_vertice(&g, aresta->nomeProxEst); // guardar índice de v (estação vizinha)

            // se foi encontrado e ainda não foi visitado
            if (v != -1 && !controle[v].visitado) {
                int peso = aresta->distancia;
                
                // D[v] = min(D[v], D[w] + Peso)
                // se distancia + peso for menor que a menor distancia já conhecida
                if (controle[w].distancia + peso < controle[v].distancia) {
                    controle[v].distancia = controle[w].distancia + peso;
                    controle[v].antecessor = w;
                }
                // desempate: se houver empate na distância, escolhe o menor u (w) alfabeticamente
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

    // 6. Impressão do menor caminho
    // se distancia for infinito, não encontrou o menor 
    if (controle[indDestino].distancia == 99999999) printf("Não existe caminho entre as estações solicitadas.\n");
    else {
        // reconstrói o caminho usando uma pilha improvisada
        int *caminho = (int *) malloc(g.nroVertices * sizeof(int));
        int tamCaminho = 0;
        
        int atual = indDestino;
        while (atual != -1) {
            caminho[tamCaminho++] = atual;
            atual = controle[atual].antecessor;
        }

        // Linha 1: número de estações percorridas (exclui a origem)
        printf("Numero de estacoes que serao percorridas: %d\n", tamCaminho - 1);

        // Linha 2: distância percorrida
        printf("Distancia que sera percorrida: %d\n", controle[indDestino].distancia);

        // Linha 3: nomeEstOrigem, ..., nomeEstDestino
        for (int i = tamCaminho - 1; i >= 0; i--) {
            printf("%s", g.vertices[caminho[i]].nomeEstacao);
            if (i > 0) printf(", ");
        }
        printf("\n");

        free(caminho);
    }

    // 7. Liberações e close
    free(controle);
    free_grafo(g);
    free(h);
    fclose(arqBin);
}

