/*
Karen Nanamy Kamo - NUSP: 15495932 
Rebeca de Oliveira Silva - NUSP: 11963923
*/

#ifndef ESCREVER_ARQ_H
#define ESCREVER_ARQ_H

#include "structs.h"

/*!  
 * @brief Inicializar os valores iniciais do registro de cabeçalho.
 *
 * @param cab Struct que irá receber os valores iniciais.
 * 
 */
void inicializar_cabecalho(RegistroCabecalho *cab);

/*!  
 * @brief Escreve um header em um arquivo binario.
 *
 * @param arq É o arquivo binário no qual deseja-se
 * escrever o header. O arquivo precisa estar em
 * um modo que permite a escrita.
 * O ponteiro de arquivo é movido.
 * 
 * @param h É o header que deseja escrever.
 */
void escreve_reg_cab_bin(FILE* arq, RegistroCabecalho *h);

/*!  
 * @brief Escreve um registro em um arquivo binario.
 *
 * @param arq É o arquivo binário no qual deseja-se
 * escrever o registro. O arquivo precisa estar em
 * um modo que permite a escrita.
 * O ponteiro de arquivo é movido.
 * 
 * @param r É o registro que deseja escrever.
 */
void escreve_reg_dado_bin(FILE* arq, const RegistroDado* r);

////////////////////////////////////////////////////////////////

/*!  
 * @brief Escreve um header em um arquivo de índice.
 *
 * @param arq É o arquivo binário no qual deseja-se
 * escrever o header. O arquivo precisa estar em
 * um modo que permite a escrita.
 * O ponteiro de arquivo é movido.
 * 
 * @param hInd É o header que deseja escrever.
 */
void escreve_reg_cab_ind(FILE* arq, RegistroCabecalhoIndice *hInd);

/*!  
 * @brief Escreve um registro de dados em um arquivo de índice.
 *
 * @param arq É o arquivo binário no qual deseja-se
 * escrever o registro. O arquivo precisa estar em
 * um modo que permite a escrita.
 * O ponteiro de arquivo é movido.
 * 
 * @param rInd É o registro que deseja escrever.
 */
void escreve_reg_dado_ind(FILE* arq, const RegistroDadoIndice* rInd);

#endif