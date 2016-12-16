//
// Created by manuggz on 20/11/16.
//

#ifndef BOMBERMAN_MENUMODOMULTIJUGADOR_HPP
#define BOMBERMAN_MENUMODOMULTIJUGADOR_HPP

#include <SDL_render.h>
#include <SDL_events.h>
#include "../../engine/interfaces/InterfazUI.hpp"
#include "../../engine/sprites/CGroup.hpp"
#include "../../engine/layout/LayoutManager/LayoutVertical.hpp"
#include "../../engine/layout/LayoutManager/LayoutAbsolute.hpp"
#include "../../engine/layout/Componentes/ImageComponent.hpp"
#include "../../engine/layout/Componentes/LabelComponent.hpp"
#include "../../engine/interfaces/InterfazSpriteGroup.hpp"
#include "../../niveles/NivelMapa.hpp"
#include "../../engine/layout/Componentes/BotonComponent.hpp"
#include "../../engine/sprites/animacion/animacion.hpp"
#include "../../engine/sprites/CDrawGroup.hpp"
#include "../juego/juego.hpp"

class MenuModoMultijugador: public InterfazUI, public InterfazSpriteGroup {

public:
    MenuModoMultijugador(GameManagerInterfazUI * gameManagerInterfazUI):
            InterfazUI(gameManagerInterfazUI),mMapaTerrenoSeleccionado(0,32){
    }

    void prepare() override {
        //cout << "MenuModoMultijugador::prepare"<<endl;

        // Buscamos el Maximo numero de terrenos disponibles para usar en el juego
        mMaxTerrenoBatalla = std::stoi(buscar_dato(RUTA_CONFIG_BASE, NAME_MAX_TERRENO_BATALLA));

        minutosEscogidos   = 1;
        victoriasEscogidas = 1;
    }

