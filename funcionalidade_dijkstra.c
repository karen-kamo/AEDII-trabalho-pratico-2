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

******************************FUNCIONALIDADE 11************************************

===================================================================================*/ 
/* Funcionalidades: 
  1. Leitura dos parâmetros conforme a especificação
  2. abrir arquivo bin para leitura
  3. inicialização do Grafo
  4. encontrar os índices numéricos da origem e do destino no vetor ordenado
  5. aplicando Dijkstra
  7. Liberações e close
*/

// Struct para auxiliar no controle do processo de buscar o menor caminho
typedef struct {
    int distancia; // vai guardar menor distância total acumulada
    int antecessor; // guarda o índice da estaçãi anterior mais curto
    int visitado; // representa o conjunto S (se 1, pertence a S - sabe o menor caminho até ele. Se 0, pertence a V - S - ainda analisando)
} ControleDijkstra;

void dijkstra() {
    char nomeArqBin[100], nomeArqInd[100];
    char campoOrigem[50], campoDestino[50];
    char estOrigem[100], estDestino[100];

    // 1. Leitura dos parâmetros conforme a especificação
    scanf("%s", nomeArqBin);
    scanf("%s", nomeArqInd);
    scanf("%s", campoOrigem);
    ScanQuoteString(estOrigem); 
    scanf("%s", campoDestino);
    ScanQuoteString(estDestino);

    // 2. abrir arquivo bin para leitura
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

    // 4. encontrar os índices numéricos da origem e do destino no vetor ordenado
    int indOrigem = -1;
    int indDestino = -1;
    for (int i = 0; i < g.nroVertices; i++) {
        // se nome inserido bate com o nome no grafo
        if (strcmp(g.vertices[i].nomeEstacao, estOrigem) == 0) indOrigem = i; 
        if (strcmp(g.vertices[i].nomeEstacao, estDestino) == 0) indDestino = i;
    }

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

    // verificação da lista de adjacência do vértice de origem
    // atualiza as distâncias no controle
    Aresta *arestaOrigem = g.vertices[indOrigem].inicioLista; // pega o primeiro caminho que sai da estação de origem

    while (arestaOrigem != NULL) {
        int indAdj = -1; // p/ guardar o índice 
        for (int i = 0; i < g.nroVertices; i++) {
            if (strcmp(g.vertices[i].nomeEstacao, arestaOrigem->nomeProxEst) == 0) {
                // se o nome bate com o nome de destino salvo na aresta
                indAdj = i; // guarda o índice
                break;
            }
        }
        // se encontrou, armazena os valores na tabela de controle
        if (indAdj != -1) {
            controle[indAdj].distancia = arestaOrigem->distancia;
            controle[indAdj].antecessor = indOrigem;
        }

        arestaOrigem = (Aresta *) arestaOrigem->prox;
    }

    // loop para achar menor caminho para cada estação do grafo
    for (int cont = 0; cont < g.nroVertices; cont++) {
        
        // encontre um vértice w pertencente a (V - S) tal que D[w] é mínimo
        int w = -1; // guarda índice do melhor vértice
        int menorDist = 99999999; // guarda distancia até essa melhor estação

        // loop para percorrer toda tabela de controle
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
            int v = -1; // guardar índice de v (estação vizinha)
            // para descobrir qual o índice de v
            for (int i = 0; i < g.nroVertices; i++) {
                if (strcmp(g.vertices[i].nomeEstacao, aresta->nomeProxEst) == 0) {
                    v = i;
                    break;
                }
            }

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
            if (i > 0) {
                printf(", ");
            }
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

