import fernpy as fp
from math import * #for pi
import numpy as np 
import matplotlib.pyplot as plot
import scipy as sp #for argmax
import scipy.integrate as spint
#from scipy.integrate import odeint
import random as rand
import sys #for printing
import time #for time limit on simulations and pausing
import fernplot as fplt

#rand.seed(3) #call with no arguments for true randomness

def random_seed(n):
	rand.seed(n)

##### ode ######
thrust = .5;
J = 100.0
h0 = 2.0 #range of random initial h is [-h0, h0]

def path(h):
	"""represents optimal return path and decision boundary"""
	return -0.5*h*abs(h)/(J*thrust)

def roll(y):
	"""takes care of angle roll-over"""
	theta = y[0]
	if abs(theta) > pi:
		theta %= 2*pi
		if theta > pi:
			theta -= 2*pi
	return [theta, y[1]]

def random_state():
	"""generate random initial condition"""
	theta = rand.random()*2*pi - pi #between -pi and pi rad
	h = (rand.random()*2 - 1)*h0	
	return [theta, h]

class OptimalController:
	def query(self, point): 
		"""mimics fern.query to give optimal control of the satellite"""
		theta, h = point[0], point[1]
		manifold = path(h)
		if theta > manifold: 
			mode = 1
		elif theta < manifold: 
			mode = 2
		else: 
			if h > 0: 
				mode = 1
			elif h < 0:
				mode = 2
			else:
				mode = 0
		return mode
		
def dh(theta, h, control=None):
	"""computes the derivative of angular momentum (torque thrust)"""
	if control is None:
		control = OptimalController()
	
	p = fp.point2()
	p[0], p[1] = roll([theta, h]) 
	mode = control.query(p) 
	if mode is 0:
		torque = 0
	elif mode is 1:
		torque = -thrust
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
	p = fp.point2()
	max_time = time.clock() + 0.1
	while solver.t < t_final and solver.successful():
		
		#select a control mode by setting torque
		p[0], p[1] = solver.y
		mode = control.query(p) 
		if mode is 0:
			solver.set_f_params(0.0)
		elif mode is 1:
			solver.set_f_params(-thrust)
		elif mode is 2:
			solver.set_f_params(thrust)

		if sum(abs(solver.y)) < state_tolerance: #dead zone around (0,0)
			solver.set_f_params(0.0)

		#run forward as far as possible
		while solver.successful() and solver.t < t_final:
			y_out.append(solver.y)
			t_out.append(solver.t)
			solver.integrate(solver.t + dt)
			#if abs(solver.y[0]) > pi:
			#	solver.set_initial_value(roll(solver.y), solver.t)
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
		bisect_step = dt / 2.0
		while solver.successful(): #program hangs here somewhere
			y_current, t_current = solver.y, solver.t
			solver.integrate(solver.t + bisect_step)
			#if abs(solver.y[0]) > pi: #two if statements interacting to stop break
			#	solver.set_initial_value(roll(solver.y), solver.t)
			p[0], p[1] = solver.y
			#sys.stdout.write("\rstep size: %f" % bisect_step)
			#sys.stdout.flush()
			if control.query(p) != mode:
				if bisect_step < 0.001:
					break
				solver.set_initial_value(y_current, t_current)
			bisect_step /= 2.0
			if time.clock() > max_time:
				break
				
		if time.clock() > max_time: #solver hit time limit
				break
	
	#if solver hit the time limit
	if t_out[-1] < t_final: 
		y_out.append(solver.y*1000)
		t_out.append(t_final)
	
	return np.array(y_out), t_out
	