    void createUI(SDL_Renderer * gRenderer) override {
        //cout << "MenuModoMultijugador::createUI"<<endl;

        mLayoutParent = new LayoutAbsolute();

        // BotonComponent para controlar cuanto tiempo para acabar una ronda
        mBtnSubirTiempo=new BotonComponent<MenuModoMultijugador>(
                mGameManagerInterfaz->getTexture(Galeria::CodeImagen::IMG_BOTON_FLECHA_PEQUE_DERECHA),this);
        mBtnSubirTiempo->setId(MENU_BOTON_SUBIR_TIEMPO);
        mBtnSubirTiempo->bindAccion(&MenuModoMultijugador::clickControl);
        mLayoutParent->addComponent(mBtnSubirTiempo);
        mBtnSubirTiempo->setLayoutParam(LAYOUT_PARAM_X,"197");
        mBtnSubirTiempo->setLayoutParam(LAYOUT_PARAM_Y,"2");

        // BotonComponent para controlar cuantas victorias son necesarias para terminar el juego
        mBtnSubirVictorias=new BotonComponent<MenuModoMultijugador>(
                mGameManagerInterfaz->getTexture(Galeria::CodeImagen::IMG_BOTON_FLECHA_PEQUE_DERECHA),this);
        mBtnSubirVictorias->setId(MENU_BOTON_SUBIR_VICTORIAS);
        mBtnSubirVictorias->bindAccion(&MenuModoMultijugador::clickControl);
        mLayoutParent->addComponent(mBtnSubirVictorias);
        mBtnSubirVictorias->setLayoutParam(LAYOUT_PARAM_X,"297");
        mBtnSubirVictorias->setLayoutParam(LAYOUT_PARAM_Y,"2");

        // BotonComponent para cambiar el mapa a usar
        mBtnCambiarMapa=new BotonComponent<MenuModoMultijugador>(
                mGameManagerInterfaz->getTexture(Galeria::CodeImagen::IMG_BOTON_CAMBIAR_MAPA),this);
        mBtnCambiarMapa->setId(MENU_BOTON_CAMBIAR_MAPA);
        mBtnCambiarMapa->bindAccion(&MenuModoMultijugador::clickControl);
        mLayoutParent->addComponent(mBtnCambiarMapa);
        mBtnCambiarMapa->setLayoutParam(LAYOUT_PARAM_X,"10");
        mBtnCambiarMapa->setLayoutParam(LAYOUT_PARAM_Y,"225");

        // BotonComponent para comenzar a jugar
        mBtnJugar=new BotonComponent<MenuModoMultijugador>(mGameManagerInterfaz->getTexture(Galeria::CodeImagen::IMG_BOTON_JUGAR_2),this);
        mBtnJugar->setId(MENU_BOTON_JUGAR);
        mBtnJugar->bindAccion(&MenuModoMultijugador::clickControl);
        mBtnJugar->setVisible(false); // Lo ocultamos hasta que se seleccionen dos jugadores
        mLayoutParent->addComponent(mBtnJugar);
        mBtnJugar->setLayoutParam(LAYOUT_PARAM_X,"240");
        mBtnJugar->setLayoutParam(LAYOUT_PARAM_Y,"225");

        // Controla las animaciones de los personajes cuando se seleccionan para jugar
        mSprites=new DrawGroup(this);
        cout << "mGrpSprites : " << mSprites << endl;

        //Animaciones para los personajes (Hace que parezcan que caminan) cuando se seleccionan
        SpriteSheet * spriteSheetTmp = new SpriteSheet();
        spriteSheetTmp->cargarDesdeArchivo(gRenderer,"data/imagenes/personajes/player_1.bmp",1,12,true);
        mAnimacionPlayer[0]=new Animacion(spriteSheetTmp,"6,6,7,7,8,8");

        spriteSheetTmp = new SpriteSheet();
        spriteSheetTmp->cargarDesdeArchivo(gRenderer,"data/imagenes/personajes/player_2.bmp",1,12,true);
        mAnimacionPlayer[1]=new Animacion(spriteSheetTmp,"6,6,7,7,8,8");

        spriteSheetTmp = new SpriteSheet();
        spriteSheetTmp->cargarDesdeArchivo(gRenderer,"data/imagenes/personajes/player_3.bmp",1,12,true);
        mAnimacionPlayer[2]=new Animacion(spriteSheetTmp,"6,6,7,7,8,8");

        spriteSheetTmp = new SpriteSheet();
        spriteSheetTmp->cargarDesdeArchivo(gRenderer,"data/imagenes/personajes/player_4.bmp",1,12,true);
        mAnimacionPlayer[3]=new Animacion(spriteSheetTmp,"6,6,7,7,8,8");

        spriteSheetTmp = new SpriteSheet();
        spriteSheetTmp->cargarDesdeArchivo(gRenderer,"data/imagenes/personajes/player_5.bmp",1,12,true);
        mAnimacionPlayer[4]=new Animacion(spriteSheetTmp,"6,6,7,7,8,8");


        for(int i=0;i<_PLAYERS;i++){
            // Animacion para cuando aun no se ha seleccionado el personaje(Hace que parpadee "presiona")

            spriteSheetTmp = new SpriteSheet();
            spriteSheetTmp->cargarDesdeArchivo(gRenderer,"data/imagenes/textos/txt_presiona.png",2,1);
            mAnimaPresiona[i]=new Animacion(spriteSheetTmp,"0,0,1,1");
            // Animacion para cuando se selecciono el personaje(Hace que parpadee "activado")
            spriteSheetTmp = new SpriteSheet();
            spriteSheetTmp->cargarDesdeArchivo(gRenderer,"data/imagenes/personajes/txt_activado.bmp",2,1);
            mAnimaActivado[i]=new Animacion(spriteSheetTmp,"0,0,0,1,1,1");

            // Hace que las animaciones se repitan indefinidamente(Hasta que se eliminen desde el codigo)
            mAnimacionPlayer[i]->setRepeticiones(-1);
            mAnimaActivado[i]->setRepeticiones(-1);
            mAnimaPresiona[i]->setRepeticiones(-1);

            // Las agrega al controlador que hace que se actualizen y se muestren
            mSprites->add(mAnimaPresiona[i]);
        }

        SDL_Color color = {255,0,0,255};
        mTextLabelMinutos = new LabelComponent();
        mTextLabelMinutos->setText(std::to_string(minutosEscogidos));
        mTextLabelMinutos->setFont("data/fuentes/OpenSans-Bold.ttf",15);
        mTextLabelMinutos->setTextColor(color);
        mTextLabelMinutos->setLayoutParam(LAYOUT_PARAM_X,"180");
        mTextLabelMinutos->setLayoutParam(LAYOUT_PARAM_Y,"0");
        mLayoutParent->addComponent(mTextLabelMinutos);

        mTextLabelVictorias = new LabelComponent();
        mTextLabelVictorias->setText(std::to_string(victoriasEscogidas));
        mTextLabelVictorias->setFont("data/fuentes/OpenSans-Bold.ttf",15);
        mTextLabelVictorias->setTextColor(color);
        mTextLabelVictorias->setLayoutParam(LAYOUT_PARAM_X,"283");
        mTextLabelVictorias->setLayoutParam(LAYOUT_PARAM_Y,"0");
        mLayoutParent->addComponent(mTextLabelVictorias);

        //static char tmp[50];
        //sprintf(tmp,"%d",i+1);
        //imprimir_palabra(gRenderer,mGameManagerInterfaz->getTexture(IMG_FUENTE_6),mAnimacionPlayer[i]->getX()-9+41,mAnimacionPlayer[i]->getY()+19,tmp,STR_MAX_ESTENDIDA);

        //mGameManagerInterfaz->playSound(SND_MENU);

        establecerTerrenoBatalla(gRenderer,0);

        SDL_ShowCursor(SDL_ENABLE);
        mGRenderer = gRenderer;
    }

