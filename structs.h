/*
Karen Nanamy Kamo - NUSP: 15495932 
Rebeca de Oliveira Silva - NUSP: 11963923
*/

#ifndef STRUCTS_H
#define STRUCTS_H

// Struct para o Registro de Cabeçalho - 17 bytes
typedef struct {
  char status;          // char de 1 byte
  int topo;             // int de 4 bytes
  int proxRRN;          // int de 4 bytes
  int nroEstacoes;      // int de 4 bytes
  int nroParesEstacoes; // int de 4 bytes
} RegistroCabecalho;

// Struct para o Registro de Dados - cada um terá 80 bytes
typedef struct {
  char removido;        // char de 1 byte
  int proximo;          // int de 4 bytes
  int codEstacao;       // int de 4 bytes
  int codLinha;         // int de 4 bytes
  int codProxEstacao;   // int de 4 bytes
  int distProxEstacao;  // int de 4 bytes
  int codLinhaIntegra;  // int de 4 bytes
  int codEstIntegra;    // int de 4 bytes
  int tamNomeEstacao;   // int de 4 bytes
  char *nomeEstacao;    // string tamanho variável
  int tamNomeLinha;     // int de 4 bytes
  char *nomeLinha;      // string tamanho variável
} RegistroDado;

// Struct para o Registro de Cabeçalho do Índice - 1 byte
typedef struct {
  char status;  // char de 1 byte
} RegistroCabecalhoIndice;

// Struct para o Registro de Dados do Índice - 8 bytes
typedef struct {
  int codEstacao; // int de 4 bytes
  int RRN;        // int de 4 bytes
} RegistroDadoIndice;

// Struct para o Nó da lista linear (Aresta)
typedef struct Aresta{
  char nomeProxEst[50]; // nome da pŕxoima estação
  int distancia;        // distância até próxima estação
  char nomesLinha[100]; // nomes das linhas associadas
  struct Aresta *prox;  // próximo nó da lista
} Aresta;

// Struct do Vértice
typedef struct {
  char nomeEstacao[50]; // nome da estação
  Aresta *inicioLista;  // ponteiro para o 1 nó de conexões
} Vertice;

// Struct do Grafo
typedef struct {
  Vertice *vertices; // vetor dinâmico de vértices
  int nroVertices;   // quant de estações únicas
} Grafo;

/*! 
 * @brief Desaloca os campos dinâmicamente alocados do registro de cabeçalho.
 * 
 * @param h Registro de cabeçalho que precisa ser liberado.
 */
void free_reg_cab(RegistroCabecalho* h);

/*! 
 * @brief Desaloca os campos dinâmicamente alocados do registro de dado.
 * 
 * @param r Registro de dado que precisa ser liberado.
 */
void free_reg_dado(RegistroDado* r);

#endif