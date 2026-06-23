/*
Karen Nanamy Kamo - NUSP: 15495932 
Rebeca de Oliveira Silva - NUSP: 11963923
*/

#ifndef LER_ARQ_H
#define LER_ARQ_H


#include "structs.h"


/*!
 * @brief Lê uma linha do arquivo csv e resgata os dados para um registro.
 *
 * @param f Arquivo csv para ser lido. Precisa estar aberto no modo leitura.
 *
 * @return Retorna um RegistroDado com os valores lidos da linha do csv.
 */
RegistroDado ler_reg_dado_csv(FILE* f);

/*!
 * @brief Lê registro de cabeçalho do arquivo bin e resgata os dados para um registro.
 *
 * @param arqBin Arquivo bin para ser lido. Precisa estar aberto no modo leitura.
 *
 * @return Retorna um RegistroCabecalho com os valores lidos do bin.
 */
RegistroCabecalho *ler_reg_cab_bin(FILE* arqBin);

/*!
 * @brief Lê registro do arquivo bin e resgata os dados para um registro.
 *
 * @param arqBin Arquivo bin para ser lido. Precisa estar aberto no modo leitura.
 *
 * @return Retorna um RegistroDado com os valores lidos da linha do bin.
 */
RegistroDado *ler_reg_dado_bin(FILE* arqBin);

/*!
 * @brief Lê registro de cabeçalho do arquivo de índice e resgata para um registro.
 *
 * @param arqInd Arquivo de índice para ser lido. Precisa estar aberto no modo leitura.
 *
 * @return Retorna um RegistroCabecalhoIndice com os valores lidos do arquivo de .
 */
RegistroCabecalhoIndice *ler_reg_cab_ind(FILE* arqInd);
#endif