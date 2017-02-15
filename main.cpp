#include <cstdio>
#include <cstdlib>
#include "bemais.h"

int main(int argc, char *argv[]) {
  int nChar, atributo, ordem, opMenu = 1, indiceBusca;
  char nomeArquivo[200], linha[MAXLINHA];
  FILE *entrada = NULL;
  vind indices;
  nodo_t *arvore = NULL, *nodoDeBusca;
  
  //pega a quantidade de caracteres que farao parte do indice, a ordem da arvore, o atributo que sera levado em conta e o nome do arquivo
  atributo = atoi(argv[1]);
  nChar = atoi(argv[2]); 
  ordem = atoi(argv[3]);

  //Verifica se foi indicado uma base de dados
  if(argv[4] != NULL) {
    strcpy(nomeArquivo,argv[4]);
    
    //abre o arquivo de entrada
    entrada = abrirArquivo(nomeArquivo); //passa o nome do arquivo
    if (!entrada) return 0;
    
    //le as entrada o coloca os offsets das tuplas no vetor de indices
    leituraArquivo(indices, nChar, atributo, entrada);
    
    //realiza o bulkload que retornara 0 no sucesso
	if(indices.size())
	  if (bulk_loading(arvore, indices, ordem)) return 0;
    
    for (int i = 0; i < (int)indices.size(); i++)
      printf("%d: %llu\n", i, indices[i].hash);
  }
  
  while (opMenu) {
    imprimeMenu();
    scanf("%d", &opMenu);
    getchar();
    switch (opMenu) {
    case 0: break;
    case 1:
      imprimeArvore(arvore);
      break;
    case 3:
      printf("Digite o texto a ser buscado\n");
      fgets(linha, MAXLINHA, stdin);
      linha[strlen(linha)-1] = '\0';
      linha[nChar] = '\0';
      printf("Você buscou: %s\n", linha);
      nodoDeBusca = achaElemento(arvore, indiceBusca, hashFunction(linha));
      if (!nodoDeBusca) printf("Linha não encontrada\n");
      else {
		printf("\nResultados encontrados:\n");
		imprimeTupla(nodoDeBusca, indiceBusca, entrada);
		putchar('\n');
      }
      break;
    case 2:
      printf("Digite o dado a ser inserido:\n");
      fgets(linha, MAXLINHA,stdin);
      arvore = insercioneElemento(arvore,linha,ordem,nChar,atributo,nomeArquivo);
	  break;
	default:
	  printf("Opcao Invalida..\n");
	  opMenu = 4;
	  break;
	}	
  }  
  //mataArvore(arvore);
  
  //Verifica se o arquivo jah foi aberto
  if(entrada != NULL)  fclose(entrada);
  return 0;
}