    virtual void eliminarSprite(Sprite *sprite) override {

    }

    void packLayout(SDL_Renderer * gRenderer){
        cout << "MenuModoMultijugador::packLayout"<<endl;
        SDL_Rect rect = mGameManagerInterfaz->getRectScreen();
        mLayoutParent->pack(gRenderer);
        mLayoutParent->setRectDibujo(rect);
    }
    /**
     * Establece/Cambia el terreno en el que se jugara
     * @param nuevoTerreno
     * @return
     */
    bool establecerTerrenoBatalla(SDL_Renderer * gRenderer,int nuevoTerreno) {
        cout << "MenuModoMultijugador::establecerTerrenoBatalla"<<endl;

        if(nuevoTerreno>=0 && nuevoTerreno < mMaxTerrenoBatalla){
            static char ruta1[50],ruta2[50];
            sprintf(ruta1,"data/niveles/batalla/mapa_batalla_%d.tmx",nuevoTerreno+1);
            if(!mMapaTerrenoSeleccionado.cargar(gRenderer,ruta1)){
                std::cerr << "Error cambiando el mapa" << std::endl;
                return false;
            }

            for(int i=0;i<_PLAYERS;i++){

                mAnimacionPlayer[i]->setX(mMapaTerrenoSeleccionado.getPosXPlayer((IdPlayer)(PLAYER_1 + i)));
                mAnimacionPlayer[i]->setY(mMapaTerrenoSeleccionado.getPosYPlayer((IdPlayer)(PLAYER_1 + i)));

                // Animacion para cuando aun no se ha seleccionado el personaje(Hace que parpadee "presiona")

                mAnimaPresiona[i]->setX(mAnimacionPlayer[i]->getX()-9);
                mAnimaPresiona[i]->setY(mAnimacionPlayer[i]->getY()+20);

                mAnimaActivado[i]->setX(mAnimacionPlayer[i]->getX()-9);
                mAnimaActivado[i]->setY(mAnimacionPlayer[i]->getY()+20);

            }
            terrenoActual = nuevoTerreno;
            return true;
        }

        return false;

    }

    /**
     * Funcion llamada por los botones de la interfaz cuando son presionados
     * @param control_click
     */
    void clickControl(BotonComponent<MenuModoMultijugador> * control_click) {
        cout << "MenuModoMultijugador::clickControl"<<endl;
        mBotonClicked = control_click->getId();
        ejecutarAccionBotonClicked();
    }

    /**
     * Ejecuta la opcion enlazada a un boton
     */
    void ejecutarAccionBotonClicked(){
        cout << "MenuModoMultijugador::ejecutarAccionBotonClicked"<<endl;
        switch(mBotonClicked){
            case MENU_BOTON_SUBIR_TIEMPO:
                if(++minutosEscogidos>5)minutosEscogidos=1;
                mTextLabelMinutos->setText(std::to_string(minutosEscogidos));
                break;
            case MENU_BOTON_SUBIR_VICTORIAS:
                if(++victoriasEscogidas>8)victoriasEscogidas=1;
                mTextLabelVictorias->setText(std::to_string(victoriasEscogidas));
                break;
            case MENU_BOTON_CAMBIAR_MAPA:
                if(establecerTerrenoBatalla(mGRenderer,(terrenoActual + 1 == mMaxTerrenoBatalla)?0:terrenoActual + 1)){
                    mGameManagerInterfaz->play(Galeria::CodeMusicEfecto::SFX_TONO_ACUATICO);
                }
                break;
            case MENU_BOTON_JUGAR:
                int total_players=mIsPlayerActivado[PLAYER_1]+ mIsPlayerActivado[PLAYER_2]+ mIsPlayerActivado[PLAYER_3] + mIsPlayerActivado[PLAYER_4] + mIsPlayerActivado[PLAYER_5];
                if(total_players>=2){
                    Juego * nuevoJuego = new Juego(
                            mGameManagerInterfaz,
                            "data/niveles/batalla/mapa_batalla_" + std::to_string(terrenoActual + 1) + ".tmx",
                            victoriasEscogidas,
                            minutosEscogidos,
                            mIsPlayerActivado
                    );
                    mGameManagerInterfaz->cambiarInterfaz(nuevoJuego); //iniciamos en modo batalla, le pasamos el array con los players seleccionados por el usuario
                    mGameManagerInterfaz->play(Galeria::CodeMusicEfecto::SFX_EXPLOSION);
                }
                break;
        }
    }

