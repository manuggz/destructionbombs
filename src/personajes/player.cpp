#include <deque>
#include "player.hpp"
#include "bomba.hpp"
#include "../engine/util/LTimer.hpp"
#include "../niveles/LectorMapa.hpp"
#include "../objetos/explosion.hpp"


/**
 * Inicializa lo minimo necesario de la clase
 * @param interfazGaleria
 * @param id
 */
Player::Player(ModoJuegoMultiPlayer * juego,IdPlayer id){
    //cout << "Constructor de Player:"<<this<<endl;

    mpJuego  = juego;

    this->mPlayerId = id;

    // Tama#o del rectangulo el cual se usará para detectar la colision del player con su entorno
    rect.w = W_COLISION;
    rect.h = H_COLISION;

    cargarTeclas();
}

void Player::cargarRecursos(SDL_Renderer *gRenderer){
    mpSpriteSheetPlayer = new SpriteSheet(gRenderer, "data/imagenes/personajes/player_" + std::to_string(mPlayerId + 1) + ".bmp", 1, 12,true);
    mpSpriteSheetPlayerEstadoMuriendo = new SpriteSheet(gRenderer, "data/imagenes/personajes/player_" + std::to_string(mPlayerId + 1) + "_muriendo.bmp", 1, 4,true);

}
/**
 * Carga la configuracion del teclado del player
 */
void Player::cargarTeclas(){
    char tmp_ruta[40];
    sprintf(tmp_ruta,"data/configuracion/teclado_%d.dat",mPlayerId+1);

    if(!control.cargar(tmp_ruta)){//si no se puede cargar de un archivo
        //se asignan teclas por default
        //control.setDefaultKeys(mPlayerId);
    }
}


/**
 * Actualiza al posicion del rectangulo de colision de acuerdo a las coordenadas X Y del personaje
 */
void Player::updateRectColision(){
    //actualiza el cuadro que representa al personaje en la colision
    rect.x = x + X_COLISION;
    rect.y = y + Y_COLISION;
}

/**
 * Actualiza al player
 * @param teclas
 */
void Player::update(const Uint8 * teclas){

    // Avanzamos la animacion
	avanzarAnimacion ();
    updateRectColision();

    // Dependiendo del estado actual del player llamamos a la funcion encargada de dicho estado
	switch (estado_actual){
        case EstadoSprite::PARADO:
		    parado (teclas);
			break;
		case EstadoSprite::IZQUIERDA:
			izquierda(teclas);
			break;
		case EstadoSprite::DERECHA:
			derecha(teclas);
			break;
		case EstadoSprite::ARRIBA:
			arriba(teclas);
			break;
		case EstadoSprite::ABAJO:
			abajo(teclas);
			break;
		case EstadoSprite::MURIENDO:break;
        default:break;
    }

    if(estado_actual != EstadoSprite::MURIENDO){
        // Si el estado es distinto de mueriendo, entonces el player esta activo en el juego
        // En ese caso una vez actualizado su estado se deben detectar colisiones con el entorno
        // Recordar que en mover_ip solo se detectaron colisiones que podian detener el movimiento
        // una vez realizado el movimiento hay que detectar si colisiona por ejemplo con un item
        if(!mEstaProtegido){//si no esta protegido

            // DETECTAMOS COLISIONES CON LAS EXPLOSIONES
            auto setColisionExplosiones = mpJuego->colisionConExplosiones(rect);

            auto pSpriteExplosion = setColisionExplosiones.begin();
            Player *mSpriteCausanteMuerte;
            if(pSpriteExplosion  != setColisionExplosiones.end()){
                // Notar que solo tomamos en cuenta la primera explosion
                // Esto es porque una vez que se colisiona con una explosion o se muere o se activa la proteccion

                // Decimos HEy! este es quien nos ha matado
                mSpriteCausanteMuerte = dynamic_cast<Explosion *>(*pSpriteExplosion)->getCreador();
                // Dejamos que el Juego se encarge de nosotros
                mpJuego->playerMuerto(this,mSpriteCausanteMuerte);
            }

        }else{//si esta protegido
            if(mTimer.getTicks() / 1000 >= mDuracionProteccion){
                mEstaProtegido=false;
            }
        }

        auto setColisionItems  = mpJuego->colisionConItems(rect);
        if(setColisionItems.size() > 0) {
            auto pItem = setColisionItems.begin();
            while (pItem != setColisionItems.end()) {
                (dynamic_cast<Item * >(*pItem))->setPlayerActivador(this);
                (*pItem)->kill(); // Dejamos que el Juego encargado de las eliminaciones se encargue del resto
                pItem++;
            }
            return;
        }

    }//fin "si no esta muriendo"


}

