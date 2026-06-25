/*
Karen Nanamy Kamo - NUSP: 15495932 
Rebeca de Oliveira Silva - NUSP: 11963923
*/

#ifndef FUNCIONALIDADES_H
#define FUNCIONALIDADES_H


/*!
 * @brief Funcionalidade 1 para criação do arquivo binário a partir de um csv, com registro de cabeçalho e registros de dados.
 *
 */
void criar_arq_bin();


/*!
 * @brief Funcionalidade 2 para mostrar todos os dados dos registros de dados do arquivo binário
 *
 */
void buscar_todos_reg_bin();


/*!
 * @brief Funcionalidade 3 para mostrar o registro buscado por dados especificados na entrada.
 *
 */
void buscar_reg_filtro();


/*!
 * @brief Funcionalidade 4 para mostrar o registro buscado pelo RRN
 *
 */
void buscar_reg_RRN();


/*!
 * @brief Busca os valores do registro de cabeçalho.
 *
 */
void buscar_reg_cab_bin();

/*!
 * @brief Funcionalidade 5 para criação do arquivo de índice primário com registro de cabeçalho e registros de dados referentes às structs de índices.
 *
 */
void criar_indice();


/*!
 * @brief Funcionalidade 6 para mostrar o registro buscado por dados especificados na entrada, caso uma das buscas seja pelo codEstacao, a busca deve ser feita usando o arquivo de índice.
 *
 */
void busca_indexada();

/*!
 * @brief Funcionalidade 7 realizar a remoção lógica do registro de dado desejado tanto na arquivo de dados quanto no arquivo de índice.
 *
 */
void remover_registro();

/*!
* @brief Funcionalidade 8 para inserir novos registros e atualizar o índice
*
*/
void inserir_registro();

/*!
* @brief Funcionalidade 9 para buscar e atualizar os registros apontados
*
*/
void atualizar_registro();


/*!
 * @brief Função para realizar a busca indexada. Irá ser utilizada em várias funcionalidades, por isso, fiz só a busca em uma função a parte.
 *
 * @param listaIndice Lista de registros do índice que está na memória.
 * 
 * @param nRegistrosIndice O número de registros no arquivo de índice.
 * 
 * @param h O Registro de Cabecalho do arquivo de dados.
 * 
 * @param arqBin O arquivo binário.
 * 
 * @param posSequencial A posição atual para realizar a busca no arquivo binário.
 * 
 * @return Retorna o registro buscado ou NULL se não tiver registro correspondente.
 */
RegistroDado *executar_busca_indexada(RegistroDadoIndice *listaIndice, int nRegistrosIndice, RegistroCabecalho *h, FILE *arqBin, int *posSequencial);


/*!
* @brief Funcionalidade 10 para criar o grafo a partir do arquivo de dados.
*
*/
void criar_grafo();

#endif