    /**
     *  Cambia el estado de un player lo establece activo/desactivado
     * @param idPlayer
     */
    void cambiarEstadoPlayer(int idPlayer){
        cout << "MenuModoMultijugador::cambiarEstadoPlayer"<<endl;
        mIsPlayerActivado[idPlayer]=!mIsPlayerActivado[idPlayer];

        if(mIsPlayerActivado[idPlayer]){ // Si debe agregarse al juego
            // Se activan/desactivan las animaciones
            mSprites->add(mAnimacionPlayer[idPlayer]);
            mSprites->add(mAnimaActivado[idPlayer]);
            mSprites->erase(mAnimaPresiona[idPlayer]);
        }else{ // Si debe eliminarse del juego
            // Se activan/desactivan las animaciones
            mSprites->erase(mAnimacionPlayer[idPlayer]);
            mSprites->erase(mAnimaActivado[idPlayer]);
            mSprites->add(mAnimaPresiona[idPlayer]);
        }

        // Si hay mas de dos botones players activados se muestra el boton de jugar
        mBtnJugar->setVisible(mIsPlayerActivado[PLAYER_1]+ mIsPlayerActivado[PLAYER_2]+ mIsPlayerActivado[PLAYER_3] + mIsPlayerActivado[PLAYER_4] + mIsPlayerActivado[PLAYER_5]>1);
        mGameManagerInterfaz->play(Galeria::CodeMusicEfecto::SFX_TONO_SECO);
    }

    virtual void resume() override {
        //cout << "MenuModoMultijugador::resume"<<endl;
        mIsPaused = false;
        SDL_ShowCursor(SDL_ENABLE);
    }

    virtual void procesarEvento(SDL_Event *event) override {
        cout << "MenuModoMultijugador::procesarEvento"<<endl;
        if(event->type==SDL_KEYDOWN){
            switch(event->key.keysym.sym){
                case SDLK_ESCAPE:
                    mGameManagerInterfaz->goBack();
                    break;
                case SDLK_RETURN:
                    mBotonClicked=MENU_BOTON_JUGAR;
                    ejecutarAccionBotonClicked();
                    break;
                case SDLK_KP_1:case SDLK_KP_2:case SDLK_KP_3:case SDLK_KP_4:case SDLK_KP_5:
                    cambiarEstadoPlayer(event->key.keysym.sym-SDLK_KP_1); //seleccionamos el player con una formula mate. SDK_1:49 y SDLK_5:53
                    break;
                case SDLK_1:case SDLK_2:case SDLK_3:case SDLK_4:case SDLK_5:
                    cambiarEstadoPlayer(event->key.keysym.sym-SDLK_1); //seleccionamos el player con una formula mate. SDK_1:49 y SDLK_5:53
                    break;
                case SDLK_LEFT:case SDLK_RIGHT:
                    mBotonClicked=MENU_BOTON_CAMBIAR_MAPA;
                    ejecutarAccionBotonClicked();
                    break;
                default:
                    break;
            }
        }

        mBtnSubirTiempo->procesarEvento(event);
        mBtnSubirVictorias->procesarEvento(event);
        mBtnCambiarMapa->procesarEvento(event);
        mBtnJugar->procesarEvento(event);
    }

    virtual void update() override {
        //cout << "MenuModoMultijugador::update"<<endl;
        mSprites->update(nullptr);
        mMapaTerrenoSeleccionado.update();
    }

