import fernpy as fp
import Tkinter as tk
import Image, ImageDraw

white = (255, 255, 255)
black = (0, 0, 0)
blue = (0, 0, 255)
red = (255, 0, 0)
green = (0,128,0)

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
		

def tk_plot(fern):
	it = leaf_iter(fern)
	first = leaf_iter(fern)
	master = tk.Tk()
	colors = ["blue", "red", "green"]
	size = 800

	if type(fern) is fp.fern1:
		canvas = tk.Canvas(master, width=size, height=25)
		canvas.pack()
		scale = float(size) / fern.get_bounds(1).span()
		xmin = fern.get_bounds(1).lower 
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
			y = round((yint.lower-ymin) * scale[1]), \
			    round((yint.upper-ymin) * scale[1])
			canvas.create_rectangle(x[0], y[0], x[1], y[1],
						fill=colors[it.node.get_leaf_bin()])
			#print colors[it.node.get_leaf_bin()]
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
		r.set_uniform(fp.interval(-300.0, 300.0))
		f = fp.fern2(r, 3)
		f.randomize(50)
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
	
	w.postscript(file="tkinter_test.ps", colormode='color')
	
def pil_test():
	image1 = Image.new("RGB", (200, 100), white)
	draw = ImageDraw.Draw(image1)
	draw.line([0, 0, 200, 100], black)
	draw.line([0, 100, 200, 0], red)
	draw.rectangle([50, 25, 150, 75], blue)
	draw.text((50, 10), "Hello, World!", black)
	
	# PIL image can be saved as .png .jpg .gif or .bmp file
	image1.save("pil_test.png")

def fernplot_pil_test(dim=1):
	if dim is 1:
		r = fp.region1()
		r[0] = fp.interval(1.0, 3.0)
		f = fp.fern1(r, 2)
		n = f.begin()
		n.right()
		n.set_leaf_bin(1)
		
		pil_plot(f)
		
	if dim is 2:
		r = fp.region2()
		r.set_uniform(fp.interval(-300.0, 300.0))
		f = fp.fern2(r, 3)
		f.randomize(50)
		print f
		pil_plot(f)

def plot(fern, filename="fern.png"):
	"""main plotting function, saves a png file"""
	it = leaf_iter(fern)
	first = leaf_iter(fern)
	colors = [blue, red, green]
	size = 800
	image = Image.new("RGB", (size, size), white)
	draw = ImageDraw.Draw(image)

	if type(fern) is fp.fern1:
		image = Image.new("RGB", (size, 20), white)
		draw = ImageDraw.Draw(image)
		scale = float(size) / fern.get_bounds(1).span()
		xmin = fern.get_bounds(1).lower 
		x = (0, size)
		
		while True:
			xint = it.region[0]
			x = round((xint.lower-xmin) * scale), \
			    round((xint.upper-xmin) * scale)
			draw.rectangle([x[0], 0, x[1], 20], 
				       colors[it.node.get_leaf_bin()])
			#draw.line outlines?
			draw.line([x[0], 0, x[0], 20], black)
			draw.line([x[1], 0, x[1], 20], black)
			it.next()
			if it.node == first.node: 
				break
		
	elif type(fern) is fp.fern2:
		image = Image.new("RGB", (size, size), white)
		draw = ImageDraw.Draw(image)
		scale = float(size) / fern.get_bounds(1).span(), \
			float(size) / fern.get_bounds(2).span()
		xmin, ymax = fern.get_bounds(1).lower, fern.get_bounds(2).upper
		x, y = (0, size), (0, size)
	
		while True:
			xint, yint = it.region[0], it.region[1]
			x = round((xint.lower-xmin) * scale[0]), \
			    round((xint.upper-xmin) * scale[0])
			y = round((ymax-yint.upper) * scale[1]), \
			    round((ymax-yint.lower) * scale[1])
			draw.rectangle([x[0], y[0], x[1], y[1]],
				       colors[it.node.get_leaf_bin()])
			#draw.line outlines?
			draw.line([x[0], y[0], x[0], y[1]], black)
			draw.line([x[1], y[0], x[1], y[1]], black)
			draw.line([x[0], y[0], x[1], y[0]], black)
			draw.line([x[0], y[1], x[1], y[1]], black)
			it.next()
			if it.node == first.node:
				break
	else:
		print "error: only takes 1D or 2D fern inputs"
		return
	
	image.save(filename)

