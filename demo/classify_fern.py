import fernpy as fp

s = fp.interval(0.0, 1.0);
r = fp.region().set_uniform(s);
population = [fp.fern(s, 2) for i in range(20)];

def fitness(individual):
	pass
