#ifndef _BEMAIS_
#define _BEMAIS_
#include "bemais.h"
using namespace std;

/*
  keys minimas = (ordem-1)/2
  filhos minimos = (ordem+1)/2
*/

Hash hashFunction(char *str) { //funcao hash vc djb2
  Hash hash = 5381;
  int c;
  while ((c = *(str++))) { hash = ((hash << 5) + hash) + c; }
  return hash;
}

//leh uma string dada, e separa o atributo desejado, retornando o hash do mesmo
Hash leituraLinha(int nChar, int atributo, char linha[MAXLINHA]){
  int tamanho, i, j, virgula;
  char  aux[nChar+1];
  Hash hash = 0;
  
  virgula = atributo - 1;
  linha[strlen(linha)-1] = '\0';
  //Encontra o atributo desejado
  for (i = 0, tamanho = strlen(linha); i < tamanho && virgula; i++)
    if (linha[i] == ',') virgula--;
  //Copia o atributo para uma string auxiliar para gerar o hash
  for (j = 0; j < nChar && linha[i] != '\0' && linha[i] != '\n' && linha[i] != ','; i++)
    if (linha[i] != '"') aux[j++] = linha[i];
  aux[j] = '\0';
  
  //Pega o valor do HASH
  hash = hashFunction(aux);
  
  return hash;
}

//Faz a leitura de um arquivo inteiro, gerando o hash dos atributos, e guardando em um vector
void leituraArquivo(vind &indices, int nChar, int atributo, FILE *entrada){
  char linha[MAXLINHA];
  Hash hash = 0;
  Offset offsetAux;

  //Faz a leitura de todas as linhas do arquivo, e gera o hash do atributo desejado
  while (offsetAux = ftell(entrada), fgets(linha, MAXLINHA, entrada)) {
    hash = leituraLinha(nChar,atributo,linha);
    //empurra no vetor
    indices.push_back(index_t(hash, offsetAux));
  }

  //ordena de acordo com as hashs
  sort(indices.begin(), indices.end(), compareIndex);
}

nodo_t* insercaoElemento(nodo_t* &arvore,char linha[MAXLINHA], int ordem, int nChar, int atributo){
  Hash valor = leituraLinha(nChar,atributo,linha);

  printf("%lld\n",valor);
  
  //Quando a raiz eh null
  if(arvore == NULL){
    arvore = criaNodo(ordem, true);
    arvore->quantidadeKeys = 1;
    arvore->keys[0] = valor;
    return arvore;
  }
  //Quando tem espaco para inserir na raiz
  else if((arvore->pai == NULL) && arvore->quantidadeKeys < ordem-1 && arvore->folha){
    //TODO
    //arrumar ordenacao dos offsets
    arvore->quantidadeKeys++;
    arvore->keys[(arvore->quantidadeKeys)-1] = valor;
    sort(arvore->keys,(arvore->keys+arvore->quantidadeKeys));
    return arvore;
  }
  //Split na raiz, quando nao tem mais espaco para inserir
  else if((arvore->pai == NULL) && arvore->quantidadeKeys >= ordem-1 && arvore->folha){
   return splitInsercao(arvore, valor,ordem,NULL);
  }
  else{
    //Italian
    nodo_t *nodoInserimento = buscaInsercao(arvore,valor);
    
    
    return arvore;
  }
  
  return 0;
}

nodo_t* splitInsercao(nodo_t* &nodoInserimento,Hash valor, int ordem, nodo_t *filho){
  nodo_t *filhoDir, *pai, *paiAux;
  int count = 0, flag = 1;

  if(nodoInserimento->quantidadeKeys < ordem-1){
    nodoInserimento = sortMiracolosoInsercione(nodoInserimento, valor, ordem,filho);
    return nodoInserimento;
  }
  else{
    filhoDir = criaNodo(ordem,true);
    pai = criaNodo(ordem,false);
    paiAux = nodoInserimento->pai;
	
    nodoInserimento = sortMiracolosoInsercione(nodoInserimento, valor, ordem,filho);
    
    //Separa as polentas do nene
    nodoInserimento->quantidadeKeys = count =((ordem/2));

    while(count < ordem){
      filhoDir->quantidadeKeys++;
      filhoDir->keys[(filhoDir->quantidadeKeys)-1] = nodoInserimento->keys[count];
      count++;
    }

    pai->quantidadeKeys++;
    pai->keys[0] = filhoDir->keys[0];
      
    filhoDir->pai = pai;
    nodoInserimento->pai = pai;
    nodoInserimento->prox = filhoDir;
      
    pai->quantidadeFilhos = 2;
    pai->filhos[0] = nodoInserimento;
    pai->filhos[1] = filhoDir; 
      
    if(pai->pai != NULL)
      return splitInsercao(pai->pai,pai->keys[0],ordem,pai->filhos[1]);
    return pai;
  }
}

