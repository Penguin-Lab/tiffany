import pygame
from is_wire.core import Channel, Message

channel = Channel('amqp://guest:guest@10.10.2.211:30000')

pygame.init()
window = pygame.display.set_mode((300, 300))
clock = pygame.time.Clock()

rect = pygame.Rect(0, 0, 20, 20)
rect.center = window.get_rect().center
vel = 2

Up = pygame.K_UP
Down = pygame.K_DOWN
Right = pygame.K_RIGHT
Left = pygame.K_LEFT

def publica_vel(canal,modulo,angulo):
    img_msg = Message()
    img_msg.body = (str(modulo)+","+str(angulo)).encode("utf-8")
    canal.publish(img_msg, topic='tiffany.vel')
    print("tiffany.vel",modulo,angulo)

run = True
while run:
    clock.tick(60)
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            run = False
        # if event.type == pygame.KEYDOWN:
            # print(pygame.key.name(event.key))

    keys = pygame.key.get_pressed()
    lista = [keys[pygame.K_w],keys[pygame.K_s],keys[pygame.K_d],keys[pygame.K_a],keys[pygame.K_i],keys[pygame.K_p],keys[pygame.K_q],keys[pygame.K_e]]
    if(any(lista)):
        if lista[4]:      # START
            publica_vel(channel,-10,0)
        elif lista[5]:    # SHUTDOWN
            publica_vel(channel,-10,180)
        elif(lista[6]):   # GIRA ESQUERDA
            publica_vel(channel,2,90+90)
        elif(lista[7]): # GIRA DIREITA
            publica_vel(channel,2,-90+90)
        elif(lista[0]):       # FRENTE
            if(lista[2]):   # DIREITA
                publica_vel(channel,5,-45+90)
            elif(lista[3]): # ESQUERDA
                publica_vel(channel,5,45+90)
            else:           # DIRECAO
                publica_vel(channel,5,0+90)
        elif(lista[1]):     # TRAS
            if(lista[2]):   # DIREITA
                publica_vel(channel,5,-135+90)
            elif(lista[3]): # ESQUERDA
                publica_vel(channel,5,135+90)
            else:           # DIRECAO
                publica_vel(channel,5,180+90)
        elif(lista[2]):     # DIREITA
            publica_vel(channel,5,-90+90)
        else:               # ESQUERDA
            publica_vel(channel,5,90+90)
    else:                   # PARAR
        publica_vel(channel,0,0)
    rect.x += (keys[pygame.K_d] - keys[pygame.K_a]) * vel
    rect.y += (keys[pygame.K_s] - keys[pygame.K_w]) * vel
        
    rect.centerx = rect.centerx % window.get_width()
    rect.centery = rect.centery % window.get_height()

    window.fill(0)
    pygame.draw.rect(window, (10, 186, 181), rect)
    pygame.display.flip()

pygame.quit()
exit()
