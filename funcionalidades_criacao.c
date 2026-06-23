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

******************************FUNCIONALIDADE 1************************************

===================================================================================*/ 

/* Funcionalidades: 
   
   1. Abrir o arquivo csv
   2. Criar o arquivo de saída, que deve ser .bin
   3. Criar o Registro de Cabeçalho e armazenar o Reg. de Cabeçalho no arquivo.bin
   4. Ler cada Registro de Dados, armazenar na struct e alocar no arquivo.bin
   5. Chamar o BinarioNaTela

*/
void criar_arq_bin(){
  char nomeArq[100];
  char nomeArqBin[100];

  scanf("%s %s", nomeArq, nomeArqBin); // leitura dos inputs do usuário

  // abrir arquivo csv para leitura
  FILE *arqCsv = fopen(nomeArq, "r");
  if (arqCsv == NULL) {
    printf("Falha no processamento do arquivo.\n");
    return;
  }

  // abrir arquivo bin para escrita
  FILE *arqBin = fopen(nomeArqBin, "wb");
  if (arqBin == NULL) {
    printf("Falha no processamento do arquivo.\n");
    return;
  }

  RegistroCabecalho *h = NULL; // incialização do Registro de cabeçalho
  h = (RegistroCabecalho *) malloc(sizeof(RegistroCabecalho)); // alocação do registro de cabeçalho
  if (h == NULL){ // verifica se a alocação não ocorreu
    fclose(arqCsv);
    fclose(arqBin);
    return;
  }

  inicializar_cabecalho(h);
  escreve_reg_cab_bin(arqBin, h);

  // descarta a primeira linha com os nomes dos campos
  char bf_lixo[256];
  fgets(bf_lixo, sizeof(bf_lixo), arqCsv);

  // variáveis auxiliares para definir os valores finais do reg cab no final dos reg de dados
  // 1. ações relacionadas ao contEstacoes
  // 2. ações relacionadas ao contPares
  int contRRN = 0, contEstacoes = 0, contPares = 0;

  // 1. lista dos nomes das estações únicas
  int capEstacoes = 100; // capacidade inicial
  char **estacoes = (char **) malloc(capEstacoes * sizeof(char *)); 

  // 2. lista de vetores com os pares codEstacao e codProxEstacao
  int capPares = 100; // capacidade inicial
  int (*paresUnicos)[2] = malloc(capPares * sizeof(int[2])); 

  //Enquanto o arquivo csv não acabar, leia linha por linha 
  while (check_eof(arqCsv)){
    RegistroDado r = ler_reg_dado_csv(arqCsv);

    // 1. precisa verificar se os nomes são iguais 
    int jaTemNome = 0;
    for (int i = 0; i < contEstacoes; i++){
      if (strcmp(r.nomeEstacao, estacoes[i]) == 0){
        jaTemNome = 1;
        break;
      }
    }

    if (!jaTemNome){
      if (contEstacoes == capEstacoes){ // quer dizer que atingiu limite
        capEstacoes *= 2; // aumentando a capacidade 
        estacoes = realloc(estacoes, capEstacoes * sizeof(char *)); // realocando espaço
      }
        estacoes[contEstacoes] = strdup(r.nomeEstacao); // 1. faz uma cópia para não ser apagado depois com o free
        contEstacoes++; // 1. soma toda vez que tem um nome único
    }

    // 2. precisa verificar se o par [codEstacao, codProxEstacao] já existe
    int jaTemPar = 0;
    if (r.codProxEstacao != -1){
      for (int i = 0; i < contPares; i++){
        // 2. verifica se já existe o par no vetor
        if (paresUnicos[i][0] == r.codEstacao && paresUnicos[i][1] == r.codProxEstacao){
          jaTemPar = 1;
          break;
        }
      }

      if (!jaTemPar){
        if (contPares == capPares){ // quer dizer que atingiu o limite
          capPares *= 2; // aumenta a capacidade
          paresUnicos = realloc(paresUnicos, capPares * sizeof(int[2])); // realocando memória
        }
        // 2. salvando o novo par
        paresUnicos[contPares][0] = r.codEstacao;
        paresUnicos[contPares][1] = r.codProxEstacao;
        contPares++;
      }
    }

    escreve_reg_dado_bin(arqBin, &r);

    // soma toda vez que tem um registro novo
    contRRN++;

    free_reg_dado(&r);
  }

  //Após ler todo o arquivo csv, atribui novas alterações para o Registro de Cabeçalho 
  h->status = '1';
  h->nroEstacoes = contEstacoes;
  h->proxRRN = contRRN; 
  h->nroParesEstacoes = contPares;
  
  escreve_reg_cab_bin(arqBin, h);

  fclose(arqCsv);
  fclose(arqBin);

  free_reg_cab(h);
  // dando free na lista de nomes de estações únicas
  for (int i = 0; i < contEstacoes; i++)
    free(estacoes[i]);
  free(estacoes);
  free(paresUnicos);

  BinarioNaTela(nomeArqBin);
}

