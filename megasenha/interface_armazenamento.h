#ifndef INTERFACE_ARMAZENAMENTO_H
#define INTERFACE_ARMAZENAMENTO_H
#ifdef INTERFACE_ARMAZENAMENTO_SERVIDOR
    #define EXT1
#else
    #define EXT1 extern
#endif

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

void primeira_fase(FILE*arquivo, wordAndHints *newWordAndHint);

void segunda_fase_facil(FILE*arquivo, wordAndHints *newWordAndHint);

void segunda_fase_medio(FILE*arquivo, wordAndHints *newWordAndHint);

void segunda_fase_dificil(FILE*arquivo, wordAndHints *newWordAndHint);

int contar_linhas();

void ranking (int recorde, char *name);

int getSizeOfRanking(FILE *fp);

// declare functions intern to armazenamento module
#ifdef ARMAZENAMENTO_OWN
    Ranking readRanking();
    void writeRanking(Ranking ranking);
    void insertNewScoreInRanking(Ranking *ranking, Score newScore);
    Score fromNameAndPointsToScore(int points, char *name);
#endif

#endif
