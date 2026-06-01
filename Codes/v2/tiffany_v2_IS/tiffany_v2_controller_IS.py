import numpy as np
import pygame
import math
import time
from is_msgs.common_pb2 import Pose
from is_wire.core import Channel, Message, Subscription

channel = Channel('amqp://guest:guest@10.10.2.211:30000')
subscription = Subscription(channel=channel)
subscription.subscribe(f'Tiffany.Pose')

pygame.init()
window = pygame.display.set_mode((300, 300))
clock = pygame.time.Clock()

rect = pygame.Rect(0, 0, 20, 20)
rect.center = window.get_rect().center
vel = 2

class Point():
    def __init__(self, x=0.0, y=0.0, z=0.0, yaw=0.0):
        self.x = x
        self.y = y
        self.z = z
        self.yaw = yaw

# Variáveis
poseAnterior = Point(0.0,0.0,0.0,-1.0)
count = 0
pwmx, pwmy = 0, 0
kp, ki = 1, 0.1
Ix, Iy = 0, 0
tempoa = time.time()

# def controllerAlpha(pose,point,kp,ki,poseAnterior=None):
#     global integral, Ix, Iy, tempoa
#     tempo = time.time()
#     deltat = tempo - tempoa
#     if deltat < 0.001: deltat = 0.001
    
#     ex = point.x - pose.x
#     ey = point.y - pose.y

#     Ix += ex*ki*deltat
#     Iy += ey*ki*deltat
    
#     pwmx = ex*kp + Ix
#     pwmy = ey*kp + Iy

#     rho = np.sqrt(ex**2 + ey**2)
#     theta = np.rad2deg(np.arctan2(pwmy, pwmx))
#     theta = int(theta)

#     return rho, theta

def controllerPID(pose,point,kp,ki):
    global integral, Ix, Iy, tempoa
    tempo = time.time()
    deltat = tempo - tempoa
    if deltat < 0.001: deltat = 0.001
    
    ex = point.x - pose.x
    ey = point.y - pose.y

    Ix += ex*ki*deltat
    Iy += ey*ki*deltat
    
    pwmx = ex*kp + Ix
    pwmy = ey*kp + Iy

    rho = np.sqrt(ex**2 + ey**2)
    theta = np.rad2deg(np.arctan2(pwmy, pwmx))
    while theta > 180: theta -= 360
    while theta < -180: theta += 360

    return rho, theta

def controller(pose,point):
    ex = point.x - pose.x
    ey = point.y - pose.y
    rho = np.sqrt(ex**2 + ey**2)
    alpha = np.rad2deg(np.arctan2(ey,ex))
    theta = alpha - pose.yaw
    while theta > 180: theta -= 360
    while theta < -180: theta += 360
    return rho, theta

def publica_vel(canal,modulo,angulo):
    img_msg = Message()
    img_msg.body = (str(modulo)+","+str(angulo)).encode("utf-8")
    canal.publish(img_msg, topic='tiffany.vel')
    print("tiffany.vel",modulo,angulo)

def consumePose(channel):
    try:
        msg = channel.consume(timeout=5.0)
    except:
        return None
    if isinstance(msg,bool):
        return None
    else:
        point = msg.unpack(Pose)
        return Point(point.position.x,point.position.y,point.position.z,point.orientation.yaw)

point = Point(0.0,0.0)
run = True
publica_vel(channel,-10,0)
time.sleep(5)

while run:
    clock.tick(60)
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            run = False

    #LE MIGUEL
    # pose = Point(0.0,0.0,0.0,-1.0)
    pose = consumePose(channel)
    if pose is None:
        continue
    print(pose)

    # Controlador
    rho, theta = controller(pose,point)
    # rho, theta = controllerPID(pose,point,kp,ki)
    # Comando
    if rho >= 0.1:
        publica_vel(channel,5,theta+90)
    else:
        publica_vel(channel,0,0)
    
    rect.x += np.cos(theta*math.pi/180.0) * vel
    rect.y += np.sin(theta*math.pi/180.0) * vel
        
    rect.centerx = rect.centerx % window.get_width()
    rect.centery = rect.centery % window.get_height()

    window.fill(0)
    pygame.draw.rect(window, (10, 186, 181), rect)
    pygame.display.flip()


publica_vel(channel,0,0)
time.sleep(2)
publica_vel(channel,-10,180)
pygame.quit()
exit()
