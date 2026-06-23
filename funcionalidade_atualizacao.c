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

******************************FUNCIONALIDADE 9************************************

===================================================================================*/ 
/* Funcionalidades: 
   1. Abrir o arquivo binário para escrita e leitura
   2. Carrega o índice para a RAM - fiz primeiro, pois abre e fecha arq na função de carregar
   3. Abrir o arquivo de índice para escrita e leitura
   4. Mudar status dos arquivos
   5. Loop para rodar a quant de vezes que usuário quer alterar
   6. Fazer a busca pelos campos e valores desejados
   7. Guardar campo e valor que usuário quer alterar
   8. Verificar mudança no Registro de Cabeçalho
   9. Atualização no registro de dados
   10. Ordena a lista de índices da RAM
   11. Mudar status nos arquivos binários
   12. Liberações de memória e fechamento
   13. Rodar o BinarioNaTela
*/


void atualizar_registro() {
   char nomeArqDados[100];
   char nomeArqIndice[100];  
   int nAtualizacoes; // quant de atualizações que serão feitas

   scanf("%s %s %d", nomeArqDados, nomeArqIndice, &nAtualizacoes);

   // 1. Abrir o arquivo binário para escrita e leitura
   FILE *arqBin = NULL; // inicializa o ponteiro
   RegistroCabecalho *h = abrir_e_validar_arq_bin(nomeArqDados, &arqBin, "rb+"); // para leitura e escrita
   if (h == NULL) return; // se der errado, só para

   // 2. Carrega o índice para a RAM - fiz primeiro, pois abre e fecha arq na função de carregar
   int nRegistrosIndice = 0; // para guardar quant de regs
   RegistroDadoIndice *listaIndice = carregamento(nomeArqIndice, &nRegistrosIndice);
   if (listaIndice == NULL){ // se deu errado
      free_reg_cab(h);
      fclose(arqBin);
      return; // quer dizer que não tem os regs 
   }

   // 3. Abrir o arquivo de índice para escrita e leitura
   FILE *arqInd = NULL; // inicializa o ponteiro
   RegistroCabecalhoIndice *hInd = abrir_e_validar_ind(nomeArqIndice, &arqInd, "rb+"); // para leitura e escrita
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

   // 5. Loop para rodar a quant de vezes que usuário quer alterar
   for (int i = 0; i < nAtualizacoes; i++){ 
      // 6. Fazer a busca pelos campos e valores desejados
      int posSequencial = 17; // p/ fazer a busca
      RegistroDado *r = NULL; // ponteiro para receber o resultado do reg buscado

      // função para fazer a busca  
      r = executar_busca_indexada(listaIndice, nRegistrosIndice, h, arqBin, &posSequencial);
     
      // 7. Guardar campo e valor que usuário quer alterar
      int quantAlt; // quant de pares para serem alterados
      scanf("%d", &quantAlt); 

      // armazenar os campos a serem atualizados e os valores novos
      char nomeCampoNew[100][500]; 
      char valorCampoNew[100][500]; 

      // flags para regras específicas do cab
      int flagCod = 0;
      int flagNomeUnico = 0;
      int flagPar = 0;

      for (int j = 0; j < quantAlt; j++){ // loop p/ a quant de pares que serão alterados
         scanf("%s", nomeCampoNew[j]); // lê o nome do campo desajado

         if (strcmp(nomeCampoNew[j], "nomeEstacao") == 0 ||
            strcmp(nomeCampoNew[j], "nomeLinha") == 0
         ) ScanQuoteString(valorCampoNew[j]); // se for string, para ler "" 
         else scanf("%s", valorCampoNew[j]);  // se for int, lê normalmente

         // verificando se os campos nomEstacao, codEstacao e codProxEstacao serão mudados
         // pois, tem regras específicas para eles
         if (strcmp(nomeCampoNew[j], "nomeEstacao") == 0)flagNomeUnico = 1;
         else if (strcmp(nomeCampoNew[j], "codEstacao") == 0)flagCod = 1;
         else if (strcmp(nomeCampoNew[j], "codProxEstacao") == 0)flagPar = 1;
      }

      while (r != NULL){ // se ainda tiver registro válido
         int byteOffSet = ftell(arqBin) - 80; // como leu agora pouco, sabe o byte
         int rrnIgnorado = (byteOffSet - 17) / 80;

         if (rrnIgnorado < h->proxRRN){
            if (r->removido == '0'){
               // 8. Verificar mudança no Registro de Cabeçalho

               // vendo quais são os novos valores
               char *novoNomeEstacao = r->nomeEstacao; 
               int novoCodEstacao = r->codEstacao;
               int novoCodProxEstacao = r->codProxEstacao;

               // verificando se o usuário pediu para alterar para NULO
               for (int j = 0; j < quantAlt; j++) {
                  if (strcmp(nomeCampoNew[j], "nomeEstacao") == 0) {
                     if (strcmp(valorCampoNew[j], "NULO") == 0) novoNomeEstacao = NULL;
                     else novoNomeEstacao = valorCampoNew[j];
                  }
                  if (strcmp(nomeCampoNew[j], "codEstacao") == 0) novoCodEstacao = verificar_nulo(valorCampoNew[j]);
                  if (strcmp(nomeCampoNew[j], "codProxEstacao") == 0) novoCodProxEstacao = verificar_nulo(valorCampoNew[j]);
               }

               // verificação do nroEstacoes
               if (flagNomeUnico){
                  // se não tiver nenhum outro registro com o mesmo nome, deve decrementar
                  if (r->nomeEstacao != NULL && !existe_nome_estacao(arqBin, h, r->nomeEstacao, rrnIgnorado)) h->nroEstacoes--;

                  // se é um novo nome, deve incrementar
                  if (novoNomeEstacao != NULL && !existe_nome_estacao(arqBin, h, novoNomeEstacao, rrnIgnorado)) h->nroEstacoes++;
               }

               // verificação do nroParesEstacoes
               if (flagCod || flagPar) {
                  // se era um par que não existia em mais nenhum, decrementa
                  if (r->codProxEstacao != -1 && !existe_par(arqBin, h, r->codEstacao, r->codProxEstacao, rrnIgnorado)) h->nroParesEstacoes--;

                  // se é um novo par, incrementa
                  if (novoCodProxEstacao != -1 && !existe_par(arqBin, h, novoCodEstacao, novoCodProxEstacao, rrnIgnorado)) h->nroParesEstacoes++;
               }

               // 9. Atualização no registro de dados
               for (int b = 0; b < quantAlt; b++) {
                  // atualizar o codEstacao
                  if (strcmp(nomeCampoNew[b], "codEstacao") == 0) {
                     int novoCod = verificar_nulo(valorCampoNew[b]); 
                     
                     // procura o código antigo no índice e atualiza com o novo
                     int posNoIndice = busca_binaria_posicao_lista_indice(listaIndice, nRegistrosIndice, r->codEstacao);

                     //se for igual a -1 não achou o indice do codEstacao
                     if (posNoIndice != -1) listaIndice[posNoIndice].codEstacao = novoCod; //atualiza a lista que esta na RAM

                     r->codEstacao = novoCod; //atualiza na Struct/Registro de Dados
                  }

                  // atualizar o nomeEstacao
                  else if (strcmp(nomeCampoNew[b], "nomeEstacao") == 0) {
                     // precisamos dar free para não causar vazamento de memória
                     free(r->nomeEstacao); // limpa o nome antigo
                     if (strcmp(valorCampoNew[b], "NULO") == 0) { //Caso o usuário digite "NULO" 
                        r->nomeEstacao = NULL;
                        r->tamNomeEstacao = 0;
                     } else {
                        r->nomeEstacao = malloc((strlen(valorCampoNew[b]) + 1) * sizeof(char)); //Alocação dinâmica para a nova string
                        strcpy(r->nomeEstacao, valorCampoNew[b]); //Copia para dentro do Registro de Dados
                        r->tamNomeEstacao = strlen(valorCampoNew[b]); //Pega o tamanho da string fornecida pelo usuário
                     }
                  }

                  // atualizar o codLinha
                  else if (strcmp(nomeCampoNew[b], "codLinha") == 0) r->codLinha = verificar_nulo(valorCampoNew[b]);

                  // atualizar o nomeLinha 
                  else if (strcmp(nomeCampoNew[b], "nomeLinha") == 0) {
                     free(r->nomeLinha); 
                     if (strcmp(valorCampoNew[b], "NULO") == 0) {
                        r->nomeLinha = NULL;
                        r->tamNomeLinha = 0;
                     } else {
                        r->nomeLinha = malloc((strlen(valorCampoNew[b]) + 1) * sizeof(char));
                        strcpy(r->nomeLinha, valorCampoNew[b]);
                        r->tamNomeLinha = strlen(valorCampoNew[b]);
                     }
                  }

                  // atualizar o codProxEstacao
                  else if (strcmp(nomeCampoNew[b], "codProxEstacao") == 0) r->codProxEstacao = verificar_nulo(valorCampoNew[b]);

                  // atualizar o distProxEstacao
                  else if (strcmp(nomeCampoNew[b], "distProxEstacao") == 0) r->distProxEstacao = verificar_nulo(valorCampoNew[b]);

                  // atualizar o codLinhaIntegra
                  else if (strcmp(nomeCampoNew[b], "codLinhaIntegra") == 0) r->codLinhaIntegra = verificar_nulo(valorCampoNew[b]);
                     
                  // atualizar o codEstIntegra
                  else if (strcmp(nomeCampoNew[b], "codEstIntegra") == 0) r->codEstIntegra = verificar_nulo(valorCampoNew[b]);
               }

               fseek(arqBin, byteOffSet, SEEK_SET); // volta p/ mudar o arq de dados
               escreve_reg_dado_bin(arqBin, r);

               // se codEstacao foi mudado, reordena
               // precisa estar ordenado p/ próxima busca
               if (flagCod) heap(listaIndice, nRegistrosIndice);
            }

            free_reg_dado(r);
            free(r); 
         }

         // se busca por codEstacao, não precisa continuar
         if (posSequencial == 17) break; 

         // se busca por outro campo, continua procurando
         r = executar_busca_indexada(listaIndice, nRegistrosIndice, h, arqBin, &posSequencial);
      }
   } 

   // 10. Ordena a lista de índices da RAM
   heap(listaIndice, nRegistrosIndice); 

   // 11. Mudar status nos arquivos binários
  
   // salvando o status do arquivo binário
   h->status = '1';
   escreve_reg_cab_bin(arqBin, h);

   // para remover qualquer lixo do arquivo
   fclose(arqInd); // fecha o primeiro fluxo de rb+
   arqInd = fopen(nomeArqIndice, "wb"); // abre de novo para só escrita

   // salvando a lista da RAM
   reescrita(arqInd, listaIndice, nRegistrosIndice); 
   
   // salvando o status do arquivo de índice
   hInd->status = '1';
   escreve_reg_cab_ind(arqInd, hInd);

   // 12. Liberações de memória e fechamento
   free(listaIndice);
   free_reg_cab(h);
   free(hInd);
   fclose(arqBin);
   fclose(arqInd);

   // 13. Rodar o BinarioNaTela
   BinarioNaTela(nomeArqDados);
   BinarioNaTela(nomeArqIndice);
}
