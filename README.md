# Máquina De Escrever 3D OpenGL com GLUT 

## Funcionalidades

F1 : Move a parte móvel para a direita.

F2 : Move a folha para cima. (Rotaciona a roda da direita)

F3 : Move a folha para baixo. (Rotaciona a roda da direita)


Setas do teclado ajustam a angulação da câmera.

Teclas númericas e alfabéticas são inseridas na folha da Máquina De Escrever,  ao teclar, a "fita" de tinta conectada aos carretéis sobe junto com a haste de metal, o respectivo carimbo da tecla acionada é rotacionado em direção ao triângulo central, como em uma Máquina De Escrever real, a tecla pressionada afunda e a parte móvel da folha se movimenta para a esquerda.

## Dependências

Instale GLEW e GLUT para compilar o projeto, caso tenha dificuldades para compilar no Windows, instale o WSL para emular o ambiente Linux no Windows, instale GLEW e GLUT lá e compile.

Ou baixe o MSYS e instale o MINGW64 e abra o MSYS MINGW64, execute o comando ''' pacman -S mingw-w64-x86_64-glew mingw-w64-x86_64-glfw mingw-w64-x86_64-freeglut mingw-w64-x86_64-glm ''' para compilar use '''  g++ MaquinaDeEscrever3D.cpp -o MaquinaDeEscrever3D.exe -I/mingw64/include -L/mingw64/lib -lglew32 -lglfw3 -lfreeglut -lopengl32 -lglu32 ''' 

Por fim rode o programa ''' ./MaquinaDeEscrever3D.exe '''