nodo_t* sortMiracolosoInsercione(nodo_t* &nodoInserimento, Hash valor, int ordem, nodo_t *filho){
  int count = 0 , flag = 1;
  
  if(nodoInserimento->folha){
    nodoInserimento->quantidadeKeys++;
    nodoInserimento->keys[(nodoInserimento->quantidadeKeys)-1] = valor;
    sort(nodoInserimento->keys,(nodoInserimento->keys+nodoInserimento->quantidadeKeys));
  }
  else{
    Hash hashAuxiliar[nodoInserimento->quantidadeKeys];
    nodo_t **filhosAux = (nodo_t**)malloc(sizeof(nodo_t*) * ordem);
    int countAux = 0, condicaoInsert, condicaoCopy;

    //Copia as keys para um vetor de hash auxiliar
    count = 0; 
    while(count < nodoInserimento->quantidadeKeys){
      hashAuxiliar[count] = nodoInserimento->keys[count];
      count++;
    }

    //Copia os filhos para um vetor de filhos auxiliar
    count = 0;
    while(count < nodoInserimento->quantidadeFilhos){
      filhosAux[count] = nodoInserimento->filhos[count];
      count++;
    }

    //Encontra a posicao de insercao do valor na nodo atual
    count = 0;
    while(count < nodoInserimento->quantidadeKeys && flag)
      if(nodoInserimento->keys[count] > valor) flag = 0;
	  
    //Insere de forma ordena o valor na nodo atual
    countAux = 0;

    nodoInserimento->quantidadeKeys++;
    while(countAux < nodoInserimento->quantidadeKeys){
      if(countAux == count)
	nodoInserimento->keys[count] = valor;
      else if (countAux >= count)
	nodoInserimento->keys[countAux] = hashAuxiliar[countAux-1];
      countAux++;
    }

    //insere o filho de forma ordenada no nodo atual
    countAux = 0;
    nodoInserimento->quantidadeFilhos++;

    condicaoInsert = (count != 0);
    condicaoCopy = (count == 0);

    while(countAux < nodoInserimento->quantidadeFilhos){
      //PorcaMadona
      if(countAux == (count+condicaoInsert))
	nodoInserimento->filhos[countAux] = filho;
      else if(countAux > (count+(condicaoInsert)))
	nodoInserimento->filhos[countAux-(condicaoCopy)] = filhosAux[countAux];
      countAux++;
    }
  }
  return nodoInserimento;
}
nodo_t* buscaInsercao(nodo_t* &nodoAtual, Hash valor){
  if(nodoAtual->folha) return nodoAtual;
 
 
  int count = 0, flag = 0;
  
  while(count < nodoAtual->quantidadeKeys && !flag){
    if(nodoAtual->keys[count] >= valor) flag = 1;
    else count++;
  }
  
  if(flag = 1) return buscaInsercao(nodoAtual->filhos[count],valor);
  else return buscaInsercao(nodoAtual->filhos[nodoAtual->quantidadeFilhos-1],valor);
 
}

FILE* abrirArquivo(char arquivoEntrada[]){
  FILE *entrada;
  entrada = fopen(arquivoEntrada, "a+"); //abre o arquivo de entrada que vai ser passado de parametro quando executar o programa
  if (!entrada) { printf("Não abriu entrada\n"); return NULL; }
  fseek(entrada, 0, SEEK_SET); //retorna o buffer pro começo (so pra garantir)
  return entrada;
}

bool compareIndex(const index_t &_a, const index_t &_b) {
  return _a.hash < _b.hash;
}

