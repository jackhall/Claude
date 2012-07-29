import fernpy
import random as rand
import sys #for printing
import numpy as np
import matplotlib.pyplot as plot
import scipy as sp #for argmax
from math import pi
from math import log
import fernplot as fplt
import pp

def random_seed(n):
	rand.seed(n)

thrust = .5;
J = 100.0
h0 = 2.0 #range of random initial h is [-h0, h0]

def path(h):
	"""represents optimal return path and decision boundary"""
	return -0.5*h*abs(h)/(J*thrust)

def generate_data(n=1000):
	"""generate distributions and classify them"""
	h = np.random.uniform(-50.0, 50.0, n)
	theta = np.random.uniform(-pi, pi, n)
	#theta = path(h) + np.random.normal(0.0, 0.5, n)
	numbers = np.vstack((theta, h)).T
	classes = [0]*n
	for index, number in enumerate(numbers):
		manifold = path(number[1])
		if number[0] > manifold: 
			classes[index] = 1
		elif number[0] < manifold: 
			classes[index] = 2
		else: 
			if number[1] > 0: 
				classes[index] = 1
			elif number[1] < 0:
				classes[index] = 2
			else:
				classes[index] = 0
	return numbers, classes

###### fitness evaluation #######
def fitness(individual, numbers, classes):
	"""procedure to compute evolutionary fitness"""
	correct = 0.0
	p = fernpy.point2()
	for index, number in enumerate(numbers):
		p[0], p[1] = number[0], number[1]
		if individual.query(p) == classes[index]:
			correct += 1.0
	return correct
	
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

###### genetic algorithm ######	
mutation_rate = .4 #best yet: .4
crossover_rate = .2 #best yet: .1
node_type_chance = .7 #best yet: .7
mutation_type_chance_leaf = .15 #best yet: .15
mutation_type_chance_fork = .1 #best yet: .1
#fitness_ratio = 2 #ratio of max fitness to median fitness (or mean)
def evolve(gen=500, population=None, pop=50):
	"""runs fern genetic algorithm and returns final population"""
	if population is None:
		r = fernpy.region2()
		r[0], r[1] = fernpy.interval(-pi, pi), fernpy.interval(-50.0, 50.0)
		r.set_uniform( fernpy.interval(-pi, pi) )
		population = [fernpy.fern2(r, 3) for i in range(pop)]
		randomize=True
	else:
		pop = len(population)
		randomize=False
	
	for index, individual in enumerate(population):
		population[index].set_node_type_chance(node_type_chance)
		population[index].set_mutation_type_chance(mutation_type_chance_fork, 
							   mutation_type_chance_leaf)
	
	if randomize:
		for index, individual in enumerate(population):
			population[index].randomize(500)
	#else:
	#	for index, individual in enumerate(population):
	#		population[index].randomize(50)
	
	job_server = pp.Server(ppservers=())
	jobs = []
	subfuncs = ()
	packages = ("numpy", "fernpy")
	template = pp.Template(job_server, fitness, subfuncs, packages)
	
	max_fitness = [0]*gen
	median_fitness = [0]*gen
	min_fitness = [0]*gen
	
	for generation in range(gen):
		#generating new data for each generation helps encourage
		#a healthy genetic variation
		numbers, classes = generate_data()
	
		#evaluate ferns
		pop_fitness = np.array([0.0]*pop)
		for index, individual in enumerate(population):
			if len(jobs) < pop: #only happens in first generation
				jobs.append( template.submit(individual, numbers, classes) )
			else:
			 	jobs[index] = template.submit(individual, numbers, classes)
		
		for index, result in enumerate(jobs):
			pop_fitness[index] = result()
		
		#record max and median fitness
		max_fitness_index = sp.argmax(pop_fitness)
		max_fitness[generation] = pop_fitness[max_fitness_index]
		median_fitness[generation] = median(pop_fitness)
		min_fitness[generation] = min(pop_fitness)
		
		#control ratio of max to median fitness and normalize
		#fitness_mean = np.mean(pop_fitness);
		#if max_fitness[generation] != fitness_mean: #median_fitness[generation]:
		#	a = log(fitness_ratio) / (log(max_fitness[generation]) - log(fitness_mean))
		#	pop_fitness = (pop_fitness - fitness_mean)**a + fitness_mean
		pop_fitness /= sum(pop_fitness)
		
		sys.stdout.write("\rgen %i" % generation)
		sys.stdout.flush()
		
		if generation == gen-1: 
			sys.stdout.write("\n")
			break #skip breeding on last step
		
		#select parents and breed new population
		new_population = []
		new_population.append( population[max_fitness_index] ) #elitism
		for i in range(1, pop):
			new_population.append(fernpy.fern2( population[select(pop_fitness)] ))
			if rand.random() < crossover_rate:
				new_population[-1].crossover( population[select(pop_fitness)] ) 
			if rand.random() < mutation_rate:
				new_population[-1].mutate()
		
		population = new_population
	
	h = 1.0*np.array(range(-50, 50))
	theta = path(h)
	plot.figure()
	plot.plot(theta, h)
	a = plot.gca()
	a.set_xlim([-pi, pi])
	plot.show()
	
	fplt.plot(population[max_fitness_index], "static_satellite.png")
	
	plot.figure()
	plot.plot(range(gen), max_fitness, '+', color='green') #plot fitness progression
	plot.plot(range(gen), median_fitness, 'x', color='blue')
	plot.plot(range(gen), min_fitness, '.', color = 'red')
	plot.show()
	
	return population
	
