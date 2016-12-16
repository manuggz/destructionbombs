//
// Created by manuggz on 06/12/16.
//

#ifndef TETRIS_TETRIS_HPP
#define TETRIS_TETRIS_HPP

static const int TETRIS_PLAYER_1 = 1;

static const int N_DIGITOS_ENTEROS = 10;

static const char *const META_DATA_HIGHSCORE = "HighScore";

static const int SINGLE_LINE = 1;

static const int DOUBLE_LINE = 2;

static const int TRIPLE_LINE = 3;

static const int TETRIS = 4;

#include "../../engine/interfaces/InterfazUI.hpp"
#include "../../engine/layout/LayoutManager/LayoutAbsolute.hpp"


#include "../../engine/layout/LayoutManager/LayoutVertical.hpp"
#include "TetrisJuego.hpp"
#include "BitmapFont.hpp"
#include "../../engine/util/CMetaData.hpp"

class TetrisInterfaz : public InterfazUI, public InterfazJuegoTetris {
public:

    TetrisInterfaz(GameManagerInterfazUI *gameManagerInterfaz)
            : InterfazUI(gameManagerInterfaz) {
        std::cout << "TetrisInterfaz::TetrisInterfaz" << std::endl;

    }

    void prepare() override {
        std::cout << "TetrisInterfaz::prepare" << std::endl;
        InterfazUI::prepare();
        mLayoutBackGround          = new LayoutAbsolute();
        mTetrisJuego = new TetrisJuego(this, TETRIS_PLAYER_1, 32, 0, 23, 10);

        mMetaData = new MetaData();
        if(mMetaData->cargarMetaData("resources/settings.db",":")){
            mHighScore = std::stoi(mMetaData->getMetaData(META_DATA_HIGHSCORE));
        }else{
            mHighScore = 0;
        }

        mMusicaFondo = cargar_musica("resources/music/music.mp3");
        mSfxChunkGameOver = cargar_sonido((char *) "resources/music/SFX_GameOver.ogg");
        mSfxChunkGameStart = cargar_sonido((char *) "resources/music/SFX_GameStart.ogg");
        mSfxClearLines[0] = cargar_sonido((char *) "resources/music/SFX_SpecialLineClearSingle.ogg");
        mSfxClearLines[1] = cargar_sonido((char *) "resources/music/SFX_SpecialLineClearDouble.ogg");
        mSfxClearLines[2] = cargar_sonido((char *) "resources/music/SFX_SpecialLineClearTriple.ogg");
        mSfxClearLines[3] = cargar_sonido((char *) "resources/music/SFX_SpecialTetris.ogg");
    }

    void playSfx(Mix_Chunk *pSfxChunk) override {
        mGameManagerInterfaz->play(pSfxChunk);
    }
    void tetrisHardDrop(int tetrisID, int nCells)override {
        if(nCells > 40) nCells = 40;
        mPuntajePlayer +=  2*nCells;
        setTextWithDigits(mBitmapScorePlayer1Valor,mPuntajePlayer,N_DIGITOS_ENTEROS);
    }

    void tetrisSoftDrop(int tetrisID, int nCells) override{
        if(nCells > 20) nCells = 20;
        mPuntajePlayer +=  nCells;
        setTextWithDigits(mBitmapScorePlayer1Valor,mPuntajePlayer,N_DIGITOS_ENTEROS);
    }

    void tetrisLineasCompletadas(int TetrisID, int nLineasCompletadas) override {
        //mLabelComponentScoreActual->setText(std::to_string(nuevoPuntaje));
        char textoDigitalizado[N_DIGITOS_ENTEROS + 1];

        switch(nLineasCompletadas){
            case SINGLE_LINE:
                mPuntajePlayer += 100*mLevelTetrisPlayer;
                break;
            case DOUBLE_LINE:
                mPuntajePlayer += 300*mLevelTetrisPlayer;
                break;
            case TRIPLE_LINE:
                mPuntajePlayer += 500*mLevelTetrisPlayer;
                break;
            case TETRIS:
                if(mLineasCompletasAnteriores == TETRIS){
                    mPuntajePlayer += 1200 * mLevelTetrisPlayer;
                }else{
                    mPuntajePlayer += 800*mLevelTetrisPlayer;
                }
                break;
            default:break; // Help to avoid warnings!
        }

        if(mLineasCompletasAnteriores){
            mPuntajePlayer += 50*mLineasCompletasAnteriores*mLevelTetrisPlayer;
        }
        mLineasCompletasAnteriores = nLineasCompletadas;

        sprintf(textoDigitalizado,"%10d",mPuntajePlayer);
        int i = 0;
        while(textoDigitalizado[i] == ' ')textoDigitalizado[i++]='0';
        mBitmapScorePlayer1Valor->setText(textoDigitalizado);

        std::fill_n(textoDigitalizado,11,0);

        mLineasCompletas += nLineasCompletadas;
        sprintf(textoDigitalizado,"%10d",mLineasCompletas);
        i = 0;
        while(textoDigitalizado[i] == ' ')textoDigitalizado[i++]='0';
        mBitmapLinesPlayer1Valor->setText(textoDigitalizado);

        mGameManagerInterfaz->play(mSfxClearLines[nLineasCompletadas - 1]);
    }