void Player::activarPoderItem(Item::TipoItem tipo){
    switch(tipo){
        case Item::ITEM_ALCANCE:
            if(mAlcanBombas < N_MAX_ALCANCE_EXPLOSION)
                mAlcanBombas++;
            break;
        case Item::ITEM_VIDA:
            mVidas++;
            break;
        case Item::ITEM_BOMBA:
            if(mNBombasDisponibles < N_MAX_BOMBAS)
                mNBombasDisponibles++;
            break;
        case Item::ITEM_ATRAVIESA_PAREDES:
            mPuedeAtravesarBloques=true;
            break;
        case Item::ITEM_ALEATORIO:
            //activarPoderItem(juego->getTipoNuevoItem(false));
            break;
        case Item::ITEM_BOMBA_MAX:
            mNBombasDisponibles = N_MAX_BOMBAS;
            break;
        case Item::ITEM_ALCANCE_MAX:
            mAlcanBombas        = N_MAX_ALCANCE_EXPLOSION;
            break;
        case Item::ITEM_PROTECCION:
            setProteccion(20);
            break;
        case Item::ITEM_BOLA_ARROZ:
            //setPuntaje(getPuntaje()+100);
            break;
        case Item::ITEM_PASTEL:
            //setPuntaje(getPuntaje()+120);
            break;
        case Item::ITEM_PALETA:
            //setPuntaje(getPuntaje()+120);
            break;
        case Item::ITEM_BARQUILLA:
            //setPuntaje(getPuntaje()+50);
            break;
        case Item::ITEM_MANZANA:
            //setPuntaje(getPuntaje()+250);
            break;
        case Item::ITEM_PATINETA:
            mVelocidad=2;
            break;
        case Item::ITEM_CORAZON:
            mCorazones++;
            break;
        case Item::ITEM_ATRAVIESA_BOMBAS:
            mPuedeAtravesarBombas=true;
            break;
        case Item::ITEM_PATEA_BOMBA:
            mPuedePatearBombas=true;
            break;
        default:
            break;
        }
}

void Player::draw(SDL_Renderer * gRenderer){
    if(estado_actual!=MURIENDO) {
        if(mEstaProtegido){
            mpSpriteSheetPlayer->setAlpha(120);
        }else{
            mpSpriteSheetPlayer->setAlpha(255);
        }
        mpSpriteSheetPlayer->setCurrentCuadro(cuadro);
        mpSpriteSheetPlayer->draw(gRenderer,x,y);
    }else {
        mpSpriteSheetPlayerEstadoMuriendo->setCurrentCuadro(cuadro);
        mpSpriteSheetPlayerEstadoMuriendo->draw(gRenderer,x,y);
    }
#ifdef DEBUG
    /*DIBUJA EL CUADRO QUE REPRESENTA LA COLISION DEL PERSONAJE*/
            updateRectColision();
            SDL_FillRect(screen,&rect,SDL_MapRGB (screen->format, 0, 0, 255));
#endif
}


void Player::ponerBomba(const Uint8 * teclas){
    TipoSprite conjun_coli[]={BOMBA,GLOBO,NIVEL,ITEM};

    updateRectColision();
	if(!mMantieneAccionPresionado&&isPressed(ControlPlayer::TECLA_ACCION,teclas)
       &&mNBombasColocadas < mNBombasDisponibles){

        Bomba *pBombaColocada = mpJuego->agregarBomba(this);
           /*si se logro agregar*/
           if(pBombaColocada != nullptr){
               mpUltimaBomba = pBombaColocada;
               mNBombasColocadas++;
            }
        }

	mMantieneAccionPresionado = isPressed(ControlPlayer::TECLA_ACCION,teclas);
}

bool Player::colision(SDL_Rect & rect){
    updateRectColision();
    return rects_colisionan(this->rect,rect);
}

void Player::parado(const Uint8 * teclas) {
        if(isPressed(ControlPlayer::TECLA_ARRIBA,teclas))
            cambiarEstado(ARRIBA);
    
        if(isPressed(ControlPlayer::TECLA_ABAJO,teclas))
            cambiarEstado (ABAJO);
    
        if(isPressed(ControlPlayer::TECLA_IZQUIERDA,teclas))
            cambiarEstado (IZQUIERDA);
    
        if(isPressed(ControlPlayer::TECLA_DERECHA,teclas))
            cambiarEstado(DERECHA);

        ponerBomba(teclas);

}

void Player::izquierda(const Uint8 * teclas) {
	mover_ip(-mVelocidad,0);
	if(!isPressed(ControlPlayer::TECLA_IZQUIERDA,teclas))
    	cambiarEstado(PARADO);

	ponerBomba(teclas);
}

