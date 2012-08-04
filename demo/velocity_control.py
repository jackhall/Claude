import fernpy
import math #for pi
import numpy
import matplotlib.pyplot as plot
import scipy #for argmax
import scipy.integrate
#from scipy.integrate import odeint
import random
import sys #for printing
import time #for time limit on simulations and pausing
import fernplot as fplt
import pp
import pickle

#rand.seed(3) #call with no arguments for true randomness

thrust = .5

def random_state():
	"""generate random initial condition"""
	v = 0.0
	while abs(v) < 3.0:
		v = numpy.random.uniform(-15.0, 15.0) #between -pi and pi rad
	return [v]

class OptimalController:
	def query(self, point): 
		"""mimics fern.query to give optimal control of the satellite"""
		v = point[0]
		if v > 0: 
			mode = 1
		elif v < 0: 
			mode = 2
		else: 
			mode = 0
		return mode

def f(t, state, force):
	return [force]

def simulate(t_final, dt, control=None, state0=None, thrust=.5):
	"""simulates a satellite (with event detection)"""
	if control is None:
		control = OptimalController()
		
	if state0 is None:
		state0 = random_state()
	
	state_tolerance = .01
	
	y_out, t_out = [], []
	solver = scipy.integrate.ode(f).set_integrator('dopri5') #vode cannot run in parallel
	solver.set_initial_value(state0, 0.0) #thinks state0 has only one number in it?
	p = fernpy.point1()
	max_time = time.clock() + 0.1 
	while solver.t < t_final and solver.successful():
		
		#select a control mode by setting torque
		p[0] = solver.y[0]
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
			p[0] = solver.y[0]
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
			p[0] = solver.y[0]
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
	
	return numpy.array(y_out), t_out

###### fitness evaluation #######
momentum_weight = .2;
def fitness(individual, state0):
	"""procedure to compute evolutionary fitness from a simulation"""
	#simulate, maybe start out with static i.c. and anneal to random ones
	total_error = 0.0;
	for i in state0: 
		results, time = simulate(50, 10, individual, i)
		total_error += scipy.integrate.trapz(abs(results[:,0]), time)
		#total_error += abs(results[-1,0])
	return 3000.0/total_error 
	
def select(population_fitness):
	"""randomly selects an element based on normalized fitnesses"""
	choice = random.random()
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

mutation_rate = .4 #best yet: .4
crossover_rate = .2 #best yet: .05
node_type_chance = .85 #best yet: .8
mutation_type_chance_leaf = .15 #best yet: .15
mutation_type_chance_fork = .1 #best yet: .1
#fitness_ratio = 3 #ratio of max fitness to median fitness (or mean)
def evolve(gen=1000, population=None, pop=50):
	"""runs fern genetic algorithm and returns final population"""
	if population is None:
		r = fernpy.region1()
		r[0] = fernpy.interval(-20.0, 20.0)
		population = [fernpy.fern1(r, 3) for i in range(pop)]
		randomize = True
	else:
		pop = len(population)
		randomize = False
	
	for index, individual in enumerate(population):
		population[index].set_node_type_chance(node_type_chance)
		population[index].set_mutation_type_chance(mutation_type_chance_fork, 
							   mutation_type_chance_leaf)
		if randomize:
			population[index].randomize(15)
	
	job_server = pp.Server(ppservers=())
	jobs = []
	subfuncs = (simulate, f)
	packages = ("time", "scipy", "scipy.integrate", "numpy", "math", "fernpy")
	template = pp.Template(job_server, fitness, subfuncs, packages)
	
	max_fitness = [0]*gen
	median_fitness = [0]*gen
	min_fitness = [0]*gen
	#state0 = [random_state() for i in range(5)] #5 simulations per evaluation
	
	for generation in range(gen):
		state0 = [-15, -7, 7, 15] #4 simulations per evaluation
		optimal_fitness = fitness(OptimalController(), state0)
	
		#evaluate ferns
		pop_fitness = numpy.array([0.0]*pop)
		#state0 = [random_state() for i in range(10)] #simulations per evaluation
		for index, individual in enumerate(population):
			if len(jobs) < pop: #only happens in first generation
				jobs.append( template.submit(individual, state0) )
			else:
			 	jobs[index] = template.submit(individual, state0)
		
		for index, result in enumerate(jobs):
			pop_fitness[index] = result() / optimal_fitness 
		
		#record and then normalize fitness
		max_fitness_index = scipy.argmax(pop_fitness)
		max_fitness[generation] = pop_fitness[max_fitness_index]
		median_fitness[generation] = median(pop_fitness)
		min_fitness[generation] = min(pop_fitness)
		
		#control ratio of max to median fitness and normalize
		#fitness_mean = np.mean(pop_fitness);
		#if max_fitness[generation] != median_fitness[generation]: #fitness_mean: 
		#	a = log(fitness_ratio) / (log(max_fitness[generation]) - log(median_fitness[generation]))
		#	pop_fitness = (pop_fitness - median_fitness[generation])**a + median_fitness[generation]
		
		sys.stdout.write("\rgen %i" % generation)
		sys.stdout.flush()
		
		if generation == gen-1: 
			sys.stdout.write("\n")
			break #skip breeding on last step
			
		pop_fitness /= sum(pop_fitness)
		
		#select parents and breed new population
		new_population = []
		new_population.append( population[max_fitness_index] ) #elitism
		for i in range(1, pop):
			new_population.append(fernpy.fern1( population[select(pop_fitness)] ))
			if random.random() < crossover_rate:
				new_population[-1].crossover( population[select(pop_fitness)] ) 
			if random.random() < mutation_rate:
				new_population[-1].mutate()
		
		population = new_population
			
	##### plots ########
	fplt.plot(population[max_fitness_index], "velocity_control.png")
	#print population[max_fitness_index]
	
	datafile = open("velocity_control.dat", "wb")
	pickler = pickle.Pickler(datafile)
	pickler.dump(population)
	pickler.dump(pop_fitness)
	datafile.close()
	
	plot_sim( population[ max_fitness_index ] ) #plot best solution
	plot.figure()
	plot.plot(range(gen), max_fitness, '+', color='green') #plot fitness progression
	plot.plot(range(gen), median_fitness, 'x', color='blue')
	plot.plot(range(gen), min_fitness, '.', color = 'red')
	plot.show()
	
	return population, pop_fitness
	
def plot_sim(control=None, state0=None):
	results, time = simulate(100, 10, control, state0)
	v = results[:,0]
	plot.figure(1)
	plot.plot(time, v)
	plot.xlabel("Time (s)")
	plot.ylabel("Velocity (m/s)")
	plot.show()

def load(filename="velocity_control.dat"):
	datafile = open(filename, "rb")
	population = pickle.load(datafile)
	pop_fitness = pickle.load(datafile)
	return population, pop_fitness
	
