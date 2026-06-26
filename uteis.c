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


void BinarioNaTela(char *arquivo) {
    FILE *fs;
    if (arquivo == NULL || !(fs = fopen(arquivo, "rb"))) {
        fprintf(stderr,
                "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
                "não foi possível abrir o arquivo que me passou para leitura. "
                "Ele existe e você tá passando o nome certo? Você lembrou de "
                "fechar ele com fclose depois de usar?\n");
        return;
    }

    fseek(fs, 0, SEEK_END);
    size_t fl = ftell(fs);

    fseek(fs, 0, SEEK_SET);
    unsigned char *mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    unsigned long cs = 0;
    for (unsigned long i = 0; i < fl; i++) {
        cs += (unsigned long)mb[i];
    }

    printf("%lf\n", (cs / (double)100));

    free(mb);
    fclose(fs);
}

////////////////////////////////////////////

void ScanQuoteString(char *str) {
    char R;

    while ((R = getchar()) != EOF && isspace(R))
        ; // ignorar espaços, \r, \n...

    if (R == 'N' || R == 'n') { // campo NULO
        getchar();
        getchar();
        getchar();       // ignorar o "ULO" de NULO.
        strcpy(str, ""); // copia string vazia
    } else if (R == '\"') {
        if (scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar();         // ignorar aspas fechando
    } else if (R != EOF) { // vc tá tentando ler uma string que não tá entre
                           // aspas! Fazer leitura normal %s então, pois deve
                           // ser algum inteiro ou algo assim...
        str[0] = R;
        scanf("%s", &str[1]);
    } else { // EOF
        strcpy(str, "");
    }
}

////////////////////////////////////////////

char check_eof (FILE* arq){
	char ch = 0;
	ch = fgetc(arq);
	fseek(arq, -1, SEEK_CUR);
	return (ch==EOF)?0:1;
}

////////////////////////////////////////////

void imprimir_reg_dado(RegistroDado *r){
  // esses dados nunca podem ser nulos, por isso, não serão tratados
  printf("%d ", r->codEstacao);
  printf("%s ", r->nomeEstacao);

  //Verifica se o campo do Reg. Dados for igual a -1, caso seja deve printar "NULO"
  //Caso não for nulo, possui um valor que deve ser printado em tela. 

  if (r->codLinha == -1) printf("NULO ");
  else printf("%d ", r->codLinha);

  if (strcmp(r->nomeLinha, "") == 0) printf("NULO ");
  else printf("%s ", r->nomeLinha);

  if (r->codProxEstacao == -1) printf("NULO ");
  else printf("%d ", r->codProxEstacao);

  if (r->distProxEstacao == -1) printf("NULO ");
  else printf("%d ", r->distProxEstacao);

  if (r->codLinhaIntegra == -1) printf("NULO ");
  else printf("%d ", r->codLinhaIntegra);

  if (r->codEstIntegra == -1) printf("NULO\n");
  else printf("%d\n", r->codEstIntegra);
}

///////////////////////////////////

void imprimir_reg_cab(RegistroCabecalho *h){
  //Não precisava para o trabalho 1
  //Usado para debugar o código pelas estudantes 
  printf("%c %d %d %d %d\n", h->status, h->topo, h->proxRRN, h->nroEstacoes, h->nroParesEstacoes);
}

//////////////////////////////

int verificar_nulo(char *valor){
  if (strcmp(valor, "NULO") == 0 || strlen(valor) == 0 || valor == NULL) return -1;
  return atoi(valor);
}

////////////////////////////////

int verificar_nulo_fixo(char *string){
  if (strcmp(string, "") == 0){
      return -1;
    } else {
      return atoi(string);
    }
}

////////////////////////////////
// funções para ordenar pelo codEstacao
void heapify (RegistroDadoIndice *array, int p, int u){
  int f = 2 * p + 1; // 1º filho da esq

  if (f <= u){
    // se tiver um 2º filho (f+1), ver qual é o maior
    if (f + 1 <= u){
        // se o da dir for maior que o da esq, atualiza o f para a dir
      if (array[f+1].codEstacao > array[f].codEstacao){
        f++;
      }
    }

    //se o pai for menor, troca de valor com o maior filho
    if (array[p].codEstacao < array[f].codEstacao){ 
        // troca a struct inteira
        RegistroDadoIndice rInd = array[p];
        array[p] = array[f];
        array[f] = rInd;
        heapify(array, f, u); //para garantir que todos sejam heaps novamente
    }
  }
}

void heap (RegistroDadoIndice *array, int n){
  //o i = n / 2 é a história de começar, inicialmente, de trás para frente
  for (int i = n / 2 - 1; i >= 0; i--){
    heapify(array, i, n - 1);
  }

  // ordenação, pega o maior e joga para o fim
  for (int i = n - 1; i > 0; i--){
    RegistroDadoIndice rInd = array[i];
    array[i] = array[0];
    array[0] = rInd;
    heapify(array, 0, i - 1);
  }
}

///////////////////////////////////////////

RegistroDadoIndice *carregamento (char *nomeArqInd, int *nRegistros){
  // abrir arquivo binário para leitura
  FILE *arqInd = fopen(nomeArqInd, "rb");
  if (arqInd == NULL) {
    printf("Falha no processamento do arquivo.\n");
    return NULL;
  }

  // verificando status do reg de cab do índice
  RegistroCabecalhoIndice *hInd = ler_reg_cab_ind(arqInd);
  if (hInd == NULL || hInd->status == '0'){
    printf("Falha no processamento do arquivo.\n");
    if (hInd != NULL) free(hInd);
    fclose(arqInd);
    return NULL;
  }

  free(hInd); // libera pois já verificou

  // criando vetor dinâmico para armazenar os registros
  int capacidade = 100;
  int qtd = 0;
  RegistroDadoIndice *listaRegistros = malloc(capacidade * sizeof(RegistroDadoIndice));
  if (listaRegistros == NULL){ // alocação deu errado
    printf("Falha no processamento do arquivo.\n");
    fclose(arqInd);
    return NULL;
  }
  
  fseek(arqInd, 1, SEEK_SET); // pula cabeçalho

  // ler enquanto não acabou o arquivo
  while(check_eof(arqInd)){
    // se não tiver mais espaço 
    if (qtd >= capacidade){
      capacidade *= 2; // aumentando a capacidade da lista dinâmica
      RegistroDadoIndice *temp = realloc(listaRegistros, capacidade * sizeof(RegistroDadoIndice));
      if (temp == NULL){ // se realloc deu errado
        printf("Falha no processamento do arquivo.\n");
        free(listaRegistros);
        fclose(arqInd);
        return NULL;
      }
      listaRegistros = temp; // se deu certo o realloc
    }

    // tenta ler os dois campos fixos do índice
    if (fread(&listaRegistros[qtd].codEstacao, sizeof(int), 1, arqInd) != 1) break;
    if (fread(&listaRegistros[qtd].RRN, sizeof(int), 1, arqInd) != 1) break;

    qtd++; // incrementa a quant de reg carregados

  }

  fclose(arqInd);

  *nRegistros = qtd; // deixa armazenado a quant de registros

  return listaRegistros;
}

//////////////////////////////////////////

void reescrita(FILE *arqInd, RegistroDadoIndice *listaIndice, int nRegistrosIndice){
  // movendo o ponteiro para depois do cabeçalho
  fseek(arqInd, 1, SEEK_SET);

  for (int i = 0; i < nRegistrosIndice; i++){
      if (listaIndice[i].RRN != -1) escreve_reg_dado_ind(arqInd, &listaIndice[i]);
   }
}

//////////////////////////////////////////

int busca_binaria_indice(RegistroDadoIndice *lista, int n, int codBuscado){
  int ini = 0;
  int fim = n - 1;

  while (ini <= fim){
    int meio = ini + (fim - ini) / 2;

    if (lista[meio].codEstacao == codBuscado){
      return lista[meio].RRN;
    }

    if (lista[meio].codEstacao < codBuscado){
      ini = meio + 1;
    } else {
      fim = meio - 1;
    }
  }

  return -1; // se não achou
}

//////////////////////////////////////////

int busca_binaria_posicao_lista_indice(RegistroDadoIndice *listaIndice, int nRegistrosIndice, int codBuscado) {
    int esq = 0;
    int dir = nRegistrosIndice - 1;

    while (esq <= dir) {
        int meio = esq + (dir - esq) / 2;

        if (listaIndice[meio].codEstacao == codBuscado) {
            return meio; // encontrou a posição exata no vetor
        }

        if (listaIndice[meio].codEstacao < codBuscado) {
            esq = meio + 1;
        } else {
            dir = meio - 1;
        }
    }
    return -1; // caso não encontre, mas deve achar, pois encontrou no arquivo binário
}

///////////////////////////////////////////

int verificacao_filtros(RegistroDado *r, char nomesCampos[100][500], char valoresCampos[100][500], int quantPar){
  // loop para a quantidade de pares que estão armazenados nas arrays
  for (int b = 0; b < quantPar; b++){
    if (strcmp(nomesCampos[b], "codEstacao") == 0){ // se o valor da array corresponde ao nome do campo
      if (verificar_nulo(valoresCampos[b]) != r->codEstacao) return 0; // se o valor da array  não corresponde ao valor armazenado na struct
    }

    // nomeEstacao nçao pode ser nulo
    else if (strcmp(nomesCampos[b], "nomeEstacao") == 0){ // se o valor da array corresponde ao nome do campo
      if (strcmp(valoresCampos[b], r->nomeEstacao) != 0) return 0; // se o valor da array  não corresponde ao valor armazenado na struct
    }

    else if (strcmp(nomesCampos[b], "codLinha") == 0){ // se o valor da array corresponde ao nome do campo
      if (verificar_nulo(valoresCampos[b]) != r->codLinha) return 0; // se o valor da array  não corresponde ao valor armazenado na struct
    }

    else if (strcmp(nomesCampos[b], "nomeLinha") == 0){ // se o valor da array corresponde ao nome do campo
      // se usuário buscou por NULO
      if (strcmp(valoresCampos[b], "") == 0){
        // se o registro tem nome, não serve pois é NULO a busca
        if (r->nomeLinha != NULL) return 0;
      }
      // se buscou por um nome real
      else {
        if (r->nomeLinha == NULL || strcmp(valoresCampos[b], r->nomeLinha) != 0) // se o valor da array  não corresponde ao valor armazenado na struct ou não possui valor
          return 0;
      }
    }

    else if (strcmp(nomesCampos[b], "codProxEstacao") == 0){ // se o valor da array corresponde ao nome do campo
      if (verificar_nulo(valoresCampos[b]) != r->codProxEstacao) return 0; // se o valor da array  não corresponde ao valor armazenado na struct
    }

    else if (strcmp(nomesCampos[b], "distProxEstacao") == 0){ // se o valor da array corresponde ao nome do campo
      if (verificar_nulo(valoresCampos[b]) != r->distProxEstacao) return 0; // se o valor da array  não corresponde ao valor armazenado na struct
    }

    else if (strcmp(nomesCampos[b], "codLinhaIntegra") == 0){ // se o valor da array corresponde ao nome do campo
      if (verificar_nulo(valoresCampos[b]) != r->codLinhaIntegra) return 0; // se o valor da array  não corresponde ao valor armazenado na struct
    }
      
    else if (strcmp(nomesCampos[b], "codEstIntegra") == 0){ // se o valor da array corresponde ao nome do campo
      if (verificar_nulo(valoresCampos[b]) != r->codEstIntegra) return 0; // se o valor da array  não corresponde ao valor armazenado na struct
    }
  }
  return 1; // se nenhum der return 0, quer dizer que achou
}

//////////////////////////////////////////

RegistroCabecalho *abrir_e_validar_arq_bin (char *nomeArqBin, FILE **arqBin, char modo[3]){
  // abrir arquivo binário para o modo desejado
  *arqBin = fopen(nomeArqBin, modo);
  if (*arqBin == NULL) {
    printf("Falha no processamento do arquivo.\n");
    return NULL;
  }

  RegistroCabecalho *h = ler_reg_cab_bin(*arqBin);
  // dá erro caso o ponteiro seja NULL ou o arquivo esteja inconsistente
  if (h == NULL || h->status == '0') {
    printf("Falha no processamento do arquivo.\n");
    if (h != NULL) free_reg_cab(h);
    fclose(*arqBin);
    return NULL;
  }

  return h;
}

/////////////////////////////////////////////////////

RegistroCabecalhoIndice *abrir_e_validar_ind (char *nomeArqInd, FILE **arqInd, char modo[3]){
  // abrir arquivo de índice para o modo desejado
  *arqInd = fopen(nomeArqInd, modo);
  if (*arqInd == NULL) {
    printf("Falha no processamento do arquivo.\n");
    return NULL;
  }

  RegistroCabecalhoIndice *hInd = ler_reg_cab_ind(*arqInd);
  // dá erro caso o ponteiro seja NULL ou o arquivo esteja inconsistente
  if (hInd == NULL || hInd->status == '0') {
    printf("Falha no processamento do arquivo.\n");
    if (hInd != NULL) free(hInd);
    fclose(*arqInd);
    return NULL;
  }

  return hInd;
}

////////////////////////////////////////////////

int existe_nome_estacao(FILE *arqBin, RegistroCabecalho *h, char *nomeProcurado, int rrnIgnorado) {
  if (nomeProcurado == NULL) return 0;

  int posicaoSalva = ftell(arqBin); // salva a posição que tava antes
  fseek(arqBin, 17, SEEK_SET); // pula cabeçalho
  
  for (int i = 0; i < h->proxRRN; i++) { //loop até quant de reg
    RegistroDado *r = ler_reg_dado_bin(arqBin); 
    if (r != NULL) { // se deu certo de pegar reg
      // rrnIgnorado p/ não levar ele em conta na busca
      if (i != rrnIgnorado && r->removido == '0' && r->nomeEstacao != NULL) {
        if (strcmp(r->nomeEstacao, nomeProcurado) == 0) {
          free_reg_dado(r); free(r);
          fseek(arqBin, posicaoSalva, SEEK_SET); // restaura a posição original
          return 1; // tem nome igual
        }
      }
      free_reg_dado(r); free(r);
    }
  }
  fseek(arqBin, posicaoSalva, SEEK_SET); // restaura a posição original
  return 0; // não tem nome igual
}

////////////////////////////////////////////////

int existe_par(FILE *arqBin, RegistroCabecalho *h, int codEstacao, int codProxEstacao, int rrnIgnorado) {
  if (codProxEstacao == -1) return 0;

  int posicaoSalva = ftell(arqBin); // salva a posição que tava antes
  fseek(arqBin, 17, SEEK_SET); // pula cabeçalho

  for (int i = 0; i < h->proxRRN; i++) { //loop até quant de reg
    RegistroDado *r = ler_reg_dado_bin(arqBin);
    if (r != NULL) { // se deu certo de pegar reg
      // rrnIgnorado p/ não levar ele em conta na busca
      if (i != rrnIgnorado && r->removido == '0'){
        if ((r->codEstacao == codEstacao && r->codProxEstacao == codProxEstacao) ||(r->codEstacao == codProxEstacao && r->codProxEstacao == codEstacao)) {
          free_reg_dado(r); free(r);
          fseek(arqBin, posicaoSalva, SEEK_SET); // restaura a posição original
          return 1; // tem nome igual
        }
      }
      free_reg_dado(r); free(r);
    }
  }
  fseek(arqBin, posicaoSalva, SEEK_SET); // restaura a posição original
  return 0; 
}

////////////////////////////////////////////////

int comparar_vertices(const void *a, const void *b) {
  // transforma os ponteiros genéricos em ponteiros de Vertice
  const Vertice *v1 = (const Vertice *)a;
  const Vertice *v2 = (const Vertice *)b;
  
  // compara os nomes das estações alfabeticamente
  return strcmp(v1->nomeEstacao, v2->nomeEstacao);
}

////////////////////////////////////////////////

char *buscar_nome_por_codigo(FILE *arqBin, int codBuscado) {
    // guarda onde o arquivo estava lendo para não estragar o loop principal
    long posAnterior = ftell(arqBin);
    
    // volta para o início dos dados
    fseek(arqBin, 17, SEEK_SET);
    char *nomeEncontrado = NULL; // p/ guardar o nome 

    while (check_eof(arqBin)) {
      RegistroDado *r = ler_reg_dado_bin(arqBin);
      if (r == NULL) break;

      if (r->removido == '0' && r->codEstacao == codBuscado) {
        // aloca memória para retornar o nome
        nomeEncontrado = malloc((strlen(r->nomeEstacao) + 1) * sizeof(char));
        strcpy(nomeEncontrado, r->nomeEstacao);
        free_reg_dado(r);
        free(r);
        break; 
      }
      free_reg_dado(r);
      free(r);
    }

    // Rrestaura a posição original do arquivo
    fseek(arqBin, posAnterior, SEEK_SET);
    return nomeEncontrado;
}

////////////////////////////////////////////////

void inserir_aresta_ordenada(Vertice *v, char *nomeProx, int dist, char *nomeLinha) {
  Aresta *atual = (struct Aresta *) v->inicioLista;
  Aresta *anterior = NULL;

  // verifica se já existe conexão para essa mesma estação de destino
  while (atual != NULL) {
    if (strcmp(atual->nomeProxEst, nomeProx) == 0) {
      // se já existe e a nova linha for diferente, adiciona
      if (strstr(atual->nomesLinha, nomeLinha) == NULL) {
        strcat(atual->nomesLinha, ", ");
        strcat(atual->nomesLinha, nomeLinha);

        ordenar_linhas(atual->nomesLinha);
      }
      return; 
    }
    atual = atual->prox;
  }

  // se for um destino inédito, aloca a nova aresta
  struct Aresta *nova = (struct Aresta *) malloc(sizeof(Aresta));

  // coloca os dados na nova aresta
  strcpy(nova->nomeProxEst, nomeProx);
  nova->distancia = dist;
  strcpy(nova->nomesLinha, nomeLinha);
  nova->prox = NULL;

  // posiciona de forma ordenada e seguindo o alfabeto
  atual = (struct Aresta *) v->inicioLista;
  while (atual != NULL) {
    int comp = strcmp(atual->nomeProxEst, nomeProx);
    if (comp > 0) break; // se for maior alfabeticamente, para
    
    // em caso de empate no nome do destino, desempata pelo nome da linha
    if (comp == 0 && strcmp(atual->nomesLinha, nomeLinha) > 0) break; 

    anterior = atual;
    atual = atual->prox;
  }

  if (anterior == NULL) { // inserção no início
    nova->prox = (struct Aresta *) v->inicioLista;
    v->inicioLista = (Aresta *) nova;
  } else { // inserção no meio ou fim
    nova->prox = atual;
    anterior->prox = nova;
  }
}

////////////////////////////////////////////////

void ordenar_linhas(char *nomesLinha) {
  char cópia[200];
  strcpy(cópia, nomesLinha);

  char *palavras[20]; // pode até 20 linhas integradas na mesma estação
  int qtd = 0;

  // separa a string por vírgulas e espaços
  char *token = strtok(cópia, ", ");
  while (token != NULL) {
    palavras[qtd++] = token;
    token = strtok(NULL, ", ");
  }

  // Bubble Sort das palavras
  for (int i = 0; i < qtd - 1; i++) {
    for (int j = i + 1; j < qtd; j++) {
      if (strcmp(palavras[i], palavras[j]) > 0) {
        char *temp = palavras[i];
        palavras[i] = palavras[j];
        palavras[j] = temp;
      }
    }
  }

  // remonta a string original com as palavras ordenadas e separadas por ,
  nomesLinha[0] = '\0';
  for (int i = 0; i < qtd; i++) {
    strcat(nomesLinha, palavras[i]);
    if (i < qtd - 1) {
      strcat(nomesLinha, ", ");
    }
  }
}