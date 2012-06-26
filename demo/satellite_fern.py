import fernpy as fp
from math import * #for pi
import numpy as np 
import matplotlib.pyplot as plot
from scipy.integrate import odeint
import random as rand

rand.seed(3) #call with no arguments for true randomness

def random_seed(n):
	rand.seed(n)

##### ode ######
thrust = 1;
J = 100
class OptimalController:
	def query(self, point): 
		"""mimics fern.query to give optimal control of the satellite"""
		theta, h = point[0], point[1]
		path = -0.5*thrust*h*abs(h)/J
		if theta > path: 
			mode = 0
		elif theta < path: 
			mode = 2
		else: 
			if h > 0:
				mode = 0
			elif h < 0:
				mode = 2
			else:
				mode = 1
		return mode
		
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


########## phase plotting ###########
def plot_phase(state0 = None, control=None):
	"""generates a phase portrait and phase trajectory from initial conditions"""
	time = np.linspace(0, 100, 100)
	
	if state0 is None:
		theta0 = rand.random()*2*pi - pi #between -pi and pi rad
		h0 = rand.random()*2 - 0.5 	 #between -1 and 1 rad/s
		state0 = [theta0, h0]
	
	if control is None:
		results = odeint(f, state0, time)
	else:
		results = odeint(f, state0, time, control)
		
	theta, h = results[:,0], results[:,1]
	
	#system trajectory
	plot.figure() #not plotting
	plot.plot(theta, h, color='green')
	plot.plot(-0.5*thrust*h*abs(h)/J, h, color='blue')
	
	#phase portrait (vector field)
	thetamax, hmax = max(abs(theta)), max(abs(h))
	theta, h = np.meshgrid(np.linspace(-thetamax, thetamax, 10), np.linspace(-hmax, hmax, 10))
	Dtheta, Dh = np.array(theta), np.array(h)
	for i in range(theta.shape[0]):
		for j in range(theta.shape[1]):
			Dtheta[i,j] = dtheta(float(theta[i,j]), float(h[i,j]))
			Dh[i,j] = dh(float(theta[i,j]), float(h[i,j]))

	plot.quiver(theta, h, Dtheta, Dh) 
	
	#optimal path mode
	h = np.linspace(-hmax, hmax, 20)
	plot.plot(-0.5*thrust*h*abs(h)/J, h, color='blue')
	
	#plot.savefig("satellite-phase.png", dpi=200)
	plot.show()
	
