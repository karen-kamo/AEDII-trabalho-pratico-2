/*
Karen Nanamy Kamo - NUSP: 15495932 
Rebeca de Oliveira Silva - NUSP: 11963923
*/

#ifndef UTEIS_H
#define UTEIS_H

#include "structs.h"

/*!
 * @brief Função que conta e printa na tela do usuário quantidade de binário na tela. Função disponibilizada pelo monitor
 *
 * @param arquivo O arquivo a ser lido.
 *
 */
void BinarioNaTela(char *arquivo);


/*!
 * @brief Função disponibilizada pelos monitores.
 *
 * @param str A string.
 *
 */
void ScanQuoteString(char *str);


/*!
 * @brief Checa se é o fim do arquivo.
 *
 * @param arq Arquivo csv para checar. Ele precisa permitir leitura.
 *
 * @return Retorna 0 caso seja o fim do arquivo, 1 caso o contrario.
 */
char check_eof (FILE* arq);


/*!
 * @brief Serve para imprimir o conteúdo de um registro de dado.
 *
 * @param r O registro de dado.
 *
 */
void imprimir_reg_dado(RegistroDado *r);


/*!
 * @brief Função auxiliar para mostrar o registro de cabeçalho.
 *
 * @param h O registro de dado.
 *
 */
void imprimir_reg_cab(RegistroCabecalho *h);


/*!
 * @brief Função auxiliar para verificar se o valor inteiro recebido é nulo. 
 *
 * @param valor O valor que deve ser verificado.
 * 
 * @return Retorna -1, caso seja nulo ou o valor inteiro.
 */
int verificar_nulo(char *valor);


/*!
 * @brief Função auxiliar para checa se o valor recebido do arquivo csv é nulo.
 *
 * @param string Valor recebido do csv.
 *
 * @return Retorna um int com o valor -1 se nulo, ou com o valor especificado.
 */
int verificar_nulo_fixo(char *string);


/*!
 * @brief Função para ordenar os registros de dados pelo codEstacao.
 *
 * @param array Vetor de registros de dados
 * 
 * @param n Número de registros de dados que tem no vetor.
 */
void heap (RegistroDadoIndice *array, int n);


/*!
 * @brief Função para carregar os registros de dados em memória primária
 *
 * @param nomeArqInd Nome do arquivo de índice
 * 
 * @param nRegistros Número de registros de dados que tem no vetor.
 * 
 * @return Retorna o ponteiro da memória primária que contém a lista de registros do índice
 */
RegistroDadoIndice *carregamento (char *nomeArqInd, int *nRegistros);


/*!
 * @brief Função para reescrever os registros de dados da lista de índice da memória primária no arquivo de índice.
 *
 * @param arqInd Ponteiro que aponta para o arquivo de índice.
 * 
 * @param listaIndice Lista de índices carregados em memória primária.
 * 
 * @param nRegistrosIndice Número de registros contido no índice.
 * 
 */
void reescrita(FILE *arqInd, RegistroDadoIndice *listaIndice, int nRegistrosIndice);


/*!
 * @brief Função para realizar a busca binária na lista de Registros de Dados do Índice.
 *
 * @param lista Lista de Registros de Dados do Índice.
 * 
 * @param n Quantidade de registros.
 * 
 * @param codBuscado O codEstacao buscado pelo usuário.
 * 
 * @return Retorna o RRN do codEstacao buscado ou -1, caso não encontre.
 */
int busca_binaria_indice(RegistroDadoIndice *lista, int n, int codBuscado);


/*!
 * @brief Função auxiliar que faz a busca binária tradicional, retorna a posição ocupada pelo registro na lista do índice na memória.
 *
 * @param listaIndice Lista dinâmica do índice.
 * 
 * @param nRegistrosIndice Número de registros.
 * 
 * @param codBuscado O codEstacao buscado.
 * 
 * @return Retorna a posição do registro buscado na lista de índices da memória. O outro retorna o RRN.
 */
int busca_binaria_posicao_lista_indice(RegistroDadoIndice *listaIndice, int nRegistrosIndice, int codBuscado);


/*!
 * @brief Função auxiliar para buscas com filtro, para verificar as buscas filtro a filtro.
 *
 * @param r Registro de dado a ser analisado.
 * 
 * @param nomesCampos Vetor com os campos a serem buscados.
 * 
 * @param valoresCampos Vetor com os valores a serem buscados.
 * 
 * @param quantPar Quantidade de pares que serão verificados.
 * 
 * @return Retorna 1 caso algum filtro de certo e 0 se não der em algum.
 */
int verificacao_filtros(RegistroDado *r, char nomesCampos[100][500], char valoresCampos[100][500], int quantPar);


/*!
 * @brief Função auxiliar para abrir um arquivo binário para o modo desejado e verificar o status do arquivo.
 *
 * @param nomeArqBin Nome do arquivo binário
 * 
 * @param arqBin O arquivo binário.
 * 
 * @param modo O modo de abertura do arquivo.
 * 
 * @return Retorna o RegistroCabecalho do arquivo binário.
 */
RegistroCabecalho *abrir_e_validar_arq_bin (char *nomeArqBin, FILE **arqBin, char modo[3]);


/*!
 * @brief Função auxiliar para abrir um arquivo de índice para o modo desejado e verificar o status do arquivo.
 *
 * @param nomeArqInd Nome do arquivo de índice
 * 
 * @param arqInd O arquivo de índice.
 * 
 * @param modo O modo de abertura do arquivo.
 * 
 * @return Retorna o RegistroCabecalho do arquivo de índice.
 */
RegistroCabecalhoIndice *abrir_e_validar_ind (char *nomeArqInd, FILE **arqInd, char modo[3]);


/*!
 * @brief Função auxiliar para verificar se o nomeEstacao existe no arquivo de dados.
 *
 * @param arqBin O arquivo de dados.
 * 
 * @param h O Registro de Cabeçalho do arquivo de dados.
 * 
 * @param nomeProcurado O nomeEstacao procurado.
 * 
 * @param rrnIgnorado RRN que não deve ser levado em conta para verificar os dados.
 * 
 * @return Retorna 1 se o nome já existe no arquivo de dados, 0 se não existe.
 */
int existe_nome_estacao(FILE *arqBin, RegistroCabecalho *h, char *nomeProcurado, int rrnIgnorado);


/*!
 * @brief Função auxiliar para verificar se o par (codEstacao, codProxEstacao) existe no arquivo de dados.
 *
 * @param arqBin O arquivo de dados.
 * 
 * @param h O Registro de Cabeçalho do arquivo de dados.
 * 
 * @param codEstacao O codEstacao para ser verificado.
 * 
 * @param codProxEstacao O codProxEstacao para ser verificado.
 * 
 * @param rrnIgnorado RRN que não deve ser levado em conta para verificar os dados.
 * 
 * @return Retorna 1 se o par já existe no arquivo de dados, 0 se não existe.
 */
int existe_par(FILE *arqBin, RegistroCabecalho *h, int codEstacao, int codProxEstacao, int rrnIgnorado);


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

#endif






