# Screensaver - Paralelización en C++

Este proyecto consiste en un screensaver que utiliza la biblioteca SDL2 para crear un entorno gráfico donde se muestran bolas animadas en movimiento. Las bolas rebotan en las paredes y colisionan entre sí, utilizando algoritmos basados en física y trigonometría para calcular sus trayectorias. El programa admite parámetros de entrada para determinar el número de bolas en la pantalla y utiliza técnicas de paralelización con OpenMP para mejorar el rendimiento.

## Funcionalidades

- Visualización de bolas en movimiento
- Colisiones de bolas y rebotes en las paredes
- Gravedad y fricción
- Reducción de tamaño de las bolas al colisionar
- Cambio de color al colisionar
- Contador de FPS
- Paralelización con OpenMP

## Bibliotecas
Ejecute el programa especificando el número de bolas que desea en la pantalla como argumento:



- SDL2
- SDL2_ttf
- OpenMP


## Compilación
Para compilar el programa, siga los siguientes pasos:

Instale las bibliotecas necesarias

```sh
sudo apt-get install libsdl2-dev libsdl2-ttf-dev libomp-dev
```
Compile el programa usando g++ y enlace las bibliotecas apropiadas:

```sh
g++ screensaverParallel.cpp -o screensaver -lSDL2 -lSDL2_ttf -fopenmp
```
## Ejecución
```sh
./screensaver
```
### Diagrama de flujo
![Diagrama de flujo](https://i.imgur.com/HeCwbjG.png)
