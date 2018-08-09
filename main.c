/*============================================================================*/
/* Exemplo: segmenta��o de uma imagem em escala de cinza.                     */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu                                                     */
/* Universidade Tecnol�gica Federal do Paran�                                 */
/*============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pdi.h"

/*============================================================================*/

#define INPUT_IMAGE "arroz.bmp"

// TODO: ajuste estes par�metros!
#define NEGATIVO 0
#define THRESHOLD 0.8f
#define ALTURA_MIN 5
#define LARGURA_MIN 5
#define N_PIXELS_MIN 30

/*============================================================================*/

typedef struct
{
    float label;
    Retangulo roi;
    int n_pixels;

} Componente;

/*============================================================================*/

void binariza(Imagem *in, Imagem *out, float threshold);
int rotula(Imagem *img, Componente **componentes, int largura_min, int altura_min, int n_pixels_min);

/*============================================================================*/

int main()
{
    int i;

    // Abre a imagem em escala de cinza, e mant�m uma c�pia colorida dela para desenhar a sa�da.
    Imagem *img = abreImagem(INPUT_IMAGE, 1);
    if (!img)
    {
        printf("Erro abrindo a imagem.\n");
        exit(1);
    }

    Imagem *img_out = criaImagem(img->largura, img->altura, 3);
    cinzaParaRGB(img, img_out);

    // Segmenta a imagem.
    if (NEGATIVO)
        inverte(img, img);
    binariza(img, img, THRESHOLD);
    salvaImagem(img, "01 - binarizada.bmp");

    Componente *componentes;
    int n_componentes;
    clock_t tempo_inicio = clock();
    n_componentes = rotula(img, &componentes, LARGURA_MIN, ALTURA_MIN, N_PIXELS_MIN);
    clock_t tempo_total = clock() - tempo_inicio;

    printf("Tempo: %d\n", (int)tempo_total);
    printf("%d componentes detectados.\n", n_componentes);

    // Mostra os objetos encontrados.
    for (i = 0; i < n_componentes; i++)
        desenhaRetangulo(componentes[i].roi, criaCor(1, 0, 0), img_out);
    salvaImagem(img_out, "02 - out.bmp");

    // Limpeza.
    free(componentes);
    destroiImagem(img_out);
    destroiImagem(img);
    return (0);
}

/*----------------------------------------------------------------------------*/
/** Binariza��o simples por limiariza��o.
 *
 * Par�metros: Imagem* in: imagem de entrada. Se tiver mais que 1 canal,
 *               binariza cada canal independentemente.
 *             Imagem* out: imagem de sa�da. Deve ter o mesmo tamanho da
 *               imagem de entrada.
 *             float threshold: limiar.
 *
 * Valor de retorno: nenhum (usa a imagem de sa�da). */

void binariza(Imagem *in, Imagem *out, float threshold)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais)
    {
        printf("ERRO: binariza: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit(1);
    }

    // TODO: escreva o c�digo desta fun��o.

    int j, k;
    for (j = 0; j < in->altura; j++)
        for (k = 0; k < in->largura; k++)
            if (in->dados[0][j][k] >= THRESHOLD)
                in->dados[0][j][k] = -1.0f;
            else
                in->dados[0][j][k] = 0.0f;
}

/*============================================================================*/
/* ROTULAGEM                                                                  */
/*============================================================================*/
/** Rotulagem usando flood fill. Marca os objetos da imagem com os valores
 * [0.1,0.2,etc].
 *
 * Par�metros: Imagem* img: imagem de entrada E sa�da.
 *             Componente** componentes: um ponteiro para um vetor de sa�da.
 *               Supomos que o ponteiro inicialmente � inv�lido. Ele ir�
 *               apontar para um vetor que ser� alocado dentro desta fun��o.
 *               Lembre-se de desalocar o vetor criado!
 *             int largura_min: descarta componentes com largura menor que esta.
 *             int altura_min: descarta componentes com altura menor que esta.
 *             int n_pixels_min: descarta componentes com menos pixels que isso.
 *
 * Valor de retorno: o n�mero de componentes conexos encontrados. */

int floodfill(Imagem *img, int j, int k, Componente *componente)
{

    if (img->dados[0][j][k] != -1.0f)
        return 0;
    else
    {
        img->dados[0][j][k] = componente->label;
        componente->n_pixels++;
        if (k < componente->roi.e) //direita
            componente->roi.e = k;
        if (k > componente->roi.d) //esquerda
            componente->roi.d = k;
        if (j < componente->roi.c) //cima
            componente->roi.c = j;
        if (j > componente->roi.b) //baixo
            componente->roi.b = j;
        if(k+1 < img->largura)
            floodfill(img, j, k + 1, componente); // direita
        if(k-1 > 0)
            floodfill(img, j, k - 1, componente); // esquerda
        if(j+1 < img->altura)
            floodfill(img, j + 1, k, componente); // baixo
        if(k-1 > 0)
            floodfill(img, j - 1, k, componente); // cima
    }
    return 1;
}

int rotula(Imagem *img, Componente **componentes, int largura_min, int altura_min, int n_pixels_min)
{
    // TODO: escreva esta fun��o.
    // Use a abordagem com flood fill recursivo.
    // Observe que o par�metro 'componentes' � um ponteiro para um vetor, ent�o a aloca��o dele deve ser algo como:
    // *componentes = malloc (sizeof (Componente) * n);
    // Dependendo de como voc� fizer a sua implementa��o, pode ser tamb�m interessante alocar primeiro um vetor maior do que o necess�rio, ajustando depois o tamanho usando a fun��o realloc.

    int j, k, c = 0;
    float label = 0.1f;

    *componentes = malloc(sizeof(Componente) * 100);

    for (j = 0; j < img->altura; j++)
    {
        for (k = 0; k < img->largura; k++)
        {
            if (img->dados[0][j][k] == -1.0f)
            {
                (*componentes)[c].label = label;
                (*componentes)[c].n_pixels = 0;
                (*componentes)[c].roi.c = 999999;
                (*componentes)[c].roi.b = 0;
                (*componentes)[c].roi.e = 999999;
                (*componentes)[c].roi.d = 0;
                floodfill(img, j, k, &((*componentes)[c]));
                if ((*componentes)[c].n_pixels < N_PIXELS_MIN)
                {
                    (*componentes)[c].label = label;
                    (*componentes)[c].n_pixels = 0;
                }
                else
                {
                    c++;
                    label += 0.1f;
                }
            }
        }
    }

    *componentes = realloc(*componentes, sizeof(Componente) * c);

    return (c);
}

/*============================================================================*/
