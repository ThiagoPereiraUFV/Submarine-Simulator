using namespace std;

//	-------- Tamanho inicial da janela --------
#define WINDOW_WIDTH 500			//	Largura inicial
#define WINDOW_HEIGHT 500			//	Altura inicial
GLsizei view_w = 500, view_h = 500;	//	Variaveis auxiliares de tela

//	----------------- Modelos -----------------
Object3D submarino;	                                    //	Modelo 3D do submarino
vector<Object3D> navio;	                                //	Modelo 3D de navio
vector<Object3D> helicoptero;                           //	Modelo 3D de helicoptero
vector<Object3D> peixe;	                                //	Modelo 3D de peixe
vector<Object3D> tubarao;	                            //	Modelo 3D de tubarao
vector<GLfloat> lDirection;                             //  Luz direcionada
vector<GLfloat> lPosition;                              //  Luz direcionada
const vector<GLfloat> spotlight{0.8, 0.8, 0.8, 1.0};    //  Luz direcionada
const vector<GLfloat> sunlight{0.8, 0.8, 0.8, 1.0};     //  Sol

//	---- Variaveis e constantes auxiliares ----
#define PI 3.1415										//	Valor aproximado de pi
bool started = false, started1 = false;		            //	Armazena se o jogo ja iniciou
const GLfloat c1 = 0.003, c2 = 0.008, c3 = 0.20;		//	Constantes auxiliares
vector<vector<GLfloat>> verticesSea;			        //	Vertices do cubo representando o mar
vector<GLdouble> viewer;     							//	Posicao do observador (camera)
vector<GLdouble> center;     							//	Posicao do foco do observador (camera)
GLdouble rotation = 0;							        //	Variavel de rotacao do submarino
bool upSub = true, upA = true, upShip = true;           //  Variavel auxiliar usada na animacao do submarino
int dispSub = 0, dispA = 0, dispShip = 0;               //  Variavel auxiliar usada na animacao do submarino
bool help = false;                                      //  Variavel de estado para o menu de ajuda
bool drawShips = false;                                 //  Variavel de estado para o desenho dos navios
vector<GLdb3> shipPos;                                  //  Posicao dos navios
vector<GLdb3> heliPos;									//	Posicao dos helicopteros
vector<GLdb3> fishPos;									//	Posicao dos peixes
vector<GLdb3> sharkPos;									//	Posicao dos tubaroes
int nShips = 10;										//	Numero de navios
int nHelis = 7;									        //	Numero de helicopteros
int nSeaAnimals = 20;									//	Numero de animais marinhos
bool fp = false;                                        //  Estado que define o ponto de vista
bool light = true;                                      //  Estado que define se luzes estao ligadas
bool lightMode = false;                                 //  Define o modelo das luzes, 0 Gouroud e 1 Flat Shade
bool l1 = true, l2 = true;                              //  Define se luz 1 ou 2 esta/estao ligada(s)