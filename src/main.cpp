#include "engine/GameManager/game_manager.hpp"
#include "Interfaces/menu/MenuPrincipal.hpp"

/*
  Name: DestructionBombs
  Copyright: GNU/GPL v3
  Author: Manuel Gonzalez @manuelggz
  Date: 26/11/16 07:21
  Description: Remake del famoso bomberman
*/

int main(int argc, char *argv[]){
    GameManager juego;
    juego.cambiarInterfaz(new MenuPrincipal(&juego));
    juego.run();
    return EXIT_SUCCESS;
}