    virtual void draw(SDL_Renderer *gRenderer) override {
        //cout << "MenuModoMultijugador::draw"<<endl;

        //mGameManagerInterfaz->getTexture((CodeImagen)std::stoi(mMapaTerrenoSeleccionado.getPropertyMap(MAPA_PROPERTY_ID_FONDO)))->render(gRenderer,0,0);*/
        mGameManagerInterfaz->getTexture(Galeria::CodeImagen::IMG_TABLERO)->render(gRenderer,0,0);//imprimimos la barra mensage
        mGameManagerInterfaz->getTexture(Galeria::CodeImagen::IMG_CUADRO_PEQUENIO)->render(gRenderer,177,0);//imprimimos la barra mensage
        mGameManagerInterfaz->getTexture(Galeria::CodeImagen::IMG_CUADRO_PEQUENIO)->render(gRenderer,280,0);//imprimimos la barra mensage
        mGameManagerInterfaz->getTexture(Galeria::CodeImagen::IMG_TXT_PLAYERS_EN_BATALLA)->render(gRenderer,15,0);//imprimimos la barra mensage
        mGameManagerInterfaz->getTexture(Galeria::CodeImagen::IMG_TXT_TIEMPO_POR_RONDA)->render(gRenderer,140,18);//imprimimos la barra mensage
        mGameManagerInterfaz->getTexture(Galeria::CodeImagen::IMG_TXT_VICTORIAS)->render(gRenderer,261,18);//imprimimos la barra mensage

        mMapaTerrenoSeleccionado.draw(gRenderer);//imprimimos el nivel

        mSprites->draw(gRenderer);
        for(int i=0;i<_PLAYERS;i++){
            if(!mIsPlayerActivado[i]){
                imprimir_desde_grilla(mGameManagerInterfaz->getTexture((Galeria::CodeImagen)(Galeria::CodeImagen::IMG_PLAYER_1 + i)), 6,gRenderer, mAnimacionPlayer[i]->getX(),mAnimacionPlayer[i]->getY(),1, 12,true);
            }else{
                imprimir_desde_grilla(mGameManagerInterfaz->getTexture(Galeria::CodeImagen::IMG_CARAS_BOMBERMAN),i*2,gRenderer,i*16+20,
                                      15 + 0,1,10,0);
            }
        }

        if(mLayoutParent->isDisabled()){
            packLayout(gRenderer);
        }
        mLayoutParent->draw(gRenderer);

    }

    ~MenuModoMultijugador(){
        cout << "MenuModoMultijugador::~MenuModoMultijugador"<<endl;
        for(int i=0;i<_PLAYERS;i++){
            delete mAnimacionPlayer[i];
            delete mAnimaPresiona[i];
            delete mAnimaActivado[i];
        }
        //SDL_FreeSurface(previewTerreno);
        //delete dataNivel;
        delete mLayoutParent;
    }
private:
    
    SDL_Renderer * mGRenderer = nullptr;
    // Contiene las animaciones(los players que se mueven)
    DrawGroup * mSprites = nullptr;

    // Usado para dibujar el mapa seleccionado actualmente
    NivelMapa mMapaTerrenoSeleccionado;

    //DatNivel *     dataNivel;
    //SDL_Surface *  previewTerreno;

    // Botones de la interfaz
    BotonComponent<MenuModoMultijugador> *mBtnSubirTiempo,*mBtnSubirVictorias,*mBtnCambiarMapa,*mBtnJugar;

    /**
     * Ids asignados a los botones de la interfaz, son usados en un swith cuando se llama a la funcion
     * enlazada a los botones para saber cual boton fué presionado.
     */
    enum{
        MENU_BOTON_SUBIR_TIEMPO,
        MENU_BOTON_SUBIR_VICTORIAS,
        MENU_BOTON_CAMBIAR_MAPA,
        MENU_BOTON_JUGAR,
    };

    // Dice cual boton fué clickeado / Tambien se usa para ejecutar una funcion asociada al click de un boton
    int mBotonClicked = -1;

    // Animacion del personaje
    Animacion * mAnimacionPlayer[_PLAYERS] {nullptr};
    // Animacion del texto "presiona"
    Animacion * mAnimaPresiona[_PLAYERS] {nullptr};
    // Animacion del texto "activado"
    Animacion * mAnimaActivado[_PLAYERS] {nullptr};

    // Dice cuales estan activados
    bool mIsPlayerActivado[_PLAYERS] {false};

    // ID del terreno actual escogido por el usuario / EL ID del terreno dibujado en pantalla
    int terrenoActual = 0;

    // Minutos Escogidos de duracion de cada ronda
    int minutosEscogidos = 1;

    // Victorias escogidas para acabar el juego
    int victoriasEscogidas = 1;

    // Maximo ID(Numero) asignado a un mapa de batalla
    int mMaxTerrenoBatalla = 1;

    // Controla los botones en un layout
    LayoutAbsolute *mLayoutParent = nullptr;

    // Muestra en la UI el numero de minutos escogidos
    LabelComponent *mTextLabelMinutos = nullptr;
    // Muestra en la UI el numero de victorias escogidas
    LabelComponent *mTextLabelVictorias = nullptr;
};
#endif //BOMBERMAN_MENUMODOMULTIJUGADOR_HPP