###### fitness evaluation #######
momentum_weight = .2;
def fitness(individual, state0):
	"""procedure to compute evolutionary fitness from a simulation"""
	#simulate, maybe start out with static i.c. and anneal to random ones
	total_error = 0.0;
	for i in state0: 
		results, time = simulate(100, 10, individual, i)
		total_error += spint.trapz(abs(results[:,0]), time)
		total_error += spint.trapz(abs(results[:,1]), time)
		#total_error += abs(results[-1,0])
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
#tweak options:
#	initial fern configuration
#	initial momentum for simulations
#	allow any merge (destruction of subtrees)
#	choose another fitness function
#	control fitness range with power law
#	mutation probabilities (leaf v fork, structure v value, dimension v bit)
#	mutation and crossover rates
#	deterministic v stochastic evaluation
#do I need to write a plotter for ferns?
mutation_rate = .4 #best yet: .4
crossover_rate = .05 #best yet: .05
node_type_chance = .85 #best yet: .8
mutation_type_chance_leaf = .25 #best yet: .15
mutation_type_chance_fork = .1 #best yet: .1
#fitness_ratio = 3 #ratio of max fitness to median fitness (or mean)
def evolve(gen=200, population=None, pop=50):
	"""runs fern genetic algorithm and returns final population"""
	if population is None:
		r = fp.region2()
		r[0], r[1] = fp.interval(-4*pi, 4*pi), fp.interval(-50.0, 50.0)
		population = [fp.fern2(r, 3) for i in range(pop)]
		randomize = True
	else:
		pop = len(population)
		randomize = False
	
	for index, individual in enumerate(population):
		population[index].set_node_type_chance(node_type_chance)
		population[index].set_mutation_type_chance(mutation_type_chance_fork, 
							   mutation_type_chance_leaf)
	
	#if randomize:
	#	for index, individual in enumerate(population):
	#		population[index].randomize(50)
	
	max_fitness = [0]*gen
	median_fitness = [0]*gen
	
	for generation in range(gen):
		state0 = [random_state() for i in range(5)] #5 simulations per evaluation
		optimal_fitness = fitness(OptimalController(), state0)
	
		#evaluate ferns
		pop_fitness = np.array([0.0]*pop)
		#state0 = [random_state() for i in range(10)] #simulations per evaluation
		for index, individual in enumerate(population):
			pop_fitness[index] = fitness(individual, state0) / optimal_fitness
		
		#record and then normalize fitness
		max_fitness_index = sp.argmax(pop_fitness)
		max_fitness[generation] = pop_fitness[max_fitness_index]
		median_fitness[generation] = median(pop_fitness)
		
		#control ratio of max to median fitness and normalize
		#fitness_mean = np.mean(pop_fitness);
		#if max_fitness[generation] != median_fitness[generation]: #fitness_mean: 
		#	a = log(fitness_ratio) / (log(max_fitness[generation]) - log(median_fitness[generation]))
		#	pop_fitness = (pop_fitness - median_fitness[generation])**a + median_fitness[generation]
		pop_fitness /= sum(pop_fitness)
		
		sys.stdout.write("\rgen %i" % generation)
		sys.stdout.flush()
		
		if generation == gen-1: 
			sys.stdout.write("\n")
			break #skip breeding on last step
		
		#select parents and breed new population
		new_population = []
		for i in range(pop):
			new_population.append(fp.fern2( population[select(pop_fitness)] ))
			if rand.random() < crossover_rate:
				new_population[-1].crossover( population[select(pop_fitness)] ) 
			if rand.random() < mutation_rate:
				new_population[-1].mutate()
		
		population = new_population
			
	##### plots ########
	fplt.plot( population[max_fitness_index] )
	#print population[max_fitness_index]
	
	plot_phase( population[ max_fitness_index ] ) #plot best solution
	plot.figure()
	plot.plot(range(gen), max_fitness, '+', color='green') #plot fitness progression
	plot.plot(range(gen), median_fitness, 'x', color='blue')
	plot.show()
	
	return population


########## phase plotting ########### 
def plot_phase(control=None, state0 = None):
	"""generates a phase portrait and phase trajectory from initial conditions"""
	#time = np.linspace(0, 20, 100) not needed for simulate()
	
	results, time = simulate(100, 10, control, state0)
	theta, h = results[:,0], results[:,1]
	#statew = [theta[-1], h[-1]]
	#print "Final: ", statew
	
	#system trajectory
	plot.figure(1) 
	plot.plot(theta, h, color='green') #use polar(theta, h)?
	plot.figure(2)
	plot.polar(theta, h, color='green')
		
	#phase portrait (vector field)
	thetamax, hmax = max(abs(theta)), max(abs(h))
	theta, h = np.meshgrid(np.linspace(-thetamax, thetamax, 10), np.linspace(-hmax, hmax, 10))
	Dtheta, Dh = np.array(theta), np.array(h)
	for i in range(theta.shape[0]):
		for j in range(theta.shape[1]):
			Dtheta[i,j] = dtheta(float(theta[i,j]), float(h[i,j]))
			Dh[i,j] = dh(float(theta[i,j]), float(h[i,j]))
	
	plot.figure(1)
	plot.quiver(theta, h, Dtheta, Dh) #no polar equivalent...
	
	#optimal path mode
	h = np.linspace(-hmax, hmax, 100)
	plot.plot(path(h), h, color='blue') #use polar(theta, h)?
	
	#optimal mode in polar
	plot.figure(2)
	plot.polar(path(h), h, color='blue')
	plot.show()
	#plot.savefig("satellite-phase.png", dpi=200)
	
	
