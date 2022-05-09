#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <locale.h>

typedef struct
{
  int tamMatriz; // NUMERO DE LINHAS E VARIAVEIS
  double **matrizRecebida;
} MatrizGS;

double **alocacaoDeMatriz(int linha, int coluna)
{
  // Alocação dinâmica da matriz usando o "malloc()"
  int i, j;
  double **matrizCRIADA;
  matrizCRIADA = malloc(sizeof(double *) * linha);
  if (matrizCRIADA == NULL)
    return NULL;
  for (i = 0; i < linha; i++)
  {
    matrizCRIADA[i] = malloc(sizeof(double) * coluna);
    if (matrizCRIADA[i] == NULL)
    {
      for (j = 0; j < i; j++)
        free(matrizCRIADA[j]);
      free(matrizCRIADA);
      return NULL;
    }
  }
  return matrizCRIADA;
}

// FUNÇÃO QUE ABRE UM ARQUIVO SEGUNDO O DIRETÓRIO DADO PELO USUÁRIO E LER ESSE ARQUIVO, PASSANDO PARA UMA MATRIZ DE TAMANHO ESPECIFICADO NO ARQUIVO

MatrizGS *Leitura_Abertura_Arquivo()
{
  char nomeArquivo[20];
  int i, j, tam;
  double **maatriz;

  FILE *arquivo;

  printf("Digite o nome do arquivo: ");
  scanf(" %s", nomeArquivo);
  arquivo = fopen(nomeArquivo, "r"); // ABRINDO O ARQUIVO DADO COM O "fopen" E LENDO ELE COM O PARÂMETRO "r" de READ

  if (arquivo == NULL)
  { // FUNÇÃO PARA CASOS ONDE O ARQUIVO RETORNA NULO
    printf("O arquivo nao existe.\n");
    return NULL;
  }
  fscanf(arquivo, " %d", &tam); // "fscanf()" TRABALHA COM A LEITURA DE ENTRADA FORMATADA (SEMELHANTE AO "scanf()"). NESTE CASO ESTÁ RECEBENDO O NÚMERO DE LINHAS QUE O SISTEMA LINEAR POSSUI

  maatriz = alocacaoDeMatriz(tam, tam + 1); // CHAMA A FUNÇÃO PARA CRIAR UMA MATRIZ QUE TENHA N LINHAS E N+1 COLUNAS

  for (i = 0; i < tam; i++)
  { // GUARDA DENTRO DA MATRIZ CRIADA OS VALORES PASSADOS DENTRO DO ARQUIVO SEGUINDO A ORDEM DAS LINHAS E COLUNAS
    for (j = 0; j < tam + 1; j++)
    {
      fscanf(arquivo, " %lf", &maatriz[i][j]);
    }
  }
  fclose(arquivo); // FUNÇÃO QUE FECHA O ARQUIVO

  MatrizGS *Matriz_SL = (MatrizGS *)malloc(sizeof(MatrizGS)); //"malloc" É A FUNÇÃO CENTRAL DA ALOCAÇÃO DINAMICA DE MEMORIA, DEVEMOS CHAMAR O "sizeof" E RECUPERAR A QUANTIDADE DE MEMÓRIA UTILIZADA NO ARMAZENAMENTO.

  if (Matriz_SL == NULL)
    return NULL;
  Matriz_SL->matrizRecebida = maatriz;
  Matriz_SL->tamMatriz = tam;

  return Matriz_SL;
}

// FUNÇÃO QUE VERIFICA SE A MATRIZ DE COEFICIENTES SATISFAZ O CRITÉRIO DAS LINHAS OU O CRITÉRIO DAS COLUNAS.

