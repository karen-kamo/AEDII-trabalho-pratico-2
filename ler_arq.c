/*
Karen Nanamy Kamo - NUSP: 15495932 
Rebeca de Oliveira Silva - NUSP: 11963923
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ler_arq.h"
#include "structs.h"
#include "uteis.h"


// reaproveitamento da função disponibilizada pelos monitores
static char *strsep_n (char **stringp, const char *delim){
  char *begin, *end;
  begin = *stringp;
  if (begin == NULL) return NULL;
  end = begin + strcspn (begin, delim);
  if (*end) {
      *end++ = '\0';
      *stringp = end;
    }
  else *stringp = NULL;
  return begin;
}


/*!
  * @brief Lê uma token que é uma string e retorna esta string.
  *
  * @param[in,out] token_string Token string que contem a string que deve ser resgatada. Avança a string para o strsep.
  *
  * @param delim Delimitador para secionar a string.
  *
  * @return Retorna a string resgatada. Ela é alocada dinâmicamente.
 */
static char *parse_str_token(char** token_string, const char* delim);

static char *parse_str_token(char** token_string, const char* delim) {
    char* str_contents = strsep_n(token_string, delim);
    unsigned long str_size = strlen(str_contents);
    char* str = calloc(str_size + 1, sizeof(char));
    if (str == NULL) exit(1);
    memcpy(str, str_contents, str_size);

    return str;
}


RegistroDado ler_reg_dado_csv(FILE* arq) {
    char* bf = calloc(100, sizeof(char));
    if (!bf) exit(1);
    char* bf_c = bf;
    fgets(bf, 100, arq);
    bf[strcspn(bf, "\r\n")] = '\0';

    //Conversão de string para inteiro em todos os campos, exceto os campos variáveis
    //Separa cada campo por vírgulas

    // valor codEstacao não pode ser nulo
    char *codEstacao = strsep_n(&bf_c, ",");  // armazena a string lida do csv 
    int codEstacao_valor;
    if (codEstacao != NULL || codEstacao[0] != '\0') { // se não tiver vazio
      codEstacao_valor = atoi(codEstacao); // faz conversão de string para int
    }
    
    //Tratamento para campo de tamanho variável
    char *nomeEstacao = parse_str_token(&bf_c, ","); // armazena a string lida do csv
    // inicializa os valores para caso não tenha valor
    char *nomeEstacao_valor = NULL;
    int tamNomeEstacao_valor = 0;
    if (strcmp(nomeEstacao, "") != 0){ // se não for vazio
      nomeEstacao_valor = strdup(nomeEstacao); // copia o nome
      tamNomeEstacao_valor = (int)strlen(nomeEstacao); // armazena o tamanho da string
    }
    free(nomeEstacao); // desalocar no que foi dado no parse_str_token

    char *codLinha = strsep_n(&bf_c, ","); // armazena a string lida do csv
    int codLinha_valor = verificar_nulo_fixo(codLinha); // se for vazio coloca -1, se não converte o valor para int
    
    //Tratamento para campo de tamanho variável
    char *nomeLinha = parse_str_token(&bf_c, ","); // armazena a string lida do csv
    // inicializa os valores para caso não tenha valor
    char *nomeLinha_valor = NULL;
    int tamNomeLinha_valor = 0;
    if (strcmp(nomeLinha, "") != 0){ // se não for vazio
      nomeLinha_valor = strdup(nomeLinha); // copia o nome
      tamNomeLinha_valor = (int)strlen(nomeLinha); // armazena o tamanho da string
    }
    free(nomeLinha); // desalocar no que foi dado no parse_str_token

    char *codProxEstacao = strsep_n(&bf_c, ","); // armazena a string lida do csv
    int codProxEstacao_valor = verificar_nulo_fixo(codProxEstacao); // se for vazio coloca -1, se não converte o valor para int

    char *distProxEstacao = strsep_n(&bf_c, ","); // armazena a string lida do csv
    int distProxEstacao_valor = verificar_nulo_fixo(distProxEstacao); // se for vazio coloca -1, se não converte o valor para int

    char *codLinhaIntegra = strsep_n(&bf_c, ","); // armazena a string lida do csv
    int codLinhaIntegra_valor = verificar_nulo_fixo(codLinhaIntegra); // se for vazio coloca -1, se não converte o valor para int

    char *codEstIntegra = strsep_n(&bf_c, ","); // armazena a string lida do csv
    int codEstIntegra_valor = verificar_nulo_fixo(codEstIntegra); // se for vazio coloca -1, se não converte o valor para int

    free(bf);

    // retornar a struct com os valores lidos do csv
    return (RegistroDado){.removido = '0', .proximo = -1, .codEstacao = codEstacao_valor, .codLinha = codLinha_valor, .codProxEstacao = codProxEstacao_valor, .distProxEstacao = distProxEstacao_valor, .codLinhaIntegra = codLinhaIntegra_valor, .codEstIntegra = codEstIntegra_valor, .tamNomeEstacao = tamNomeEstacao_valor, .nomeEstacao = nomeEstacao_valor, .tamNomeLinha = tamNomeLinha_valor, .nomeLinha = nomeLinha_valor};
}