    void start() override {
        std::cout << "TetrisInterfaz::start" << std::endl;
        InterfazUI::start();
        mTetrisJuego->start();
        mControlTimer.start();
        mGameManagerInterfaz->playFadeInSound(mMusicaFondo,MIX_MAX_VOLUME/2);
        mGameManagerInterfaz->play(mSfxChunkGameStart);
    }

    void tetrisPaused(int tetrisID) override {
        mControlTimer.pause();
    }

    void tetrisResumed(int tetrisID) override {
        mControlTimer.resume();
    }

    virtual void createUI(SDL_Renderer *renderer)  override{

        std::cout << "TetrisInterfaz::createUI" << std::endl;
        // Establecemos el fondo con los parametros adecuados para que ocupe toda la pantalla
        mLayoutBackGround->setLayoutParam(LAYOUT_PARAM_FILL_PARENT_HEIGHT,LAYOUT_PARAM_TRUE);
        mLayoutBackGround->setLayoutParam(LAYOUT_PARAM_FILL_PARENT_WIDTH,LAYOUT_PARAM_TRUE);
        mLayoutBackGround->setLayoutParam(LAYOUT_PARAM_WRAP_WIDTH,LAYOUT_PARAM_FALSE);
        mLayoutBackGround->setLayoutParam(LAYOUT_PARAM_WRAP_HEIGHT,LAYOUT_PARAM_FALSE);
        //mLayoutBackGround->setBackgroundColor(SDL_Color {27,63,177,255});
        LTexture * lTexture = new LTexture();
        lTexture->loadFromFile("resources/backgroundSinglePlayer.png",renderer,false);
        mLayoutBackGround->setBackgroundTexture(lTexture);

        /*LabelComponent * labelComponentScore = new LabelComponent();
        labelComponentScore->setText("SCORE:");
        //labelComponentScore->setText(std::to_string(minutosEscogidos));
        labelComponentScore->setFont("resources/fuentes/OpenSans-Bold.ttf",25);
        //labelComponentScore->setTextColor(color);
        labelComponentScore->setLayoutParam(LAYOUT_PARAM_X,"700");
        labelComponentScore->setLayoutParam(LAYOUT_PARAM_Y,"100");

        mLayoutBackGround->addComponent(labelComponentScore);*/

        mBitmapFont = new BitmapFont(renderer,"resources/fuentes/fuente_1.png");

        mBitmapHighScore = new BitmapFontRenderer(mBitmapFont,721,313);
        mBitmapHighScore->setText("HIGHSCORE");

        mBitmapHighScorePlayer1Valor = new BitmapFontRenderer(mBitmapFont,0,355);
        mBitmapHighScorePlayer1Valor->setRight(907);
        setTextWithDigits(mBitmapHighScorePlayer1Valor,mHighScore,N_DIGITOS_ENTEROS);
//        mBitmapHighScorePlayer1Valor->setText("000000000");

        mBitmapScore = new BitmapFontRenderer(mBitmapFont,763,430);
        mBitmapScore->setText("SCORE");

        mBitmapScorePlayer1Valor = new BitmapFontRenderer(mBitmapFont,0,481);
        mBitmapScorePlayer1Valor->setRight(918);
        mBitmapScorePlayer1Valor->setText("0000000000");

        mBitmapTime = new BitmapFontRenderer(mBitmapFont,622,576);
        mBitmapTime->setText("TIME");

        mBitmapTimePlayer1Valor = new BitmapFontRenderer(mBitmapFont,0,576);
        mBitmapTimePlayer1Valor->setRight(970);
        mBitmapTimePlayer1Valor->setText("00:00:00");

        mBitmapLevel = new BitmapFontRenderer(mBitmapFont,628,639);
        mBitmapLevel->setText("LEVEL");

        mBitmapLevelPlayer1Valor = new BitmapFontRenderer(mBitmapFont,628,639);
        mBitmapLevelPlayer1Valor->setRight(970);
        mBitmapLevelPlayer1Valor->setText("0000000001");

        mBitmapLines = new BitmapFontRenderer(mBitmapFont,627,703);
        mBitmapLines->setText("LINES");

        mBitmapLinesPlayer1Valor = new BitmapFontRenderer(mBitmapFont,627,703);
        mBitmapLinesPlayer1Valor->setRight(970);
        mBitmapLinesPlayer1Valor->setText("0000000000");

        /*mLabelComponentScoreActual = new LabelComponent();
        mLabelComponentScoreActual->setText("0");
        //labelComponentScore->setText(std::to_string(minutosEscogidos));
        mLabelComponentScoreActual->setFont("resources/fuentes/OpenSans-Bold.ttf",25);
        //labelComponentScore->setTextColor(color);
        mLabelComponentScoreActual->setLayoutParam(LAYOUT_PARAM_X,"800");
        mLabelComponentScoreActual->setLayoutParam(LAYOUT_PARAM_Y,"100");

        mLayoutBackGround->addComponent(mLabelComponentScoreActual);*/
        mTetrisJuego->crearUI(renderer);
        SDL_ShowCursor(SDL_DISABLE);//ocultamos el cursor

    }

