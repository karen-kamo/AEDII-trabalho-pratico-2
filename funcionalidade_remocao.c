/*
Karen Nanamy Kamo - NUSP: 15495932 
Rebeca de Oliveira Silva - NUSP: 11963923
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "structs.h"
#include "ler_arq.h"
#include "escrever_arq.h"
#include "funcionalidades.h"
#include "uteis.h"

/* ================================================================================

******************************FUNCIONALIDADE 7************************************

===================================================================================*/ 

/* Funcionalidades: 

  1. Abrir o arquivo binário para escrita e leitura
  2. Carrega o índice para a RAM - fiz primeiro, pois abre e fecha arq na função de carregar
  3. Abrir o arquivo de índice para escrita e leitura
  4. Mudar status dos arquivos
  5. Loop para a quant de remoções que serão feitas
  6. Realizando a busca do RRN que quero remover
  7. Se existir, fazer a remoção lógica
  8. Salvar os dados nos arquivos binários
  9. Liberações de memória e fechamento
  10. Chamar o BinarioNaTela no final
*/

void remover_registro(){
  char nomeArqBin[100];
  char nomeArqInd[100];
  int nRemocoes; // quant de remoções que serão feitas

  scanf("%s %s %d", nomeArqBin, nomeArqInd, &nRemocoes);

  // 1. Abrir o arquivo binário para escrita e leitura
  FILE *arqBin = NULL; // inicializa o ponteiro
  RegistroCabecalho *h = abrir_e_validar_arq_bin(nomeArqBin, &arqBin, "rb+"); // para leitura e escrita
  if (h == NULL) return; // se der errado, só para

  // 2. Carrega o índice para a RAM - fiz primeiro, pois abre e fecha arq na função de carregar
  int nRegistrosIndice = 0; // para guardar quant de regs
  RegistroDadoIndice *listaIndice = carregamento(nomeArqInd, &nRegistrosIndice);
  if (listaIndice == NULL){ // se deu errado
    free_reg_cab(h);
    fclose(arqBin);
    return; // quer dizer que não tem os regs 
  }

  // 3. Abrir o arquivo de índice para escrita e leitura
  FILE *arqInd = NULL; // inicializa o ponteiro
  RegistroCabecalhoIndice *hInd = abrir_e_validar_ind(nomeArqInd, &arqInd, "rb+"); // para leitura e escrita
  if (hInd == NULL) { // se der errado, só para
      free_reg_cab(h);
      fclose(arqBin);
      free(listaIndice);
      return;
   }

  // 4. Mudar status dos arquivos
  h->status = '0';
  escreve_reg_cab_bin(arqBin, h);

  hInd->status = '0';
  escreve_reg_cab_ind(arqInd, hInd);

  // 5. Loop para a quant de remoções que serão feitas
  for (int i = 0; i < nRemocoes; i++){
    int posSequencial = 17; // para busca - inicia após cabeçalho
    RegistroDado *r = NULL; // ponteiro para receber o resultado do reg removido

    // 6. Realizando a busca do RRN que quero remover
    while ((r = executar_busca_indexada(listaIndice, nRegistrosIndice, h, arqBin, &posSequencial)) != NULL){
      // acabou de ser lido, então dá p/ calcular
      int byteOffSet = ftell(arqBin) - 80;
      int rrnRemovido = (byteOffSet - 17) / 80;

      if (rrnRemovido < h->proxRRN){
        // 7. Se existir, fazer a remoção lógica 
        if (r->removido == '0') { // se existir o registro

          // verificar se a estação que estamos apagando era a última ativa com esse nome
          if (!existe_nome_estacao(arqBin, h, r->nomeEstacao, rrnRemovido)) h->nroEstacoes--;

          // vrificar se esse par era o último ativo
          if (r->codProxEstacao != -1 && !existe_par(arqBin, h, r->codEstacao, r->codProxEstacao, rrnRemovido)) h->nroParesEstacoes--;

          // salva dados da remoção
          r->removido = '1';     
          r->proximo = h->topo;  
          h->topo = rrnRemovido; 

          fseek(arqBin, byteOffSet, SEEK_SET); 
          escreve_reg_dado_bin(arqBin, r);

          // atualizando a lista de índices na RAM
          // procura a posição do registro na lista dinâmica
          int posRegistroLista = busca_binaria_posicao_lista_indice(listaIndice, nRegistrosIndice, r->codEstacao);

          // se encontrou, atualiza com -1 para mostrar remoção
          if (posRegistroLista != -1) listaIndice[posRegistroLista].RRN = -1;

          free_reg_dado(r);
          free(r);
        }

        if (posSequencial == 17) break; // se busca por codEstacao, não precisa continuar
      }
    }
  }

  // 8. Salvar os dados nos arquivos binários
  // salvando o status do arquivo binário
  h->status = '1';
  escreve_reg_cab_bin(arqBin, h);

  // para remover qualquer lixo do arquivo
  fclose(arqInd); // fecha o primeiro fluxo de rb+
  arqInd = fopen(nomeArqInd, "wb"); // abre de novo para só escrita

  // salvando a lista da RAM
  reescrita(arqInd, listaIndice, nRegistrosIndice); 

  // salvando o status do arquivo de índice
  hInd->status = '1';
  escreve_reg_cab_ind(arqInd, hInd);

  // 9. Liberações de memória e fechamento
  free(listaIndice);
  free(h);
  free(hInd);
  fclose(arqBin);
  fclose(arqInd);

  // 10. Chamar o BinarioNaTela no final
  BinarioNaTela(nomeArqBin);
  BinarioNaTela(nomeArqInd);
}