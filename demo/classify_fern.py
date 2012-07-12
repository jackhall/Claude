import fernpy as fp
import random as rand
import sys #for printing
import numpy as np
import matplotlib.pyplot as plot

def random_seed(n):
	rand.seed(n)

###### fitness evaluation #######
def fitness(individual):
	"""procedure to compute evolutionary fitness"""
	
	return 
	
def select(population_fitness):
	"""randomly selects an element based on normalized fitnesses"""
	choice = rand.random()
	for index, fitness in enumerate(population_fitness):
		if fitness > choice: 
			return index
		else:
			choice -= fitness
	return len(population_fitness)-1

###### genetic algorithm ######	
def evolve(gen=200, pop=20, mutation_rate=.1, crossover_rate=.7):
	"""runs fern genetic algorithm and returns final population"""
	s = fp.interval(0.0, 1.0);
	r = fp.region1().set_uniform(s);
	population = [fp.fern1(s, 1) for i in range(pop)];
	
	max_fitness = [0]*gen
	median_fitness = [0]*gen
	
	#generate distributions and classify them (np.random.normal)
	
	for generation in range(gen):
		#evaluate ferns
		pop_fitness = np.array([0.0]*pop)
		#state0 = [random_state() for i in range(10)] #simulations per evaluation
		for index, individual in enumerate(population):
			pop_fitness[index] = fitness(individual)
		
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
			new_population.append(fp.fern2( population[select(pop_fitness)] ))
			if rand.random() < crossover_rate:
				new_population[-1].crossover( population[select(pop_fitness)] ) 
			if rand.random() < mutation_rate:
				new_population[-1].mutate()
		
		population = new_population
	
	plot.figure()
	plot.plot(range(n), max_fitness, color='green') #plot fitness progression
	plot.plot(range(n), median_fitness, color='blue')
	plot.show()
	
	return population
	