    void setTextWithDigits(BitmapFontRenderer * bitmapFontRenderer,int valor,int nDigitos){
        char textoDigitalizado[nDigitos + 1];

        sprintf(textoDigitalizado,"%*d",nDigitos,valor);

        int i = 0;
        while(textoDigitalizado[i] == ' ')textoDigitalizado[i++]='0';
        bitmapFontRenderer->setText(textoDigitalizado);

    }

    void tetrisGameOver(int tetrisID) override {
        mGameManagerInterfaz->play(mSfxChunkGameOver);
    }
    void tetrisRetry(int tetrisID) override {
        std::cout << "TetrisInterfaz::tetrisRetry(" << tetrisID << ")" << std::endl;
        mTetrisJuego->reset();
        if(actualizarHighScore()){
            setTextWithDigits(mBitmapHighScorePlayer1Valor,mHighScore,N_DIGITOS_ENTEROS);
        }
        mPuntajePlayer = 0;
        mLineasCompletas = 0;
        mLineasCompletasAnteriores = 0;
        mLevelTetrisPlayer = 1;

        mBitmapLinesPlayer1Valor->setText("0000000000");
        mBitmapLevelPlayer1Valor->setText("0000000001");
        mBitmapScorePlayer1Valor->setText("0000000000");

        //mControlTimer.stop();
        mControlTimer.start();
        mGameManagerInterfaz->play(mSfxChunkGameStart);
    }

    void tetrisEnd(int tetrisID) override {
        actualizarHighScore();
        mGameManagerInterfaz->goBack();
    }

    bool actualizarHighScore(){
        if(mPuntajePlayer > mHighScore){
            mHighScore = mPuntajePlayer;
            mMetaData->setMetaData(META_DATA_HIGHSCORE,std::to_string(mHighScore));
            mMetaData->guardar("resources/settings.db",":");
            return true;
        }

        return false;

    }
    void nuevoTetrominoSiguiente(Tetromino *nuevoTetrominoSiguiente) override {
        mTetrominoSiguiente = nuevoTetrominoSiguiente;
        mTetrominoSiguiente->move(775,43);
    }

    virtual void resume() override {
        std::cout << "TetrisInterfaz::resume" << std::endl;
        InterfazUI::resume();
        mLayoutBackGround->setDisabled(true);
        SDL_ShowCursor(SDL_DISABLE);//ocultamos el cursor
    }


/**
         * Procesa el evento del usuario
         * Se encarga de mover la opcion resaltada al usuario o en caso de que sea ENTER llamar a la funcion enlazada a
         * la opcion.
         * @param evento  Evento producido por SDL
         */
    void procesarEvento(SDL_Event *evento) override {
        if(evento->type==SDL_KEYDOWN) {
            switch (evento->key.keysym.sym) {
                case SDLK_ESCAPE:
                    actualizarHighScore();
                    mGameManagerInterfaz->goBack();
                    //mGameManagerInterfaz->showPopUp()
                    break;
                default:
                    break;
            }
        }
    }