nodo_t* trataExcecoes(nodo_t* paiAtual, nodo_t *filhoAtual, int ordem) {
  //excecão: único filho tem menos entrada que o mínimo da ordem
  if (!paiAtual->pai && paiAtual->quantidadeFilhos == 1 && filhoAtual) {
    filhoAtual->pai = NULL;
    paiAtual->quantidadeFilhos = 0;
    mataArvore(paiAtual);
    return filhoAtual;
  }
  //excecão: último filho tem menos que a ordem ↓
  if (filhoAtual && filhoAtual->quantidadeKeys < (ordem-1)/2) {
    nodo_t *familiarBeneficiario = NULL;
    int i, j;
    
    //acha o parente que vai receber as keys e os offsets
    if (paiAtual->quantidadeFilhos > 1) familiarBeneficiario = paiAtual->filhos[paiAtual->quantidadeFilhos-2];
    else { //else: pega do primo
      nodo_t *voAtual = paiAtual->pai;
      nodo_t *tioAtual = voAtual->filhos[voAtual->quantidadeFilhos-2];
      familiarBeneficiario = tioAtual->filhos[tioAtual->quantidadeFilhos-1];
    }
    for (i = familiarBeneficiario->quantidadeKeys, j = 0; j < filhoAtual->quantidadeKeys; i++, j++) {
      familiarBeneficiario->keys[i] = filhoAtual->keys[j];
      familiarBeneficiario->offsets[i] = filhoAtual->offsets[j];
      familiarBeneficiario->quantidadeKeys++;
    }
    filhoAtual->quantidadeKeys = 0; //coloca 0 se nao o mataArvore vai apagar todos os offsets que agora estao no familiarBeneficiario
    //paiAtual->quantidadeFilhos--;
    //paiAtual->quantidadeKeys--;
    mataArvore(filhoAtual);
    filhoAtual = familiarBeneficiario;
    removeUltimo(paiAtual, ordem); //remove um de todos os ancestrais do antigo filhoAtual
  }
  return filhoAtual;
}

int bulk_loading(nodo_t* &arvore, vind &indices, int ordem){
  nodo_t *filhoAtual = NULL, *paiAtual = NULL;
  offsets_t *novo;
  int iteradorIndices = 0, first = 1, condicaoParaFor = (ordem-1)/2;
  
  //cria o primeiro pai
  paiAtual = criaNodo(ordem, false);
  if (!paiAtual) { printf("Erro criando o primeiro pai\n"); return 1; }
  
  while (iteradorIndices < (int)indices.size()) {
    filhoAtual = criaNodo(ordem, true);

    //preenche o nodo filhoAtual, que é o filho do paiAtual
    for(int j = 0; j <= condicaoParaFor && iteradorIndices < (int)indices.size(); iteradorIndices++, j++){
      //se o hash a ser inserido for o mesmo que o anterior, ele coloca o offset no mesmo
      if (j && filhoAtual->keys[j-1] == indices[iteradorIndices].hash) { j--; }
      else if (j == condicaoParaFor && indices[iteradorIndices].hash != filhoAtual->keys[j-1]) { break; }
      else {
	filhoAtual->keys[j] = indices[iteradorIndices].hash;
	filhoAtual->quantidadeKeys++;
      }
        
      //cria novo offset, passando como parametro o offset da hash atual e ligando o novo offset no começo da lista
      novo = NULL;
      novo = criaOffset(indices[iteradorIndices].offset, filhoAtual->offsets[j]);
      if (!novo) { printf("Erro ao criar offset %lld", indices[iteradorIndices].offset); return 1; }
      filhoAtual->offsets[j] = novo;
    }
      
    if (first) { //primeiro caso/folha
      first = 0;
      paiAtual->filhos[0] = filhoAtual;
      filhoAtual->pai = paiAtual;
      paiAtual->quantidadeFilhos = 1;
    }
    else if (checaPai(filhoAtual, &paiAtual, filhoAtual->keys[0], ordem)) { return 1; }
  }

  filhoAtual = trataExcecoes(paiAtual, filhoAtual, ordem);
  
  //atualiza a raiz
  while (filhoAtual->pai != NULL) filhoAtual = filhoAtual->pai;
  arvore = filhoAtual;
  return 0;
}