int VerificacaoDasLinhasEColunas(double **m)
{
  int var = 3, i, j;
  double pivo, Soma_da_linha, Soma_da_Coluna;

  for (i = 0; i < var; i++)
  {
    Soma_da_linha = 0.0;
    Soma_da_Coluna = 0.0;
    pivo = fabs(m[i][i]); // O pivo é localizado na linha i coluna i de uma determinada matriz, a função "fabs()" retorna o valor absoluto de x.

    for (j = 0; j < var; j++)
    {
      if (i != j)
      {

        Soma_da_linha = Soma_da_linha + fabs(m[i][j]);
        Soma_da_Coluna = Soma_da_Coluna + fabs(m[j][i]);
      }

      // O critério das linhas e das colunas é atendido quando o pivô é maior ou igual a soma dos elementos da sua linha e coluna
      if (Soma_da_linha > pivo || Soma_da_Coluna > pivo)
        return -1;
    } // DEVOLVE O VALOR 1 PARA BEM CONDICIONADA E -1 PARA MAL CONDICIONADA
  }
  return 1;
}

void Metodo_GaussSeidel()
{
  MatrizGS *Matriz_SL = Leitura_Abertura_Arquivo();
  // CHAMADA DA FUNÇÃO PARA ABRIR E LER O ARQUIVO
  if (Matriz_SL == NULL)
  {
    printf("Erro na leitura do arquivo.\n");
    return;
  }
  // SE O ARQUIVO FOR NULO A FUNÇÃO RETORNARÁ UMA MENSAGEM DE ERRO

  double **m = Matriz_SL->matrizRecebida;

  // SE A FUNÇÃO DE VERIFICAÇÃO DEVOLVER -1 ELA NÃO SATISFAZ O CRITÉRIO DAS LINHAS E DAS COLUNAS

  if (VerificacaoDasLinhasEColunas(m) == -1)
  {
    printf("\nO sistema linear não satisfaz o critério das linhas nem o critério das colunas.\n");
    return;
  }

  double pivo, Elemento_Linha;
  int i, j, interacoes = 0;
  int var = Matriz_SL->tamMatriz; // RECEBENDO O VALOR DA PRIMEIRA LINHA DO ARQUIVO, REFERENTE AO NUMERO DE VARIÁVEIS E LINHAS.

  double valores_linha_pivo[var], valoresAnteriores[var];

  for (i = 0; i < var; i++)
  {
    valores_linha_pivo[i] = 0.0;
    valoresAnteriores[i] = 0.0;
  }
  // EXECUTANDO 1000 iterações
  int Dif_Minima = 0;
  while (interacoes < 1000 && Dif_Minima == 0)
  {
    for (i = 0; i < var; i++)
    {
      Elemento_Linha = 0.0;
      pivo = m[i][i]; // PIVO
      for (j = 0; j < var; j++)
      {
        if (i != j)
        {
          // CALCULANDO O NOVO VALOR DOS ELEMENTOS DA LINHA
          Elemento_Linha = Elemento_Linha - (valores_linha_pivo[j] * m[i][j]);
        }
      }
      // CALCULANDO OS VALORES DOS MULTIPLICADORES
      valores_linha_pivo[i] = (Elemento_Linha + m[i][var]) / pivo;
    }

    Dif_Minima = 1;

    for (i = 0; i < var; i++)
    {
      Dif_Minima = (fabs(valoresAnteriores[i] - valores_linha_pivo[i]) < 0.00000001) && (Dif_Minima == 1) ? 1 : 0;
      valoresAnteriores[i] = valores_linha_pivo[i];
    }
    interacoes++;
  }

  printf("\n\n============RESOLUCAO DO SL METODO DE GAUSS-SEIDEL=============");
  printf("\n\n° Numero de iterações realizadas: %d\n", interacoes);
  printf("\n° Solucao encontrada:\n");
  for (i = 0; i < var; i++)
  {
    printf("x%d: %f; ", i + 1, valores_linha_pivo[i]);
  }
  printf("\n\n===============================================================\n\n");
}

int main(int argc, char *argv[])
{
  setlocale(LC_ALL, "");

  char input;
  do
  {
    printf("S - Sistema Linear \n");
    scanf(" %c", &input);
    switch (input)
    {
      double value, intpart, fracpart;

    case 'S':
      Metodo_GaussSeidel();
      break;

    default:
      printf("Comando inválido!\n");
      break;
    }
  } while (1);
}