    void stop() override {
        InterfazUI::stop();
        mMetaData->guardar("resources/settings.db",":");
    }

    void update() override {
        InterfazUI::update();
        mTetrisJuego->update();
    }

    void draw(SDL_Renderer *renderer) override {
        if(mLayoutBackGround->isDisabled()){
            SDL_Rect rect = mGameManagerInterfaz->getRectScreen();
            mLayoutBackGround->pack(renderer);
            mLayoutBackGround->setRectDibujo(rect);
        }
        mLayoutBackGround->draw(renderer);
        mBitmapScore->draw(renderer);
        mBitmapHighScore->draw(renderer);
        mBitmapTime->draw(renderer);
        mBitmapLevel->draw(renderer);
        mBitmapLines->draw(renderer);
        mBitmapHighScorePlayer1Valor->draw(renderer);
        mBitmapScorePlayer1Valor->draw(renderer);

        if(mControlTimer.isRunning()){
            static char hrs[3],min_[3],seg[3],tiempo[6];

            int segundos = mControlTimer.getTicks()/1000;

            sprintf(seg,"%2d",(segundos) % 60);
            if(seg[0]==' ')seg[0]='0';

            int minutos = (segundos - segundos%60)/60;
            sprintf(min_,"%2d",minutos%60);
            if(min_[0]==' ')min_[0]='0';

            int horas = (minutos - minutos%60)/60;
            sprintf(hrs,"%2d",horas);
            if(hrs[0]==' ')hrs[0]='0';

            sprintf(tiempo,"%s:%s:%s",hrs,min_,seg);
            mBitmapTimePlayer1Valor->setText(tiempo);

        }
        mBitmapTimePlayer1Valor->draw(renderer);
        mBitmapLevelPlayer1Valor->draw(renderer);
        mBitmapLinesPlayer1Valor->draw(renderer);

        mTetrisJuego->draw(renderer);

        if(mTetrominoSiguiente != nullptr) mTetrominoSiguiente->draw(renderer);
    }

    virtual ~TetrisInterfaz() override {
        delete mLayoutBackGround; // Al liberar el layout parent se liberan todos sus mComponentes
        delete mTetrisJuego;
        //delete mLabelComponentScoreActual;
        delete mBitmapScore;
        delete mBitmapHighScore;
        delete mBitmapTime;
        delete mBitmapLevel;
        delete mBitmapLines;
        delete mBitmapHighScorePlayer1Valor;
        delete mBitmapScorePlayer1Valor;
        delete mBitmapTimePlayer1Valor;
        delete mBitmapLevelPlayer1Valor;
        delete mBitmapLinesPlayer1Valor;
        delete mBitmapFont;
        Mix_FreeMusic(mMusicaFondo);
        Mix_FreeChunk(mSfxChunkGameOver);
        Mix_FreeChunk(mSfxChunkGameStart);

        for(int i = 0; i < 4;i++){
            Mix_FreeChunk(mSfxClearLines[i]);
        }
    }

private:

    LayoutAbsolute *mLayoutBackGround = nullptr;

    TetrisJuego * mTetrisJuego = nullptr;
    //LabelComponent * mLabelComponentScoreActual;
    BitmapFontRenderer *mBitmapScore;
    BitmapFontRenderer *mBitmapHighScore;
    BitmapFontRenderer *mBitmapTime;
    BitmapFontRenderer *mBitmapLevel;
    BitmapFontRenderer *mBitmapLines;
    BitmapFontRenderer *mBitmapHighScorePlayer1Valor;
    BitmapFontRenderer *mBitmapScorePlayer1Valor;
    BitmapFontRenderer *mBitmapTimePlayer1Valor;
    BitmapFontRenderer *mBitmapLevelPlayer1Valor;
    BitmapFontRenderer *mBitmapLinesPlayer1Valor;
    BitmapFont *mBitmapFont;
    Tetromino * mTetrominoSiguiente = nullptr;
    LTimer mControlTimer;

    int mPuntajePlayer = 0;
    int mLineasCompletas = 0;
    MetaData *mMetaData;
    int mHighScore;

    Mix_Music * mMusicaFondo;
    Mix_Chunk * mSfxChunkGameStart;
    Mix_Chunk * mSfxChunkGameOver;

    Mix_Chunk * mSfxClearLines[4];
    int mLevelTetrisPlayer = 1;
    int mLineasCompletasAnteriores = 0;
};

#endif //TETRIS_TETRIS_HPP