int checaPai(nodo_t *filhoAtual, nodo_t** pAtual, Hash hashQueVem, int ordem) { //checa se precisa trocar o pai atual
  nodo_t *tioAtual = NULL, *voAtual = (*pAtual)->pai, *paiAtual = *pAtual;
  Hash hashQueSobe;
  int i, j;
  
  if (paiAtual->quantidadeFilhos >= ordem) {
    tioAtual = criaNodo(ordem, false);
    if (!tioAtual) return 1; //retorna erro se nao criou
    
    //pega hash que sobe
    hashQueSobe = paiAtual->keys[(ordem-1)/2];
    
    //divide paiAtual com tioAtual
    for (i = (ordem + 1) / 2, j = 0; i < ordem - 1; i++, j++) {
      tioAtual->keys[j] = paiAtual->keys[i];
      tioAtual->filhos[j] = paiAtual->filhos[i];
      tioAtual->filhos[j]->pai = tioAtual;
    }
    tioAtual->filhos[j] = paiAtual->filhos[i];
    tioAtual->filhos[j]->pai = tioAtual;
    //coloca novos numeros
    tioAtual->quantidadeKeys = ordem / 2 - 1;
    tioAtual->quantidadeFilhos = tioAtual->quantidadeKeys + 1;
    paiAtual->quantidadeKeys = (ordem-1)/2;
    paiAtual->quantidadeFilhos = (ordem+1)/2;
    
    if (!paiAtual->pai) {
      voAtual = criaNodo(ordem, false);
      paiAtual->pai = voAtual;
      voAtual->filhos[0] = paiAtual;
      voAtual->quantidadeFilhos = 1;
    }
    checaPai(tioAtual, &voAtual, hashQueSobe, ordem);
    paiAtual = tioAtual;
  }

  //coloca hashQueVem no final
  paiAtual->keys[paiAtual->quantidadeKeys++] = hashQueVem;
  paiAtual->filhos[paiAtual->quantidadeFilhos++] = filhoAtual;
  filhoAtual->pai = paiAtual;
  *pAtual = paiAtual;
  return 0;
}

nodo_t* criaNodo(int ordem, bool folha){
  nodo_t *nodo = NULL;
  
  nodo = (nodo_t*)malloc(sizeof(nodo_t));
  if (!nodo) { printf("Erro na criaNodo\n"); return NULL; }
  
  if(!folha) {
    nodo->filhos = NULL;
    nodo->filhos = (nodo_t**)malloc( sizeof(nodo_t*) * ordem );
    if (!nodo->filhos) { printf("Erro inicializando vetor dos filhos\n"); return NULL; }
    nodo->offsets = NULL;
  }
  else {
    nodo->offsets = NULL;
    nodo->offsets = (offsets_t**)malloc(sizeof(offsets_t*)*(ordem-1));
    if (!nodo->offsets) { printf("Erro inicializando vetor de offsets\n"); return NULL; }
    nodo->filhos = NULL;
    memset(nodo->offsets, 0, sizeof(offsets_t*)*(ordem-1));
  }
  nodo->keys = NULL;
  nodo->keys = (Hash*)malloc(sizeof(Hash)*(ordem-1));
  if (!nodo->keys) { printf("Erro inicializando vetor das chaves\n"); return NULL; }
  nodo->quantidadeKeys = nodo->quantidadeFilhos = 0;
  nodo->prox = nodo->pai = NULL;
  nodo->folha = folha;
  return nodo;

}

offsets_t* criaOffset(Offset o, offsets_t *p) {
  offsets_t *r = NULL;
  r = (offsets_t*)malloc(sizeof(offsets_t));
  r->offset = o;
  r->prox = p;
  return r;
}

void mataArvore(nodo_t *n) {
  if (!n) return;
  for (int i = 0; n->folha && i < n->quantidadeKeys; i++) {
    if (n->offsets[i]) mataOffsets(n->offsets[i]->prox);
  }
  
  for (int i = 0; !n->folha && i < n->quantidadeFilhos; i++) {
    mataArvore(n->filhos[i]);
  }

  if (n->filhos) free(n->filhos);
  if (n->offsets) free(n->offsets);
  free(n->keys);
}

void mataOffsets(offsets_t *o) {
  if (!o) return;
  mataOffsets(o->prox);
  free(o);
}

int imprimeArvore(nodo_t *arvore) {
  FILE *dotFile = NULL;
  char nomeArquivo[] = "saida.dot", comando[400];
  int numeroNodo = 0;
  
  //cria o comando
  sprintf(comando, "dot %s -Tpng -o saida.png && display saida.png\n", nomeArquivo);

  //abre dotFile
  dotFile = fopen(nomeArquivo, "w");
  if (!dotFile) { printf("Erro abrindo %s\n", nomeArquivo); return 1; }
  fprintf(dotFile, "graph {\n");
  imprimeNodos(dotFile, arvore, &numeroNodo, 0);
  fprintf(dotFile, "}\n");
  fclose(dotFile);
  system(comando);
  return 0; 
}

