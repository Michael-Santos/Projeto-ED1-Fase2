/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 02 - Árvore B
 *
 * RA:
 * Aluno:
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 	11
#define TAM_NOME 			51
#define TAM_DESENVOLVEDORA 	51
#define TAM_DATA 			11
#define TAM_CLASSIFICACAO 	3
#define TAM_PRECO 			8
#define TAM_DESCONTO 		4
#define TAM_CATEGORIA 		51
#define TAM_STRING_INDICE 	(TAM_DESENVOLVEDORA + TAM_NOME)


#define TAM_REGISTRO 		192
#define MAX_REGISTROS 		1000
#define MAX_ORDEM 			150
#define TAM_ARQUIVO 		(MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas do usuário */
#define OPCAO_INVALIDA 			 "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE	 "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO 	 "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 			 "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA 		 "ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 			 "Arquivo vazio!"
#define INICIO_BUSCA			 "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM 		 "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO		 "********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO			 "********************************ARQUIVO*******************************\n"
#define INICIO_INDICE_PRIMARIO	 "***************************INDICE PRIMÁRIO****************************\n"
#define INICIO_INDICE_SECUNDARIO "***************************INDICE SECUNDÁRIO**************************\n"
#define SUCESSO  				 "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 "FALHA AO REALIZAR OPERACAO!\n"
#define NOS_PERCORRIDOS_IP 		 "Busca por %s. Nos percorridos:\n"
#define NOS_PERCORRIDOS_IS 		 "Busca por %s.\nNos percorridos:\n"


/* Registro do jogo */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char desenvolvedora[TAM_DESENVOLVEDORA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char classificacao[TAM_CLASSIFICACAO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Jogo;


/*Estrutura da chave de um nó do Índice Primário*/
typedef struct Chaveip
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave_ip;

/*Estrutura da chave de um  do Índice Secundário*/
typedef struct Chaveis
{
	char string[TAM_STRING_INDICE];
	char pk[TAM_PRIMARY_KEY];
} Chave_is;

/* Estrutura das Árvores-B */
typedef struct nodeip
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_ip *chave;	/* vetor das chaves e rrns [m-1]*/
	int *desc;			/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_ip;

typedef struct nodeis
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_is *chave;	/* vetor das chaves e rrns [m-1]*/
	int *desc;			/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_is;

typedef struct {
	int raiz;
} Indice;

/* Variáveis globais */

char ARQUIVO[MAX_REGISTROS * TAM_REGISTRO];
char ARQUIVO_IP[1000*sizeof(Chave_ip) + 40];
char ARQUIVO_IS[1000*sizeof(Chave_is) + 40];
/*Ordem das árvores*/
int ordem_ip;
int ordem_is;
int nregistros;
int nregistrosip; /*Número de nós presentes no ARQUIVO_IP*/
int nregistrosis; /*Número de nós presentes no ARQUIVO_IS*/
/*Quantidade de bytes que ocupa cada nó da árvore nos arquivos de índice:*/
int tamanho_registro_ip;
int tamanho_registro_is;
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

 /********************FUNÇÕES DO MENU*********************/
 void cadastrar(Indice* iprimary, Indice* idev);

 void alterar(Indice iprimary);

 void buscar(Indice iprimary,Indice idev);

 void listar(Indice iprimary,Indice idev);

 /*******************************************************/

 /* Recebe do usuário uma string simulando o arquivo completo e retorna o número
  * de registros. */
 int carregar_arquivo();

/* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary);

/* (Re)faz o índice de jogos  */
void criar_idev(Indice *idev) ;

/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata */
void write_btree(void *salvar, int rrn, char ip);

/*Lê um nó do arquivo de índice e retorna na estrutura*/
void *read_btree(int rrn, char ip);

/* Aloca um nó de árvore para ser utilizado em memória primária, o primeiro parametro serve para informar que árvore se trata
* É conveniente que essa função também inicialize os campos necessários com valores nulos*/
void *criar_no(char ip);

/*Libera todos os campos dinâmicos do nó, inclusive ele mesmo*/
void libera_no(void *node, char ip);

/*
*	Caro aluno,
*	caso não queira trabalhar com void*, é permitido dividir as funções que utilizam
* em duas, sendo uma para cada índice...
* Um exemplo, a write_btree e read_btree ficariam como:
*
*	void write_btree_ip(node_Btree_ip *salvar, int rrn),  node_Btree_ip *read_btree_ip(int rrn),
*	void write_btree_is(node_Btree_is *salvar, int rrn) e node_Btree_is *read_btree_is(int rrn).
*/



/* Atualiza os dois índices com o novo registro inserido */
void inserir_registro_indices(Indice *iprimary, Indice *idev, Jogo j);

/* Exibe o jogo */
int exibir_registro(int rrn);



int main()
{

	int carregarArquivo = 0; nregistros = 0, nregistrosip = 0, nregistrosis = 0;
	scanf("%d\n", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	scanf("%d %d%*c", &ordem_ip, &ordem_is);

	tamanho_registro_ip; /* = Deixarei esse cálculo pesado para você aluno... */
	tamanho_registro_is; /* = E esse também. */

	/* Índice primário */
	Indice iprimary ;
	criar_iprimary(&iprimary);

	/* Índice secundário de nomes dos Jogos */
	Indice idev;
	criar_idev(&idev);

	/* Execução do programa */
	int opcao = 0;
	while(1)
	{
		scanf("%d%*c", &opcao);
		switch(opcao) {
		case 1: /* Cadastrar um novo jogo */
			cadastrar(&iprimary, &idev);
			break;
		case 2: /* Alterar o desconto de um Jogo */
			printf(INICIO_ALTERACAO);
			alterar(iprimary);
			break;
		case 3: /* Buscar um jogo */
			printf(INICIO_BUSCA);
			buscar(iprimary, idev);
			break;
		case 4: /* Listar todos os Jogos */
			printf(INICIO_LISTAGEM);
			listar(iprimary, idev);
			break;
		case 5: /* Imprimir o arquivo de dados */
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO) ? ARQUIVO : ARQUIVO_VAZIO);
			break;
		case 6: /* Imprime o Arquivo de Índice Primário*/
			printf(INICIO_INDICE_PRIMARIO);
			printf("%s\n", (*ARQUIVO_IP) ? ARQUIVO_IP : ARQUIVO_VAZIO);
			break;
		case 7: /* Imprime o Arquivo de Índice Secundário*/
			printf(INICIO_INDICE_SECUNDARIO);
			printf("%s\n", (*ARQUIVO_IS) ? ARQUIVO_IS : ARQUIVO_VAZIO);
			break;
		case 8: /*Libera toda memória alocada dinâmicamente (se ainda houver) e encerra*/
			return 0;
		default: /* Exibe mensagem de erro */
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return -1;
}

/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}


/* Exibe o jogo */
int exibir_registro(int rrn)
{
	if(rrn<0)
		return 0;

	float preco;
	int desconto;

	Jogo j = recuperar_registro(rrn);

  	char *cat, categorias[TAM_CATEGORIA];

	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.desenvolvedora);
	printf("%s\n", j.data);
	printf("%s\n", j.classificacao);

	sscanf(j.desconto,"%d",&desconto);
	sscanf(j.preco,"%f",&preco);
	preco = preco *  (100-desconto);
	preco = ((int) preco)/ (float) 100 ;
	printf("%07.2f\n",  preco);

	strncpy(categorias, j.categoria, strlen(j.categoria));

  	for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    	printf("%s ", cat);
	printf("\n");

	return 1;
}
