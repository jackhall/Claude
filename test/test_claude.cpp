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
//	g++ -std=c++11 -g -I../src -lgtest test_claude.cpp -o test_claude
//	./test_claude

#include <iostream>
#include "Fern.h"
#include "gtest/gtest.h"

namespace {

	class FernTest : public ::testing::Test {
	protected:
		FernTest() {
	    	// You can do set-up work for each test here.
	  	}

	  	virtual ~FernTest() {
	    	// You can do clean-up work that doesn't throw exceptions here.
	  	}
	};

	// Tests that the Foo::Bar() method does Abc.
	TEST_F(FernTest, MethodBarDoesAbc) {
	  const string input_filepath = "this/package/testdata/myinputfile.dat";
	  const string output_filepath = "this/package/testdata/myoutputfile.dat";
	  Fern f;
	  EXPECT_EQ(0, f.Bar(input_filepath, output_filepath));
	}

	// Tests that Foo does Xyz.
	TEST_F(FernTest, DoesXyz) {
	  // Exercises the Xyz feature of Foo.
	}

}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

