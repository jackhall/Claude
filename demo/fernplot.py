import fernpy as fp
import Tkinter as tk

class leaf_iter:
	node = fp.node_handle1()
	region = fp.region1()
	dimensions = 1
	branch_stack = []
	
	def __init__(self, fern):
		if type(fern) is fp.fern1:
			self.dimensions = 1
		elif type(fern) is fp.fern2:
			self.dimensions = 2
		else:
			print "error: only takes node_handles from 1D or 2D ferns"
			return
		
		self.node = fern.begin()
		self.region = fern.get_region()
		self.branch_stack = []
		self.left_most()
	
	def left_most(self):
		while not self.node.is_leaf():
			self.region.split(self.node.get_fork_division(), 
					  False)
			self.branch_stack.append(False)
			self.node.left()
	
	def next(self):
		restart = False
		self.node.up()
		self.region.expand(self.node.get_fork_division(), 
				   self.branch_stack[-1]) 
		while self.branch_stack[-1]:
			self.branch_stack.pop()
			if len(self.branch_stack) is 0:
				restart = True
				break
			self.node.up()
			self.region.expand(self.node.get_fork_division(),
					   self.branch_stack[-1])
		
		if not restart:
			self.branch_stack[-1] = True
			self.region.split(self.node.get_fork_division(), True)
			self.node.right()
		
		self.left_most()
		

def plot(fern):
	it = leaf_iter(fern)
	first = leaf_iter(fern)
	master = tk.Tk()
	colors = ["blue", "green", "red"]
	size = 800

	if type(fern) is fp.fern1:
		canvas = tk.Canvas(master, width=size, height=25)
		canvas.pack()
		scale = float(size) / fern.get_bounds(1).span()
		xmin = fern.get_bounds(1).lower #something wrong here
		x = (0, size)
		
		while True:
			xint = it.region[0]
			x = round((xint.lower-xmin) * scale), \
			    round((xint.upper-xmin) * scale)
			canvas.create_rectangle(x[0], 0, x[1], 50, 
						fill=colors[it.node.get_leaf_bin()])
			it.next()
			if it.node == first.node: 
				break
		
	elif type(fern) is fp.fern2:
		canvas = tk.Canvas(master, width=size, height=size)
		canvas.pack()
		scale = float(size) / fern.get_bounds(1).span(), \
			float(size) / fern.get_bounds(2).span()
		xmin, ymin = fern.get_bounds(1).lower, fern.get_bounds(2).lower
		x, y = (0, size), (0, size)
	
		while True:
			xint, yint = it.region[0], it.region[1]
			x = round((xint.lower-xmin) * scale[0]), \
			    round((xint.upper-xmin) * scale[0])
			y = round((yint.lower-xmin) * scale[1]), \
			    round((yint.upper-xmin) * scale[1])
			canvas.create_rectangle(x[0], y[0], x[1], y[1],
						fill=colors[it.node.get_leaf_bin()])
			it.next()
			if it.node == first.node:
				break
	else:
		print "error: only takes 1D or 2D fern inputs"
		return
	
	tk.mainloop()

def fernplot_test(dim=1):
	if dim is 1:
		r = fp.region1()
		r[0] = fp.interval(1.0, 3.0)
		f = fp.fern1(r, 2)
		n = f.begin()
		n.right()
		n.set_leaf_bin(1)
		
		plot(f)
		
	if dim is 2:
		r = fp.region2()
		r.set_uniform(fp.interval(1.0, 3.0))
		f = fp.fern2(r, 3)
		for i in range(50):
			f.mutate()
		print f
		plot(f)

def tkinter_test():
	master = tk.Tk()
	w = tk.Canvas(master, width=200, height=100)
	w.pack()
	w.create_line(0, 0, 200, 100)
	w.create_line(0, 100, 200, 0, fill="red", dash=(4, 4))
	w.create_rectangle(50, 25, 150, 75, fill="blue")

	tk.mainloop()

