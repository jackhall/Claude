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
//	g++ -std=c++11 -g -I../src test_claude.cpp -o test_claude
//	./test_claude

#include <iostream>
#include "Fern.h"

int main() {
	using namespace std;
	using namespace clau;
	
	cout << "Testing default construction" << endl;
	Fern<1> fern1;
	cout << fern1 << "===================" << endl;
	
	cout << "Testing normal construction" << endl;
	Interval interval = {0.0, 1.0};
	Region<2> region;
	region.set_uniform(interval);
	cout << "Initial Region:" << endl;
		for(int i=2; i>0; i--)
			cout << "\t[" << region(i).lower << ", " << region(i).upper << "]" << endl; 
	Fern<2> fern2(region, 3);
	cout << fern2 << "===================" << endl;
	
	cout << "Testing copy construction" << endl;
	auto fern3(fern2);
	cout << fern3 << "===================" << endl;
	/*	
	cout << "Testing splits" << endl;
	fern2.test_splitting();
	cout << fern2 << endl;
	
	cout << "Testing copies" << endl;
	fern1.set_bounds(0.0, 1.0); 
	fern1.set_bins(3);
	fern1.test_copying(fern2); 
	cout << fern1 << endl;
	
	cout << "Testing merges" << endl;
	fern2.test_merging();
	cout << fern2 << endl;
	*/
	return 0;
}

