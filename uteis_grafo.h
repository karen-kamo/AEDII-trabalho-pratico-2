/*
Karen Nanamy Kamo - NUSP: 15495932 
Rebeca de Oliveira Silva - NUSP: 11963923
*/

#ifndef UTEIS_GRAFO_H
#define UTEIS_GRAFO_H

#include "structs.h"

/*!
 * @brief Função auxiliar para usar o qsort.
 */
int comparar_vertices(const void *a, const void *b);


/*!
 * @brief Função para buscar o nome da estação a partir do codEstacao.
 *
 * @param arqBin O arquivo de dados.
 * 
 * @param codBuscado O codEstacao buscado.
 * 
 * @return Retorna o nome da estação encontrada.
 */
char *buscar_nome_por_codigo(FILE *arqBin, int codBuscado);


/*!
 * @brief Função auxiliar para ordenar as linhas dentro da string.
 *
 * @param nomesLinha String com os nomes das linhas que devem ser ordenadas.
 */
void ordenar_linhas(char *nomesLinha);


/*!
 * @brief Função para pegar uma linha entre duas estações e colocá-la no lugar exato da lista da estação de origem, garantindo que não duplique destinos e mantenha a ordem alfabética.
 *
 * @param v O ponteiro do vértice da estação de origem.
 * 
 * @param nomeProx O nome da próxima estação que está no registro.
 * 
 * @param dist A distância entre as estações.
 * 
 * @param nomeLinha O nome da linha que conecta as estações.
 */
void inserir_aresta_ordenada(Vertice *v, char *nomeProx, int dist, char *nomeLinha);


/*!
 * @brief Função auxiliar para realizar busca binária do índice de um vértice no grafo a partir do nome da estação.
 *
 * @param g Grafo a ser utilizado na busca.
 * 
 * @param nomeEstacao Nome da estação que quero buscar.
 * 
 * @return Retorna o índice do vértice no grafo ou -1 se não encontrou.
 */
int buscar_indice_vertice(Grafo *g, char *nomeEstacao);
#endif