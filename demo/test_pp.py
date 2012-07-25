import fernpy as fp
import pp
import numpy as np

#prepare data
n = 10000
numbers = np.random.normal(0.0, 1.0, n)
classes = [0]*n
for index, number in enumerate(numbers):
	if number > 1.0:
		classes[index] = 0
	else:
		classes[index] = 1

#prepare ferns
r = fp.region1()
r.set_uniform( fp.interval(-3.0, 3.0) )
population = [fp.fern1(r, 2) for i in range(20)]

for index, individual in enumerate(population):
	population[index].randomize(15)

def fitness(individual):
	"""procedure to compute evolutionary fitness"""
	correct = 0.0
	p = fp.point1()
	for index, number in enumerate(numbers):
		p[0] = number
		if individual.query(p) == classes[index]:
			correct += 1.0
	return correct

def run():
	job_server = pp.Server(ppservers=())
	jobs = []
	subfuncs = ()
	packages = ("fp", "np")
	
	for i in range(10):
		pop_fitness = np.array([0.0]*20)
		if len(jobs) < 20:
			for index, individual in enumerate(population):
				jobs.append(job_server.submit(fitness, (individual,), 
							      subfuncs, packages))
		else:
			for index, individual in enumerate(population):
			 	jobs[index] = job_server.submit(fitness, (individual,), 
			 					subfuncs, packages)
		
		for index, result in enumerate(jobs):
			pop_fitness[index] = result()
	
		for index, individual in enumerate(population):
			population[index].randomize(10)
		
		print "Best in gen ", i, " is ", max(pop_fitness)
	
