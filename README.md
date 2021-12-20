# tiffany
A Tiffany é uma hexapod desenvolvida para andar de forma autônoma ou para ser controlada pelo celular. Para se locomover, ela possui 6 patas de 3 graus de liberdade cada (3 motores) e uma ESP32 que comanda o conjunto! Ela é a mais nova integrante robótica do nosso espaço inteligente!

# Estrutura física
A estrutura física do robô é composta por seis patas de três eixos cada (três motores). As patas são fixadas a duas placas de PLA para deixar o robô firme. As peças são todas projetadas em 3D e foram impressas utilizando PLA, podendo ser encontradas na pasta Pecas_3d deste projeto.

<img src="https://github.com/Penguin-Lab/tiffany/blob/master/images/tiffany0.jpeg" width="300"> <img src="https://github.com/Penguin-Lab/tiffany/blob/master/images/tiffany1.jpeg" width="300"> <img src="https://github.com/Penguin-Lab/tiffany/blob/master/images/tiffany2.jpeg" width="300"> <img src="https://github.com/Penguin-Lab/tiffany/blob/master/images/tiffany3.jpeg" width="300">

## Peças 3D em STL
As peças em 3D desenvolvidas para este projeto foram baseadas no projeto do hexapod Steve da Eng. Eletricista Bruna Reis Lyra. Na pasta Pecas_3d, estão listadas essas peças em STL. Para ajudar, segue abaixo uma lista de todas as peças que devem ser impressas e qual o valor da sua maior dimensão (para ajudar na escolha da impressora):
* Corpo
  * 1x TIFFANY_BODY_DOWN.STL (230 mm)
  * 1x TIFFANY_BODY_UP.STL (230 mm)
* Patas
  * 3x TIFFANY_TIBIA_RIGHT.STL (141 mm)
  * 3x TIFFANY_TIBIA_RIGHT.STL espelhado (141 mm)
  * 6x TIFFANY_OMBRO_UP.STL (61 mm)
  * 3x TIFFANY_OMBRO_DOWN_RIGHT.STL (61 mm)
  * 3x TIFFANY_OMBRO_DOWN_RIGHT.STL espelhado (61 mm)
  * 6x TIFFANY_FEMUR_FRONT.STL (110 mm)
  * 6x TIFFANY_FEMUR_BACK.STL (118 mm)
* Motor
  * 12x TIFFANY_TAMPA_MOTOR.STL (41 mm)

Obs: a tampa de 12 motores deverão ser trocadas pelas peças TIFFANY_TAMPA_MOTOR.STL para que seja possível fixar os rolamentos nas patas. Serão utilizados 3 rolamentos por pata:

<img src="https://github.com/Penguin-Lab/tiffany/blob/master/images/pata0.jpeg" width="300">

## Placas de circuito impresso
No projeto, será desenvolvida uma placa de circuito para servir como:
* Shield para a ESP32 e sensores
* Placa de alimentação dos motores

## Componentes eletrônicos
Os componentes eletrônicos embarcados no robô e equipamentos utilizados são:
* Controlador com Wi-fi
  * ESP32 (38 pinos)
* Motores
  * Servo TowerPro MG995 Metálico
* Alimentação
  * Por enquanto, via cabo

# Software
Ainda em desenvolvimento.