void Player::derecha (const Uint8 * teclas) {
	mover_ip(mVelocidad,0);
	if(!isPressed(ControlPlayer::TECLA_DERECHA,teclas))
    	cambiarEstado ( PARADO);

	ponerBomba(teclas);
}


bool Player::isPressed(ControlPlayer::TeclaPlayer tecla, const Uint8 * _teclas){
    if(!_teclas)return false;
    if(!control.isBotonJoystick(tecla) && !control.isDireccionJoystick(tecla)){
        return _teclas[SDL_GetScancodeFromKey(control.getKey(tecla))];

    }else{

        static char temp[33];
        SDL_Joystick * pjoy;
        for(int i=0;i<mpJuego->getJoysActivos();i++){

            pjoy = mpJuego->getJoy(i);
            if(SDL_JoystickGetAttached(pjoy)){
                SDL_JoystickGUID guidj = SDL_JoystickGetGUID(pjoy);
                SDL_JoystickGetGUIDString(guidj,temp,33);

                if(!strcmp(temp,control.getJoystickGUID(tecla))){//si coincide con el joistick con el que se configuro
                    if(control.isDireccionJoystick(tecla))
                        return estado_direccion_joy(control.getKey(tecla),pjoy);
                    else{
                        return SDL_JoystickGetButton(pjoy, control.getJoybuttonMapping(tecla));
                    }
                }
            }
         }
         return false;
     }
}
void Player::arriba (const Uint8 * teclas) {
    mover_ip (0,-mVelocidad);
	if(!isPressed(ControlPlayer::TECLA_ARRIBA,teclas))
    	cambiarEstado(PARADO);
	ponerBomba(teclas);
}

void Player::abajo(const Uint8 * teclas) {
	mover_ip(0,mVelocidad);
	if(!isPressed(ControlPlayer::TECLA_ABAJO,teclas))
    	cambiarEstado (PARADO);

	ponerBomba(teclas);
}

/*
 * modifica el cuadro de la animaci�n que se debe mostrar en pantalla
 */
void Player::avanzarAnimacion () {
	static int animaciones [_ESTADOS_ANIMACION][17] = {\
		{3,3, 4,4, 5,5,-1},\
		{9,9, 10,10, 11,11, -1},\
		{6,6,  7,7, 8,8,-1},\
		{0,0, 1,1, 2,2,-1},\
        {0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,-1}};
	if (--delay < 1) {
		delay = 3;
		if (animaciones [estado_actual] [paso + 1] == -1){
		    if(estado_actual!=MURIENDO)
    			paso = 0;
    		else
                mpJuego->playerMuerto(this, nullptr);
		}else
			paso ++;
	}


	if(estado_actual!=PARADO)
	   cuadro = animaciones [estado_actual][paso];
	else
	   cuadro = animaciones [estado_anterior][0];

}

void Player::cambiarEstado(EstadoSprite nuevo) {
    estado_anterior = estado_actual;
	estado_actual   = nuevo;
	paso  = 0;
	delay = 3;
}

/**
 * Establece un estado en el que el player es inmune a explosiones
 * @param segundos cantidad de segundos por la que el personaje es inmune
 */
void Player::setProteccion(int segundos){
    mDuracionProteccion=segundos;
    mEstaProtegido=true;
    mTimer.start();
}

/**
 * Mueve al personaje detectando alguna colision
 * Las colisiones que se detectan son solo las que pueden detener el movimiento como colision con una bomba
 * @param incremento_x
 * @param incremento_y
 */
