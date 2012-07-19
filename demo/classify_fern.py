import fernpy as fp
import random as rand
import sys #for printing
import numpy as np
import matplotlib.pyplot as plot
import scipy as sp #for argmax
from math import log

def random_seed(n):
	rand.seed(n)

def generate_data(n=1000):
	"""generate distributions and classify them"""
	numbers = np.random.normal(0.0, 1.0, n)
	classes = [0]*n
	for index, number in enumerate(numbers):
		if number > 1.0:
			classes[index] = 0
		else:
			classes[index] = 1
	return numbers, classes

###### fitness evaluation #######
def fitness(individual, numbers, classes):
	"""procedure to compute evolutionary fitness"""
	correct = 0.0
	p = fp.point1()
	for index, number in enumerate(numbers):
		p[0] = number
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

###### genetic algorithm ######	 working! don't touch now
mutation_rate = .4 #best yet: .4
crossover_rate = .05 #best yet: .05
node_type_chance = .85 #best yet: .85
mutation_type_chance_leaf = .15 #best yet: .15
mutation_type_chance_fork = .1 #best yet: .1
#fitness_ratio = 2 #ratio of max fitness to median fitness (or mean)
def evolve(gen=600, population=None, pop=50):
	"""runs fern genetic algorithm and returns final population"""
	if population is None:
		r = fp.region1()
		r.set_uniform( fp.interval(-3.0, 3.0) )
		population = [fp.fern1(r, 2) for i in range(pop)]
	else:
		pop = len(population)
	
	for index, individual in enumerate(population):
		population[index].set_node_type_chance(node_type_chance)
		population[index].set_mutation_type_chance(mutation_type_chance_fork, 
							   mutation_type_chance_leaf)
	
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
			pop_fitness[index] = fitness(individual, numbers, classes)
		
		#record and then normalize fitness
		max_fitness_index = sp.argmax(pop_fitness)
		max_fitness[generation] = pop_fitness[max_fitness_index]
		median_fitness[generation] = median(pop_fitness)
		min_fitness[generation] = min(pop_fitness)
		
		#control ratio of max to median fitness and normalize
		#fitness_mean = np.mean(pop_fitness);
		#if max_fitness[generation] != fitness_mean: 
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
			new_population.append(fp.fern1( population[select(pop_fitness)] ))
			if rand.random() < crossover_rate:
				new_population[-1].crossover( population[select(pop_fitness)] ) 
			if rand.random() < mutation_rate:
				new_population[-1].mutate()
		
		population = new_population
	
	plot.figure()
	plot.plot(range(gen), max_fitness, '+', color='green') #plot fitness progression
	plot.plot(range(gen), median_fitness, 'x', color='blue')
	plot.plot(range(gen), min_fitness, '.', color = 'red')
	plot.show()
	
	return population
	
