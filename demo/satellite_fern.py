import fernpy as fp
from math import * #for pi
import numpy as np 
import matplotlib.pyplot as plot
from scipy.integrate import odeint
import random as rand

rand.seed(3) #call with no arguments for true randomness

##### ode ######
class OptimalController:
	def query(self, point): 
		"""mimics fern.query to give optimal control of the satellite"""
		theta, h = point[0], point[1]
		#if pass: #<----------
		#	mode = 0
		#elif pass: #<----------
		#	mode = 1
		#else pass: #<----------
		#	mode = 2
		mode = 1
		return mode
		
thrust = 1;
def dh(theta, h, control=None):
	"""computes the derivative of angular momentum (torque thrust)"""
	if control is None:
		control = OptimalController()
	
	p = fp.point()
	p[0], p[1] = theta, h 
	mode = control.query(p) 
	if mode is 0:
		force = -thrust
	elif mode is 1:
		force = 0
	elif mode is 2:
		force = thrust		
	return force

J = 100
def dtheta(theta, h):
	"""computes the angular velocity"""
	return h/J

def f(state, t, control=None):
	"""ode representing a satellite - to be solved numerically"""
	theta, h = state
	Dtheta = dtheta(theta, h)
	Dh = dh(theta, h, control)
	return Dtheta, Dh


###### fitness evaluation #######
def fitness(individual, state0):
	"""procedure to compute evolutionary fitness from a simulation"""
	#simulate
	results = odeint(f, state0, time, individual[0])
	theta = results[:,0]
	
	#evaluate fitness function
	individual[1] = 0.0
	for i in theta: individual[1] += abs(i) #sum of absolute values of error
	
	
#################################
##### genetic algorithm #########
#################################
def evolve():
	"""runs fern genetic algorithm and returns final population"""
	#initialize ferns
	r = fp.region()
	r[0], r[1] = fp.interval(-pi, pi), fp.interval(-2.0*J, 2.0*J)

	population = [(fp.fern(r, 2), 1.0) for i in range(20)] #list of tuple(fern, fitness)
	time = np.linspace(0, .1, 100)

	for generation in range(20):
		#evaluate ferns
		for individual in population:
			theta0 = rand.random()*2*pi - pi #between -pi and pi rad
			h0 = rand.random()*J*2 - 0.5 	 #between -1 and 1 rad/s
			state0 = [theta0, h0]
			fitness(individual, state0)
	
		#select parents and breed new population


	##### plots ########
	
	return population


def plot_phase(state0 = None, control=None):
	"""generates a phase portrait and phase trajectory from random initial conditions"""
	time = np.linspace(0, 1, 100)
	
	if state0 is None:
		theta0 = rand.random()*2*pi - pi #between -pi and pi rad
		h0 = rand.random()*J*2 - 0.5 	 #between -1 and 1 rad/s
		state0 = [theta0, h0]
	
	if control is None:
		results = odeint(f, state0, time)
	else:
		results = odeint(f, state0, time, control)
		
	theta, h = results[:,0], results[:,1]
	
	#system trajectory
	plot.figure() #not plotting
	plot.plot(theta, h, color='green')
	
	thetamax, hmax = max(theta), max(h)
	theta, h = np.meshgrid(np.linspace(-thetamax, thetamax, 10), np.linspace(-hmax, hmax, 10))
	Dtheta, Dh = np.array(theta), np.array(h)
	for i in range(theta.shape[0]):
		for j in range(theta.shape[1]):
			Dtheta[i,j] = dtheta(float(theta[i,j]), float(h[i,j]))
			Dh[i,j] = dh(float(theta[i,j]), float(h[i,j]))

	plot.quiver(theta, h, Dtheta, Dh) #phase portrait (vector field)
	
