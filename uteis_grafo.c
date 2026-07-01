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

int comparar_vertices(const void *a, const void *b) {
  // transforma os ponteiros genéricos em ponteiros de Vertice
  const Vertice *v1 = (const Vertice *)a;
  const Vertice *v2 = (const Vertice *)b;
  
  // compara os nomes das estações alfabeticamente
  return strcmp(v1->nomeEstacao, v2->nomeEstacao);
}

////////////////////////////////////////////////

char *buscar_nome_por_codigo(FILE *arqBin, int codBuscado) {
  // guarda onde o arquivo estava lendo para não estragar o loop principal
  long posAnterior = ftell(arqBin);
  
  // volta para o início dos dados
  fseek(arqBin, 17, SEEK_SET);
  char *nomeEncontrado = NULL; // p/ guardar o nome 

  while (check_eof(arqBin)) {
    RegistroDado *r = ler_reg_dado_bin(arqBin);
    if (r == NULL) break;

    if (r->removido == '0' && r->codEstacao == codBuscado) {
      // aloca memória para retornar o nome
      nomeEncontrado = malloc((strlen(r->nomeEstacao) + 1) * sizeof(char));
      strcpy(nomeEncontrado, r->nomeEstacao);
      free_reg_dado(r);
      free(r);
      break; 
    }
    free_reg_dado(r);
    free(r);
  }

  // Rrestaura a posição original do arquivo
  fseek(arqBin, posAnterior, SEEK_SET);
  return nomeEncontrado;
}

////////////////////////////////////////////////

void ordenar_linhas(char *nomesLinha) {
  char buffer[200];
  strcpy(buffer, nomesLinha);

  char *palavras[20]; // pode até 20 linhas integradas na mesma estação
  int qtd = 0;

  // separa a string por vírgulas e espaços
  char *token = strtok(buffer, ", ");
  while (token != NULL) {
    palavras[qtd++] = token;
    token = strtok(NULL, ", ");
  }

  // Bubble Sort das palavras
  for (int i = 0; i < qtd - 1; i++) {
    for (int j = i + 1; j < qtd; j++) {
      if (strcmp(palavras[i], palavras[j]) > 0) {
        char *temp = palavras[i];
        palavras[i] = palavras[j];
        palavras[j] = temp;
      }
    }
  }

  // remonta a string original com as palavras ordenadas e separadas por ,
  nomesLinha[0] = '\0';
  for (int i = 0; i < qtd; i++) {
    strcat(nomesLinha, palavras[i]);
    if (i < qtd - 1) {
      strcat(nomesLinha, ", ");
    }
  }
}

////////////////////////////////////////////////

void inserir_aresta_ordenada(Vertice *v, char *nomeProx, int dist, char *nomeLinha) {
  Aresta *atual = (struct Aresta *) v->inicioLista;
  Aresta *anterior = NULL;

  // verifica se já existe conexão para essa mesma estação de destino
  while (atual != NULL) {
    if (strcmp(atual->nomeProxEst, nomeProx) == 0) {
      // se já existe e a nova linha for diferente, adiciona
      if (strstr(atual->nomesLinha, nomeLinha) == NULL) {
        strcat(atual->nomesLinha, ", ");
        strcat(atual->nomesLinha, nomeLinha);

        ordenar_linhas(atual->nomesLinha);
      }
      return; 
    }
    atual = atual->prox;
  }

  // se for um destino inédito, aloca a nova aresta
  struct Aresta *nova = (struct Aresta *) malloc(sizeof(Aresta));

  // coloca os dados na nova aresta
  strcpy(nova->nomeProxEst, nomeProx);
  nova->distancia = dist;
  strcpy(nova->nomesLinha, nomeLinha);
  nova->prox = NULL;

  // posiciona de forma ordenada e seguindo o alfabeto
  atual = (struct Aresta *) v->inicioLista;
  while (atual != NULL) {
    int comp = strcmp(atual->nomeProxEst, nomeProx);
    if (comp > 0) break; // se for maior alfabeticamente, para
    
    // em caso de empate no nome do destino, desempata pelo nome da linha
    if (comp == 0 && strcmp(atual->nomesLinha, nomeLinha) > 0) break; 

    anterior = atual;
    atual = atual->prox;
  }

  if (anterior == NULL) { // inserção no início
    nova->prox = (struct Aresta *) v->inicioLista;
    v->inicioLista = (Aresta *) nova;
  } else { // inserção no meio ou fim
    nova->prox = atual;
    anterior->prox = nova;
  }
}

////////////////////////////////////////////////

int buscar_indice_vertice(Grafo *g, char *nomeEstacao) {
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

////////////////////////////////////////////////