# Submarine Simulator

Developed by Wallace Ferancini Rosa and Thiago Marinho Pereira

Para compilar basta executar o comando a seguir no terminal do Linux: 

make && ./main.out

O que foi implementado - PARTE 1 DO TRABALHO: 

-Setas direcionais para cima e para baixo fazem o submarino imergir ou emergir, respectivamente
-Setas direcionais para direita e para esquerda fazem o submarino mudar a sua direcao
-Botao H e h ativam e desativam o menu de ajuda
-Botao F e f ou I e i mudam a a posicao da camera
-Existem limites para que o submarino nao fique acima do mar ou nao saia do cubo por baixo
-Modelo de agua foi implementado usando um cubo grande de cor azul e transparencia e se move junto do jogador
-Modelo de submarino funcional
-Modelo de cardume de peixes eh carregado no ambiente em posicao aleatoria
-Modelo de navio eh carregado no ambiente em posicao aleatoria
-Modelos sao posicionados de acordo com sua natureza (navios acima da agua e animais marinhos abaixo)ing

O que foi feito a mais - PARTE 1 DO TRABALHO:

-Objetos sao carregados por meio da classe Object3D e sao munidos de atributos e manipulados por metodos
-Variaveis globais e funcoes de manipulacao do ambiente estao separados da main 
-Submarino tem animacao de subida e descida para simular a agua do mar
-Navios, peixes e tubaroes tem animacoes de subida e descida e movimentacao
-Modelo de helicoptero eh carregado no ambiente em posicao aleatoria
-Modelo de tubarao eh carregado no ambiente em posicao aleatoria
-Modelos (exceto navio e submarino) foram usados com poucos triangulos para obter um melhor desempenho
-Modelo de sol foi implementado usando uma esfera de cor amarela e se move junto ao submarino
-Os modelos sao carregados de acordo com o aspecto da janela para que nao haja distorcoes

O que foi implementado - PARTE 2 DO TRABALHO: 

-Iluminacao ambiente foi adicionada e posicionada na mesma posicao do sol
-Iluminacao direcionada foi adicionada e posicionada atras do submarino apontando para frente
-Efeito de textura foi adicionado ao submarino
-Tecla 1 desliga luz ambiente e tecla 2 desliga luz direcionada
-Tecla L ou l desliga o efeito de luz e permanece identico a parte 1 do Trabalho
-Tecla G ou g alterna entre o metodo de iluminacao global de Gouroud ou Flat-Shading

O que foi feito a mais - PARTE 2 DO TRABALHO:

-Ao desligar a luz ambiente o ceu fica em tom de azul escuro e a esfera que representa o sol se torna uma lua

Referencias:

-https://github.com/nothings/stb/blob/master/stb_image.h Biblioteca para carregar imagens

-https://www.freepik.com/free-photos-vectors/background Textura de camuflagem

-https://www.khronos.org/opengl/ - utilizado para consultar algumas duvidas quanto a funcao gluPerspective

-https://www.opengl.org/ - utilizado para consultar a referencia do OpenGL e entender algumas funcoes

-https://www.stackoverflow.com/ - utilizado para consultar algumas duvidas sobre o posicionamento de texto

-Utilizamos como base o arquivo da primeira aula pratica sobre visualizacao 3D e adaptamos ao simulador

-Utilizamos como orientacao os arquivos das praticas sobre iluminacao e textura

-Conversamos com os colegas Lucas, Eliel, Matheus e Caio para sanar algumas duvidas quanto a rotacao do submarino

-Conversamos com os colegas Lucas, Eliel e Caio sobre o posicionamento de luzes e adicao de textura