void imprimeNodos(FILE *dotFile, nodo_t *n, int *numeroNodo, int liga) {
  int esseNumero = *numeroNodo;
  if (!n) return;
  fprintf(dotFile, "%d [label=%d];\n", (*numeroNodo)++, liga);
  for (int i = 0; i < n->quantidadeFilhos; i++) {
    fprintf(dotFile, "%d -- ", esseNumero);
    (*numeroNodo)++;
    imprimeNodos(dotFile, n->filhos[i], numeroNodo, i);
  }
  
  //imprime esse nodo
  fprintf(dotFile, "%d [label=\"", esseNumero);
  for (int i = 0; i < n->quantidadeKeys; i++)
    fprintf(dotFile, "%s %lld", i? " »":"", n->keys[i]);
  fprintf(dotFile, "\"];\n");
  fprintf(dotFile, "%d [shape=box]\n", esseNumero);
}

void removeUltimo(nodo_t *paiAtual, int ordem) {
  if (paiAtual->quantidadeKeys > (ordem-1)/2 || paiAtual->pai == NULL) { //se ele tem suficiente ou é raiz
    paiAtual->quantidadeKeys--;
    paiAtual->quantidadeFilhos--;
    if (paiAtual->quantidadeKeys <= 0) {
      paiAtual->filhos[0]->pai = NULL;
      mataArvore(paiAtual);
    }
    return;
  }
  nodo_t *tioAtual, *voAtual;
  int i, j;
  voAtual = paiAtual->pai;
  tioAtual = voAtual->filhos[--voAtual->quantidadeFilhos - 1];
  
  //abaixa uma hash pro tio
  tioAtual->keys[tioAtual->quantidadeKeys++] = voAtual->keys[--voAtual->quantidadeKeys];
  printf("%d]", tioAtual->quantidadeKeys);
  
  for (i = tioAtual->quantidadeKeys, j = 0; j < paiAtual->quantidadeKeys; j++, i++) {
    paiAtual->filhos[j]->pai = tioAtual;
    tioAtual->keys[i] = paiAtual->keys[j];
    tioAtual->filhos[i] = paiAtual->filhos[j];
    tioAtual->quantidadeKeys++;
    tioAtual->quantidadeFilhos++;
  }
  tioAtual->filhos[i] = paiAtual->filhos[j];
  paiAtual->filhos[j]->pai = tioAtual;
  tioAtual->quantidadeFilhos++;
  
  paiAtual->quantidadeFilhos = paiAtual->quantidadeKeys = 0;
  mataArvore(paiAtual);
  removeUltimo(voAtual, ordem);
      
}

int bbin(nodo_t *nodoAtual, Hash numero) {
  int li = 0, ls = nodoAtual->quantidadeKeys;
  while (li < ls) {
    int meio = (li + ls) / 2;
    if (nodoAtual->keys[meio] < numero) li = meio + 1;
    else ls = meio;
  }
  return ls + (!nodoAtual->folha && nodoAtual->keys[ls] == numero);
}

nodo_t *achaElemento(nodo_t* noAtual, int &indice, Hash procurando) {
  int ind = bbin(noAtual, procurando);
  if (noAtual->keys[ind] == procurando && noAtual->folha) { indice = ind; return noAtual; }
  if (!noAtual->folha) return achaElemento(noAtual->filhos[ind], indice, procurando);
  return NULL;
}

void imprimeTupla(nodo_t *nodoAtual, int indiceElemento, FILE *arquivo) {
  offsets_t *o;
  char tupla[MAXLINHA];
  if (!nodoAtual || !arquivo || indiceElemento < 0) return;
  for (o = nodoAtual->offsets[indiceElemento]; o; o = o->prox) {
    fseek(arquivo, o->offset, SEEK_SET);
    fgets(tupla, MAXLINHA, arquivo);
    if (tupla[strlen(tupla)-1] == '\n') tupla[strlen(tupla)-1] = '\0';
    printf("%s\n", tupla);
  }
}

void imprimeMenu(){
  printf("Digite a opção desejada\n");
  printf("0• Sair\n");
  printf("1• Imprimir a árvore\n");
  printf("2• Buscar\n");
  printf("3• Inserir elemento\n");
}

#endif