/* ================================================================================

******************************FUNCIONALIDADE 5************************************

===================================================================================*/ 

/* Funcionalidades: 
  1. abrir os arquivos necessários
  2. escrever o Registro de Cabeçalho do Índice
  3. ler todos os registros de dados e armazenar dados (codEstacao e RRN) em memória primária (para ordenar)
  4. ordenar por codEstacao
  5. escrever os reg dados no arq de índice
  6. chamar o BinarioNaTela
*/

void criar_indice(){
  // 1. abrir os arquivos necessários
  char nomeArqBin[100];
  char nomeArqIndice[100];

  scanf("%s %s", nomeArqBin, nomeArqIndice); // leitura dos inputs do usuário

  // abrir arquivo bin para leitura
  FILE *arqBin = fopen(nomeArqBin, "rb");
  if (arqBin == NULL) {
    printf("Falha no processamento do arquivo.\n");
    return;
  }

  // abrir arquivo de índice para escrita
  FILE *arqInd = fopen(nomeArqIndice, "wb");
  if (arqInd == NULL) {
    printf("Falha no processamento do arquivo.\n");
    return;
  }

  // 2. escrever o Registro de Cabeçalho do Índice
  RegistroCabecalhoIndice *hInd = NULL; // incialização do Registro de cabeçalho de índice
  hInd = (RegistroCabecalhoIndice *) malloc(sizeof(RegistroCabecalhoIndice)); // alocação do registro de cabeçalho
  
  if (hInd == NULL){ // verifica se a alocação não ocorreu
    fclose(arqBin);
    fclose(arqInd);
    printf("Falha no processamento do arquivo.\n");
    return;
  }

  // valor inicial do status deve ser 0
  hInd->status = '0'; // 0 para inconsistente
  escreve_reg_cab_ind(arqInd, hInd);

  // verificar se o arq bin está consistente
  RegistroCabecalho *h = ler_reg_cab_bin(arqBin);
  // dá erro caso o ponteiro seja NULL ou o arquivo esteja inconsistente
  if (h == NULL || h->status == '0') {
    printf("Falha no processamento do arquivo.\n");
    fclose(arqBin);
    fclose(arqInd);
    return;
  }

  // variáveis para auxiliar no armazenamento em mem. primária e ordenação
  int capacidade = 100; // capacidade inicial
  int nRegistros = 0; // quant de registros não removidos
  RegistroDadoIndice *listaIndices = malloc(capacidade * sizeof(RegistroDadoIndice)); // lista dinâmica de reg de dados de índice
  int rrnAtual = 0; 

  if (listaIndices == NULL){ // se n foi alocado corretamente
    fclose(arqBin);
    fclose(arqInd);
    printf("Falha no processamento do arquivo.\n");
    return;
  }
  
  fseek(arqBin, 17, SEEK_SET); // pulando o Reg de Cabeçalho do bin

  // 3. ler todos os registros de dados e armazenar dados (codEstacao e RRN) em memória primária (para ordenar)
  while (check_eof(arqBin)){
    RegistroDado *r = ler_reg_dado_bin(arqBin);

    if (r == NULL) break; // se n tiver memória
      
    if (r->removido == '1'){
      rrnAtual++; // deve incrementar sendo removido ou não
      free_reg_dado(r);
      free(r);
      continue; // os removidos devem ser ignorados
    }
    
    // caso o espaço não seja suficiente, realocar memória
    if (nRegistros >= capacidade){
      capacidade *= 2;
      listaIndices = realloc(listaIndices, capacidade * sizeof(RegistroDadoIndice));
    }

    // se tem espaço, vai armazenar os valores na struct de reg dados de índice
    if (r->codEstacao >= 0 && rrnAtual >= 0){ // n podem ser nulos
      listaIndices[nRegistros].codEstacao = r->codEstacao;
      listaIndices[nRegistros].RRN = rrnAtual;
      nRegistros++;
      rrnAtual++;
    }

    free_reg_dado(r);
    free(r);
  }

  // 4. ordenar por codEstacao
  heap(listaIndices, nRegistros);

  // 5. armazenar no arqIndice
  reescrita(arqInd, listaIndices, nRegistros);

  //deve atualizar o status no final
  hInd->status = '1';
  escreve_reg_cab_ind(arqInd, hInd);

  free(listaIndices);
  free(hInd);
  fclose(arqBin);
  fclose(arqInd);

  // 6. chamar o binário na tela
  BinarioNaTela(nomeArqIndice);
}