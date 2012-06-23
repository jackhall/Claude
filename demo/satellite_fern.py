import fernpy as fp
from math import *
import np
import matplotlib.pyplot as plot
from scipy.integrate import odeint
import random as rand

rand.seed(3); #call with no arguments for true randomness

##### ode functions ######
thrust = 1;
def dh(theta, h, control):
	p = fp.point(theta, h)
	mode = control.query(p)
	if mode == 0:
		force = -thrust
	elif mode == 1:
		force = 0
	elif mode == 2:
		force = thrust
	return force

J = 100
def f(state, t, control):
	theta, h = state
	Dtheta = h/J
	Dh = dh(theta, h, control)
	return Dtheta, Dh

###### evaluation functions #######
time = numpy.linspace(0, 10, 1000)
def fitness(individual, state0):
	#simulate
	results = odeint(f, state0, time, individual[0])
	theta = results[:,0]
	h = results[:,1]
	
	#evaluate fitness function
	#individual[1] = 
	
#################################
##### genetic algorithm #########
#################################

#initialize ferns
s = fp.interval(0.0, 1.0)
r = fp.region().set_uniform(s)
population = [(fp.fern(r, 2), 1.0) for i in range(20)] #list of tuple(fern, fitness)

for generation in range(20):
	#evaluate ferns
	for individual in population:
		theta0 = rand.random()*2*pi - pi #between -pi and pi rad
		h0 = rand.random()*J*2 - 0.5 	 #between -1 and 1 rad/s
		state0 = [theta0, h0]
		fitness(individual, state0)
	
	#select parents and breed new population


##### plots ########