void Player::mover_ip(int incremento_x, int incremento_y) {

    // Movemos el cuadro de colision primero
    rect.x += incremento_x;
    rect.y += incremento_y;

    // Detectamos si colisiona con BOMBAS
    auto setBombasColision = mpJuego->colisionConBombas(rect);
    // assert 0 <= setBombasColision.size() <= 1

    if(setBombasColision.size() > 0){ // Si colisiona con alguna bomba
        auto pBomba = setBombasColision.begin();
        while(pBomba != setBombasColision.end()){
            // Si colisiona con una bomba que no es la ultima colocada no movemos al personaje
            if((*pBomba) != mpUltimaBomba && !mPuedeAtravesarBombas){
                return;
            }
            pBomba++;
        }
    }else{
        // Si no colisiona con una bomba y teniamos una referencia a la ultima bomba la quitamos
        // Esta referencia solo era para permitir al usuario moverse arriba de la bomba que él puso hasta que
        // se salga del cuadro de colision
        mpUltimaBomba= nullptr;
    }

    // Si se sale del mapa no actualizamos la posicion del personajes, nos salimos
    if(mpJuego->isOutOfMapBounds(rect))return;

    /*
     * DETECTAMOS ALGUNA COLISION CON EL MAPA
     */
    // Nos dice cual de los extremos colision con el mapa
    LectorMapa::ExtremoColision extremoQueColisiona;
    int numeroDeColisiones = 0;
    extremoQueColisiona    = mpJuego->colisionConMapa(rect, &numeroDeColisiones, mPuedeAtravesarBloques);

    if(extremoQueColisiona != LectorMapa::ExtremoColision::NINGUNO){

        // Si colision en una sola esquina entonces deslizamos un poco al personaje para que le sea más facil
        // manejarse por el mapa :)
        if(numeroDeColisiones == 1){
            if(estado_actual == EstadoSprite::IZQUIERDA){
                if(extremoQueColisiona == LectorMapa::ExtremoColision::TOPLEFT){
                    y += 1; // Hacia Abajo
                }else if(extremoQueColisiona == LectorMapa::ExtremoColision::BOTTOMLEFT){
                    y -= 1; // Hacia Arriba
                }
            }else if (estado_actual == EstadoSprite::DERECHA){
                if(extremoQueColisiona == LectorMapa::ExtremoColision::TOPRIGHT){
                    y += 1;// Hacia Abajo
                }else if(extremoQueColisiona == LectorMapa::ExtremoColision::BOTTOMRIGHT){
                    y -= 1;// Hacia Arriba
                }
            }else if(estado_actual == EstadoSprite::ARRIBA){
                if(extremoQueColisiona == LectorMapa::ExtremoColision::TOPLEFT){
                    x += 1; // Hacia la derecha
                }else if(extremoQueColisiona == LectorMapa::ExtremoColision::TOPRIGHT){
                    x -= 1;// Hacia la izquierda
                }
            }else if(estado_actual == EstadoSprite::ABAJO){
                if(extremoQueColisiona == LectorMapa::ExtremoColision::BOTTOMRIGHT){
                    x -= 1;// Hacia la izquierda
                }else if(extremoQueColisiona == LectorMapa::ExtremoColision::BOTTOMLEFT){
                    x += 1;// Hacia la derecha
                }
            }
        }
    }else{
        move(x+incremento_x,y+incremento_y);
    }
}

/**
 * Establece al personaje en la posicion indicada
 * No actualiza su rectangulo de colision
 * @param x
 * @param y
 */
void Player::move(int x,int y){
    //establece al jugador en la posicion indicada
    this->x=x;
    this->y=y;
}

Player::~Player(){
    //cout << "Destructor de Player:"<<this<<endl;
    delete mpSpriteSheetPlayer;
    delete mpSpriteSheetPlayerEstadoMuriendo;
}

void Player::setNBombas(int nBombas) {
    mNBombasDisponibles = nBombas;
}

void Player::setAlcanceBombas(int alcanceBombas) {
    mAlcanBombas = alcanceBombas;

}

int Player::getBombasColocadas() {
    return mNBombasColocadas;
}

void Player::setBombasColocadas(int n) {
    mNBombasColocadas = n;
}

int Player::getNCorazones() {
    return mCorazones;
}

EstadoSprite Player::getEstado() {
    return estado_actual;
}

void Player::setNCorazones(int nuevosNCorazones) {
    mCorazones = nuevosNCorazones;

}

bool Player::isMPuedeAtravesarBloques() const {
    return mPuedeAtravesarBloques;
}

void Player::setMPuedeAtravesarBloques(bool mPuedeAtravesarBloques) {
    Player::mPuedeAtravesarBloques = mPuedeAtravesarBloques;
}

bool Player::isMPuedeAtravesarBombas() const {
    return mPuedeAtravesarBombas;
}

void Player::setMPuedeAtravesarBombas(bool mPuedeAtravesarBombas) {
    Player::mPuedeAtravesarBombas = mPuedeAtravesarBombas;
}

bool Player::isMPuedePatearBombas() const {
    return mPuedePatearBombas;
}

void Player::setMPuedePatearBombas(bool mPuedePatearBombas) {
    Player::mPuedePatearBombas = mPuedePatearBombas;
}

bool Player::isMPuedeGolpearBombas() const {
    return mPuedeGolpearBombas;
}

void Player::setMPuedeGolpearBombas(bool mPuedeGolpearBombas) {
    Player::mPuedeGolpearBombas = mPuedeGolpearBombas;
}

bool Player::isMEstaEnfermo() const {
    return mEstaEnfermo;
}

void Player::setMEstaEnfermo(bool mEstaEnfermo) {
    Player::mEstaEnfermo = mEstaEnfermo;
}

void Player::setVelocidad(int nuevaVelocidad) {
    mVelocidad = nuevaVelocidad;
}

