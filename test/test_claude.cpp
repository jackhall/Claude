/*
    Claude: a real-to-discrete coding scheme based on spiraling trees
    Copyright (C) 2012  Jack Hall

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    e-mail: jackwhall7@gmail.com
*/

//to test Claude, run the following from the test directory:
//	g++ -std=c++0x -g -I../src -lClaude test_claude.cpp -o test_claude
//	./test_claude

#include <iostream>
#include "Claude.h"

int main() {
	using namespace std;
	using namespace clau;
	
	cout << "Testing default construction" << endl;
	Fern fern1;
	cout << fern1 << endl;
	
	cout << "Testing copy construction" << endl;
	Fern fern3(fern1);
	cout << fern3 << endl;
	
	cout << "Testing normal construction" << endl;
	Fern fern2(0.0, 1.0, 3);
	cout << fern2 << endl;
	
	cout << "Testing splits" << endl;
	fern2.test_splitting();
	cout << fern2 << endl;
	
	cout << "Testing copies" << endl;
	fern1.set_bounds(0.0, 1.0); 
	fern1.set_bins(3);
	fern1.test_copying(fern2); //fixed with major-league kludge
	cout << fern1 << endl;
	
	cout << "Testing merges" << endl;
	fern2.test_merging();
	cout << fern2 << endl;
	
	cout << "Testing mutation" << endl;
	fern3.set_bounds(0.0, 1.0); 
	fern3.set_bins(3);
	fern3.mutate();
	cout << fern3 << endl;
	fern3.mutate();
	cout << fern3 << endl;
	
	cout << "Testing crossover" << endl;
	fern2.test_splitting();
	cout << fern2 << endl;
	fern2.mutate();
	fern2.mutate();
	cout << fern2 << endl;
	cout << fern1 << endl;
	fern1.crossover(fern2);
	cout << fern1 << endl;
	
	return 0;
}
