import fernpy
import pp
import numpy

def fitness(individual, numbers, classes):
	"""procedure to compute evolutionary fitness"""
	correct = 0.0
	p = fernpy.point1()
	for index, number in enumerate(numbers):
		p[0] = number
		if individual.query(p) == classes[index]:
			correct += 1.0
	return correct

def run():
	#prepare ferns
	r = fernpy.region1()
	r.set_uniform( fernpy.interval(-3.0, 3.0) )
	population = [fernpy.fern1(r, 2) for i in range(5)]
	for index, individual in enumerate(population):
		population[index].randomize(15)
	
	#prepare data
	n = 10000
	numbers = numpy.random.normal(0.0, 1.0, n)
	classes = [0]*n
	for index, number in enumerate(numbers):
		if number > 1.0:
			classes[index] = 0
		else:
			classes[index] = 1

	job_server = pp.Server(ppservers=())
	jobs = []
	subfuncs = ()
	packages = ("fernpy", "numpy")
	
	for i in range(5):
		pop_fitness = numpy.array([0.0]*5)
		if len(jobs) < 5:
			for index, individual in enumerate(population):
				jobs.append(job_server.submit(fitness, 
							      (individual, numbers, classes,), 
							      subfuncs, packages))
		else:
			for index, individual in enumerate(population):
			 	jobs[index] = job_server.submit(fitness, 
			 					(individual, numbers, classes,), 
			 					subfuncs, packages)
		
		for index, result in enumerate(jobs):
			pop_fitness[index] = result()
	
		for index, individual in enumerate(population):
			population[index].randomize(10)
		
		print "Best in gen ", i, " is ", max(pop_fitness)
	
