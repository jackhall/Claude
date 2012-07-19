import fernpy as fp
import Tkinter as tk

class leaf_iter:
	node = fp.nodehandle1()
	region = fp.region1()
	dimensions = 1
	
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
		
		while not self.node.is_leaf():
			#look at forks as you go down to keep track of active region
			self.node.left()
			
	def next():
		
	
	
		
		

def plot(fern):
	if type(fern) is fp.fern1:
		pass
	elif type(fern) is fp.fern2:
		pass
	else:
		print "error: only takes 1D or 2D fern inputs"
		return

def tkinter_test():
	master = tk.Tk()
	w = tk.Canvas(master, width=200, height=100)
	w.pack()
	w.create_line(0, 0, 200, 100)
	w.create_line(0, 100, 200, 0, fill="red", dash=(4, 4))
	w.create_rectangle(50, 25, 150, 75, fill="blue")

	tk.mainloop()