RegistroDado *ler_reg_dado_bin(FILE* arqBin){
  RegistroDado *r = NULL; // incialização do Registro de dado

  // aloca memória para o registro de dado
  r = (RegistroDado*) malloc(sizeof(RegistroDado));

  //verifica se a alocação ocorreu corretamente
  if (r == NULL) return NULL;

  // inicializando os ponteiros para os de tamanho variável
  r->nomeEstacao = NULL;
  r->nomeLinha = NULL;

  // Verificar se o registro é logicamente removido primeiro
  fread(&r->removido, sizeof(char), 1, arqBin);
  if (r->removido == '1'){
    // pula para 79, pois o primeiro já foi lido
    fseek(arqBin, 79, SEEK_CUR);

    // vai retornar só indicando que está removido
    // tava devolvendo como NULL, mas tava dando conflito com outras funções
    return r;
  }
  
  // se não é logicamente removido
  //Serve para ler a quantidade de bytes do Registro de Dados, que é 80.
  int bytes_lidos = 0;

  //Lendo cada campo dos registros de dados
  //Armazenando na struct
  fread(&r->proximo, sizeof(int), 1, arqBin);
  fread(&r->codEstacao, sizeof(int), 1, arqBin);
  fread(&r->codLinha, sizeof(int), 1, arqBin);
  fread(&r->codProxEstacao, sizeof(int), 1, arqBin);
  fread(&r->distProxEstacao, sizeof(int), 1, arqBin);
  fread(&r->codLinhaIntegra, sizeof(int), 1, arqBin);
  fread(&r->codEstIntegra, sizeof(int), 1, arqBin);

  //Lê e verifica se o campo do nome não é vazio. 
  fread(&r->tamNomeEstacao, sizeof(int), 1, arqBin);
  if (r->tamNomeEstacao > 0){
    // como o nomeEstacao é um ponteiro na struct, é preciso alocar espaço primeiro antes de ler
    r->nomeEstacao = (char*) malloc((size_t)r->tamNomeEstacao + 1);
    fread(r->nomeEstacao, sizeof(char), (size_t)r->tamNomeEstacao, arqBin);
  } 

  //Lê e verifica se o campo do nome não é vazio. 
  fread(&r->tamNomeLinha, sizeof(int), 1, arqBin);
  if (r->tamNomeLinha > 0){
    // como o nomeLinha é um ponteiro na struct, é preciso alocar espaço primeiro antes de ler
    //Alocação antes, pois ao debugar estava dando erro sem o malloc
    r->nomeLinha = (char*) malloc((size_t)r->tamNomeLinha + 1);
    fread(r->nomeLinha, sizeof(char), (size_t)r->tamNomeLinha, arqBin);
  } else {
    r->nomeLinha = NULL;
  }

  // quantidade de bytes dos valores fixos (int) + removido(char) + proximo (int) + bytes dos campos variáveis
  bytes_lidos = 37 + r->tamNomeEstacao + r->tamNomeLinha;

  // se o registro que lemos tem menos de 80 bytes, então pula para o próximo
  int lixo = 80 - bytes_lidos;
  if (lixo > 0){
    fseek(arqBin, lixo, SEEK_CUR);
  }

  // adicionando \0 nas strings para não imprimir lixo
  if (r->tamNomeEstacao > 0) r->nomeEstacao[r->tamNomeEstacao] = '\0';
  if (r->tamNomeLinha > 0) r->nomeLinha[r->tamNomeLinha] = '\0';

  return r;
}


RegistroCabecalho *ler_reg_cab_bin(FILE* arqBin){
  RegistroCabecalho *h = NULL; // incialização do Registro de cabeçalho

  // aloca memória para o registro de cabeçalho
  h = (RegistroCabecalho*) malloc(sizeof(RegistroCabecalho));

  //verifica se a alocação ocorreu corretamente
  if (h == NULL) return NULL;

  //Lendo cada campo do registro de cabeçalho
  //Armazenando na struct
  fread(&h->status, sizeof(char), 1, arqBin);
  fread(&h->topo, sizeof(int), 1, arqBin);
  fread(&h->proxRRN, sizeof(int), 1, arqBin);
  fread(&h->nroEstacoes, sizeof(int), 1, arqBin);
  fread(&h->nroParesEstacoes, sizeof(int), 1, arqBin);

  return h;
}

RegistroCabecalhoIndice *ler_reg_cab_ind(FILE* arqInd){
  RegistroCabecalhoIndice *hInd = NULL; // incialização do Registro de cab do índice

  // aloca memória para o registro de cabeçalho
  hInd = (RegistroCabecalhoIndice*) malloc(sizeof(RegistroCabecalhoIndice));

  //verifica se a alocação ocorreu corretamente
  if (hInd == NULL) return NULL;

  //Lendo cada campo do registro de cabeçalho
  //Armazenando na struct
  fread(&hInd->status, sizeof(char), 1, arqInd);

  return hInd;
}


