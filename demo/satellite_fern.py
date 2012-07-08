import fernpy as fp
from math import * #for pi
import numpy as np 
import matplotlib.pyplot as plot
import scipy as sp #for argmax
import scipy.integrate as spint
#from scipy.integrate import odeint
import random as rand
import sys #for printing
import time #for time limit on simulations

rand.seed(3) #call with no arguments for true randomness

def random_seed(n):
	rand.seed(n)

##### ode ######
thrust = 1;
J = 100

def random_state():
	theta = rand.random()*2*pi - pi #between -pi and pi rad
	h = (rand.random()*2 - 1)*1	
	return [theta, h]
	
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
		torque = -thrust
	elif mode is 1:
		torque = 0
	elif mode is 2:
		torque = thrust		
	return torque

def dtheta(theta, h):
	"""computes the angular velocity"""
	return h/J

#def f(state, t, control=None):
#	"""ode representing a satellite - to be solved numerically"""
#	theta, h = state
#	Dtheta = dtheta(theta, h)
#	Dh = dh(theta, h, control)
#	return Dtheta, Dh

#event detection by stepping through scipy.ode (object-oriented wrapper of lsoda)
def fblank(t, state, torque):
	"""ode representing satellite - needs 'torque' to be set"""
	theta, h = state
	Dtheta = dtheta(theta, h)
	Dh = torque
	return [Dtheta, Dh] #f2py weirdness requires a list instead of a tuple

def simulate(t_final, dt, control=None, state0=None):
	"""simulates a satellite (with event detection)"""
	if control is None:
		control = OptimalController()
		
	if state0 is None:
		state0 = random_state()
	
	state_tolerance = .01
	
	y_out, t_out = [], []
	solver = spint.ode(fblank).set_integrator('vode', method='adams', with_jacobian=False) 
	solver.set_initial_value(state0, 0.0) #thinks state0 has only one number in it?
	p = fp.point()
	max_time = time.clock() + 0.5
	while solver.t < t_final and solver.successful():
		
		#select a control mode by setting torque
		p[0], p[1] = solver.y
		mode = control.query(p) 
		if mode is 0:
			solver.set_f_params(-thrust)
		elif mode is 1:
			solver.set_f_params(0.0)
		elif mode is 2:
			solver.set_f_params(thrust)

		if sum(abs(solver.y)) < state_tolerance: #dead zone around (0,0)
			solver.set_f_params(0.0)

		#run forward as far as possible
		while solver.successful() and solver.t < t_final:
			y_out.append(solver.y)
			t_out.append(solver.t)
			solver.integrate(solver.t + dt) 
			p[0], p[1] = solver.y
			if sum(abs(solver.y)) < state_tolerance:
				solver.set_f_params(0.0)
			elif control.query(p) != mode:
				#print p, ", stopping at t = ", solver.t
				break
		
		#check if simulation is done
		if solver.t >= t_final or time.clock() > max_time or not solver.successful():
			y_out.append(solver.y)
			t_out.append(solver.t)
			break
		
		#go back one step, cut step size in half until switch time is known
		solver.set_initial_value(y_out[-1], t_out[-1])
		step = dt / 2.0
		while True:
			y_current, t_current = solver.y, solver.t
			solver.integrate(solver.t + step)
			p[0], p[1] = solver.y
			if control.query(p) != mode:
				if step < 0.001:
					break
				solver.set_initial_value(y_current, t_current)
			step /= 2.0
	
	#if solver hit the time limit
	if t_out[-1] < t_final: 
		y_out.append(solver.y*1000)
		t_out.append(t_final)
	
	return np.array(y_out), t_out
	

###### fitness evaluation #######
def fitness(individual, state0):
	"""procedure to compute evolutionary fitness from a simulation"""
	#simulate
	total_error = 0.0;
	for i in state0: 
		results, time = simulate(100, 10, individual, i)
		total_error += spint.trapz(abs(results[:,0]), time)
	return 3000.0/total_error 
	
def select(population_fitness):
	"""randomly selects an element based on normalized fitnesses"""
	choice = rand.random()
	for index, fitness in enumerate(population_fitness):
		if fitness > choice: 
			return index
		else:
			choice -= fitness
	return len(population_fitness)-1

def median(sequence):
	sequence.sort()
	if len(sequence)%2 == 0:
		mid_right = len(sequence)/2
		return (sequence[mid_right] + sequence[mid_right-1]) / 2.0
	else:
		return sequence[ len(sequence)/2 ]    

#################################
##### genetic algorithm #########
#################################
def evolve(n=200, pop=20, mutation_rate=.1, crossover_rate=.7):
	"""runs fern genetic algorithm and returns final population"""
	#initialize ferns
	r = fp.region()
	r[0], r[1] = fp.interval(-pi, pi), fp.interval(-2.0*J, 2.0*J)

	population = [fp.fern(r, 3) for i in range(pop)] #list of tuple(fern, fitness)
	#time = np.linspace(0, .1, 100) not needed with simulate()
	
	max_fitness = [0]*n
	median_fitness = [0]*n
	
	for generation in range(n):
		#evaluate ferns
		pop_fitness = np.array([0.0]*pop)
		state0 = [random_state() for i in range(3)] #3 simulations per evaluation
		for index, individual in enumerate(population):
			pop_fitness[index] = fitness(individual, state0)
		
		#record and then normalize fitness
		max_fitness_index = sp.argmax(pop_fitness)
		max_fitness[generation] = pop_fitness[max_fitness_index]
		median_fitness[generation] = median(pop_fitness)
		pop_fitness /= sum(pop_fitness)
		
		sys.stdout.write("\rgen %i" % generation)
		sys.stdout.flush()
		
		if generation == n-1: 
			sys.stdout.write("\n")
			break #skip breeding on last step
		
		#select parents and breed new population
		new_population = []
		for i in range(pop):
			new_population.append(fp.fern( population[select(pop_fitness)] ))
			if rand.random() < crossover_rate:
				new_population[-1].crossover( population[select(pop_fitness)] ) 
			if rand.random() < mutation_rate:
				new_population[-1].mutate()
		
		population = new_population
			
	##### plots ########
	plot_phase( population[ max_fitness_index ] ) #plot best solution
	plot.figure()
	plot.plot(range(n), max_fitness, color='green') #plot fitness progression
	plot.plot(range(n), median_fitness, color='blue')
	plot.show()
	
	return population


########## phase plotting ########### 
def plot_phase(control=None, state0 = None):
	"""generates a phase portrait and phase trajectory from initial conditions"""
	#time = np.linspace(0, 20, 100) not needed for simulate()
	
	if state0 is None:
		state0 = random_state()
	#print "Initial: ", state0
	
	if control is None:
		control = OptimalController()

	results, time = simulate(50, 5, control, state0)
	theta, h = results[:,0], results[:,1]
	#statew = [theta[-1], h[-1]]
	#print "Final: ", statew
	
	#system trajectory
	plot.figure() 
	plot.plot(theta, h, color='green')
	
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
	
