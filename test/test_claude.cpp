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
	
	Fern fern1;
	cout << fern1 << endl;
	
	Fern fern2(0.0, 1.0, 3);
	cout << fern2 << endl;
	
	return 0;
}

