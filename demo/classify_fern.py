import fernpy as fp
import random as rand
import sys #for printing
import numpy as np
import matplotlib.pyplot as plot
import scipy as sp #for argmax

def random_seed(n):
	rand.seed(n)

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

###### genetic algorithm ######	
def evolve(gen=200, pop=20, mutation_rate=.1, crossover_rate=.7):
	"""runs fern genetic algorithm and returns final population"""
	s = fp.interval(-3.0, 3.0)
	r = fp.region1()
	r.set_uniform(s)
	population = [fp.fern1(r, 2) for i in range(pop)]
	
	max_fitness = [0]*gen
	median_fitness = [0]*gen
	
	#generate distributions and classify them (np.random.normal)
	n = 1000
	numbers = np.random.normal(0.0, 1.0, n)
	classes = [0]*n
	for index, number in enumerate(numbers):
		if number > 1.0:
			classes[index] = 0
		else:
			classes[index] = 1
	
	for generation in range(gen):
		#evaluate ferns
		pop_fitness = np.array([0.0]*pop)
		for index, individual in enumerate(population):
			pop_fitness[index] = fitness(individual, numbers, classes)
		
		#record and then normalize fitness
		max_fitness_index = sp.argmax(pop_fitness)
		max_fitness[generation] = pop_fitness[max_fitness_index]
		median_fitness[generation] = median(pop_fitness)
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
	plot.plot(range(gen), max_fitness, color='green') #plot fitness progression
	plot.plot(range(gen), median_fitness, color='blue')
	plot.show()
	
	return population
	
