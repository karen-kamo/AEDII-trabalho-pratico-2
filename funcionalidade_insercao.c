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

/* ================================================================================

******************************FUNCIONALIDADE 8************************************

===================================================================================*/ 
/* Funcionalidades: 
   1. Abrir o arquivo binário para escrita e leitura
   2. Carrega o índice para a RAM - fiz primeiro, pois abre e fecha arq na função de carregar
   3. Abrir o arquivo de índice para escrita e leitura
   4. Mudar status dos arquivos
   5. Pegar os valores para inserção
   6. Escrever o registro no arquivo de dados
   7. Inserir o novo registro na lista de índices
   8. Ordena a lista de índices da RAM
   9. Salvar os dados nos arquivos binários
   10. Liberações de memória e fechamento
   11. Rodar o BinarioNaTela
*/

void inserir_registro() {
   char nomeArqDados[100];
   char nomeArqIndice[100];
   int nInsercao; // quant de inserções que serão feitas

   scanf("%s %s %d", nomeArqDados, nomeArqIndice, &nInsercao);

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

   char buffer[1000]; // para guardar os strings digitados

   // loop de inserções
   for (int i = 0; i < nInsercao; i++) {
      // 5. Pegar os valores para inserção
      RegistroDado *r = (RegistroDado *) malloc(sizeof(RegistroDado));

      // leitura de dados pelo teclado para cada campo
      // codEstacao - lê para ver se é NULO, se não vai guardar o valor
      scanf("%s", buffer); // se é int, usa scanf
      r->codEstacao = verificar_nulo(buffer);

      // nomeEstacao - se é um valor para campo de tamanho variável usa ScanQuoteString
      ScanQuoteString(buffer);
      if (strcmp(buffer, "NULO") == 0) { // se é NULO
         r->nomeEstacao = NULL;
         r->tamNomeEstacao = 0;
      } else {
         r->nomeEstacao = malloc((strlen(buffer) + 1) * sizeof(char));
         strcpy(r->nomeEstacao, buffer); // copia o que foi digitado no reg
         r->tamNomeEstacao = strlen(buffer); // pega o tamanho da string
      }

      // codLinha - lê para ver se é NULO, se não vai guardar o valor
      scanf("%s", buffer); // se é int, usa scanf
      r->codLinha = verificar_nulo(buffer);

      // nomeLinha - se é um valor para campo de tamanho variável usa ScanQuoteString
      ScanQuoteString(buffer);
      if (strcmp(buffer, "NULO") == 0) { // se é NULO
         r->nomeLinha = NULL;
         r->tamNomeLinha = 0;
      } else {
         r->nomeLinha = malloc((strlen(buffer) + 1) * sizeof(char));
         strcpy(r->nomeLinha, buffer);
         r->tamNomeLinha = strlen(buffer);
      }

      // codProxEstacao
      scanf("%s", buffer); // se é int, usa scanf
      r->codProxEstacao = verificar_nulo(buffer);

      // distProxEstacao
      scanf("%s", buffer); // se é int, usa scanf
      r->distProxEstacao = verificar_nulo(buffer);

      // codLinhaIntegra
      scanf("%s", buffer); // se é int, usa scanf
      r->codLinhaIntegra = verificar_nulo(buffer);

      // codEstIntegra
      scanf("%s", buffer); // se é int, usa scanf
      r->codEstIntegra = verificar_nulo(buffer);

      // verificar se a estação que estamos apagando era a última ativa com esse nome
      if (!existe_nome_estacao(arqBin, h, r->nomeEstacao, -1)) h->nroEstacoes++;

      // vrificar se esse par era o último ativo
      if (r->codProxEstacao != -1 && !existe_par(arqBin, h, r->codEstacao, r->codProxEstacao, -1)) h->nroParesEstacoes++;

      // 6. Escrever o registro no arquivo de dados
      int rrnDestino;
      r->removido = '0'; // para o novo registro ficar ativo
      r->proximo = -1; // apontar que é o final
      
      if (h->topo == -1) { // se pilha vazia
         rrnDestino = h->proxRRN; // pega o próximo RRN
         int byteOffset = 17 + (rrnDestino * 80);
         fseek(arqBin, byteOffset, SEEK_SET);
         escreve_reg_dado_bin(arqBin, r);
         h->proxRRN++; // incrementa o próximo RRN disponível

      } else { //reaproveita espaço dos removidos
         rrnDestino = h->topo; // desempilha a pilha de removidos

         int byteOffset = 17 + (rrnDestino * 80);
         fseek(arqBin, byteOffset, SEEK_SET); // ir p/ posição a inserir

         char removido; 
         int proximoPilha; // p/ guardar p/ topo 
         fread(&removido, sizeof(char), 1, arqBin); // só p/ pular esse 1 byte 
         fread(&proximoPilha, sizeof(int), 1, arqBin);
         
         // voltamos ao início do espaço e gravamos o registro novo por cima
         fseek(arqBin, byteOffset, SEEK_SET); 
         escreve_reg_dado_bin(arqBin, r);
         
         h->topo = proximoPilha; // atualizamos o topo da pilha
      }

      // 7. Inserir o novo registro na lista de índices
      // realocar espaço para cada inserção nova
      listaIndice = realloc(listaIndice, (nRegistrosIndice + 1) * sizeof(RegistroDadoIndice));
      listaIndice[nRegistrosIndice].codEstacao = r->codEstacao;
      listaIndice[nRegistrosIndice].RRN = rrnDestino;
      nRegistrosIndice++;

      free_reg_dado(r);
      free(r);
   }

   // 8. Ordena a lista de índices da RAM
   heap(listaIndice, nRegistrosIndice); 

   // 9. Salvar os dados nos arquivos binários
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

   // 10. Liberações de memória e fechamento
   free(listaIndice);
   free_reg_cab(h);
   free(hInd);
   fclose(arqBin);
   fclose(arqInd);

   // 11. Rodar o BinarioNaTela
   BinarioNaTela(nomeArqDados);
   BinarioNaTela(nomeArqIndice);
}

