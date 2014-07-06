//!Main
/*!Call interface_palavra_e_dica.h,interface_gui.h and *interface_controle.h.Start interface and show the winner player 
*
*
*\author Douglas,Isabella,Thiago,Tiago
*\since 04/07/14
*\version 2.0
*
*
*/
#include "interface_palavra_e_dicas.h"
#include "interface_gui.h"
#include "interface_controle.h"


int main(int argc, char *argv[]) {
  int playerVencedor = 1;//!< Integer of the nunber og the winner player
  int scoreFirstRound = 0;//!< Integer of the score of the first round
  
  playerVencedor = iniciaPrimeiraFase(argc, argv, &scoreFirstRound);//!< Receive the winner player
  closeInterface();
  updateInterface();
  printf("o player vencedor e %d\n", playerVencedor);//!< Show the winner player
  iniciaSegundaFase(argc, argv, playerVencedor, scoreFirstRound);//!<Started the second round
}

// TODO: add interface to add new words
// TODO: interface to see ranking
// TODO: word can be called two times
// TODO: give more time to give up
