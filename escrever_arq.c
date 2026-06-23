/*
Karen Nanamy Kamo - NUSP: 15495932 
Rebeca de Oliveira Silva - NUSP: 11963923
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "structs.h"
#include "escrever_arq.h"
#include "uteis.h"


void inicializar_cabecalho(RegistroCabecalho *h){
  //Valores iniciais do cabeçalho, quando cria o bin pela primeira vez
  h->status = '0'; // 0 para inconsistente
  h->topo = -1;
  h->proxRRN = 0;
  h->nroEstacoes = 0;
  h->nroParesEstacoes = 0;
}

void escreve_reg_cab_bin(FILE* arq, RegistroCabecalho *h){
  // movendo o ponteiro para o início do arquivo
  fseek(arq, 0, SEEK_SET);

  // inserção do registro de cabeçalho no arquivo bin
  // registro de cabeçalho com 17 bytes
  fwrite(&h->status, sizeof(char), 1, arq);
  fwrite(&h->topo, sizeof(int), 1, arq);
  fwrite(&h->proxRRN, sizeof(int), 1, arq);
  fwrite(&h->nroEstacoes, sizeof(int), 1, arq);
  fwrite(&h->nroParesEstacoes, sizeof(int), 1, arq);
}

void escreve_reg_dado_bin(FILE* arq, const RegistroDado* r){
  // para calcular se os 80 bytes foram utilizados
  int bytes_usados = 0;

  // inserção do registro de dado no arquivo bin
  // cada um deve ter 80 bytes
  fwrite(&r->removido, sizeof(char), 1, arq);
  fwrite(&r->proximo, sizeof(int), 1, arq);
  fwrite(&r->codEstacao, sizeof(int), 1, arq);
  fwrite(&r->codLinha, sizeof(int), 1, arq);
  fwrite(&r->codProxEstacao, sizeof(int), 1, arq);
  fwrite(&r->distProxEstacao, sizeof(int), 1, arq);
  fwrite(&r->codLinhaIntegra, sizeof(int), 1, arq);
  fwrite(&r->codEstIntegra, sizeof(int), 1, arq);

  // 1 (byte char) + 7 * 4 (byte int) = 29 bytes até aqui
  bytes_usados += 29;

  fwrite(&r->tamNomeEstacao, sizeof(int), 1, arq);
  if (r->tamNomeEstacao > 0 && r->nomeEstacao != NULL){ // se for diferente, quer dizer que tem valor
    fwrite(r->nomeEstacao, sizeof(char), (size_t)r->tamNomeEstacao, arq);
  }
  // 4 (byte int) + quantidades de char que tem o nomeEstacao
  bytes_usados = bytes_usados + 4 + r->tamNomeEstacao;

  fwrite(&r->tamNomeLinha, sizeof(int), 1, arq);
  if (r->tamNomeLinha > 0 && r->nomeLinha != NULL){
    fwrite(r->nomeLinha, sizeof(char), (size_t)r->tamNomeLinha, arq);
  }
  // 4 (byte int) + quantidades de char que tem o nomeLinha
  bytes_usados = bytes_usados + 4 + r->tamNomeLinha;

  // vai preencher os espaços vazios com $ até dar os 80 bytes
  while (bytes_usados < 80){
    fputc('$', arq);
    bytes_usados++;
  }
}

////////////////////////////////////////////////////////////////

void escreve_reg_cab_ind(FILE* arq, RegistroCabecalhoIndice *hInd){
  // movendo o ponteiro para o início do arquivo
  fseek(arq, 0, SEEK_SET);

  // inserção do registro de cabeçalho no arquivo de índice
  fwrite(&hInd->status, sizeof(char), 1, arq);
}

void escreve_reg_dado_ind(FILE* arq, const RegistroDadoIndice* rInd){
  // inserção de um registro de dados de índice
  fwrite(&rInd->codEstacao, sizeof(int), 1, arq);
  fwrite(&rInd->RRN, sizeof(int), 1, arq);
}