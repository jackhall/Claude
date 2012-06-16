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
//	g++ -std=c++11 -g -I../src -lgtest -lpthread test_claude.cpp -o test_claude
//	./test_claude

#include <iostream>
#include "Fern.h"
#include "gtest/gtest.h"

namespace {

	TEST(ConstructionTests, DefaultConstruction) {
		using namespace clau;
		Fern<1> fern;
		
		//verify default Fern properties
		EXPECT_EQ(1, fern.get_num_bins());
		
		Region<1> region;
		region.set_uniform(Interval());
		EXPECT_EQ(region, fern.get_bounds());
		
		//verify default Fern structure
		auto node = fern.begin();
		ASSERT_TRUE(node.is_root());
		EXPECT_EQ(1, node.get_fork_dimension());
		EXPECT_FALSE(node.get_fork_bit());
		
		node.left();
		EXPECT_TRUE(node.is_leaf());
		EXPECT_EQ(0, node.get_leaf_bin());
		
		node.up().right();
		EXPECT_TRUE(node.is_leaf());
		EXPECT_EQ(0, node.get_leaf_bin());
		
		//verify ability to set bounds manually
		region(1).lower = 1.0;
		region(1).upper = 2.0;
		fern.set_bounds(region);
		EXPECT_EQ(region, fern.get_bounds());
		
		//haven't written a way to set num_bins manually
	}
	
	TEST(ConstructionTests, NormalConstruction) {
		using namespace clau;
		
		Interval span1(0.0, 1.0);
		Interval span2(2.0, 4.0);
		Region<2> region;
		region(1) = span1;
		region(2) = span2;
		bin_type num_bins = 3;
		Fern<2> fern(region, num_bins);
		
		//verify Fern properties
		EXPECT_EQ(region, fern.get_bounds());
		EXPECT_EQ(num_bins, fern.get_num_bins());
		
		//verify Fern structure
		auto node = fern.begin();
		ASSERT_TRUE(node.is_root());
		EXPECT_EQ(1, node.get_fork_dimension());
		EXPECT_FALSE(node.get_fork_bit());
		
		node.left();
		EXPECT_TRUE(node.is_leaf());
		EXPECT_EQ(0, node.get_leaf_bin());
		
		node.up().right();
		EXPECT_TRUE(node.is_leaf());
		EXPECT_EQ(0, node.get_leaf_bin());
	}
	
	class NodeManipulationTest : public ::testing::Test {
	protected:
		clau::Interval span1, span2;
		clau::Region<2> region;
		clau::bin_type num_bins;
		clau::Fern<2> fern;
		clau::Fern<2>::node_handle node;
		
		NodeManipulationTest() : span1(0.0, 1.0), span2(2.0, 4.0), 
					 region(), num_bins(3), 
					 fern(num_bins), node() {
			region(1) = span1;
			region(2) = span2;
			fern.set_bounds(region);
			node = fern.begin();
		}
		
		void ExpandFern() { //for Pruning and Splicing cases
			using namespace clau;
			node.left().set_leaf_bin(1);
			Fern<2>::Division division(true, 2);
			node.split_leaf(division);
		
			node.right();
			division.bit = false;
			node.split_leaf(division);
		
			node.root().right();
			EXPECT_TRUE(node.set_leaf_bin(2));
			division.bit = true;
			division.dimension = 1;
			node.split_leaf(division);
			node.root();
			//final fern is identical to that from Growing case
			//has all 4 division permutations (value.bit and 2 dimensions)
		}
	};
	
	TEST_F(NodeManipulationTest, Setting) {
		using namespace clau;
		//check setup/initialization
		ASSERT_TRUE(node == fern.begin());
		ASSERT_TRUE(node.belongs_to(fern));
		
		//check fork setting
		EXPECT_TRUE(node.set_fork_bit(true));
		EXPECT_TRUE(node.get_fork_bit());
		
		EXPECT_TRUE(node.set_fork_dimension(2));
		EXPECT_FALSE(node.set_fork_dimension(0));
		EXPECT_EQ(2, node.get_fork_dimension());
		
		//check leaf setting
		node.left();
		EXPECT_TRUE(node.set_leaf_bin(1));
		EXPECT_FALSE(node.set_leaf_bin(3));
		EXPECT_EQ(1, node.get_leaf_bin());
		
		node.up().right();
		EXPECT_TRUE(node.set_leaf_bin(2));
		EXPECT_EQ(2, node.get_leaf_bin());
		EXPECT_FALSE(node.set_fork_bit(false));
	}

	TEST_F(NodeManipulationTest, Growing) {
		//builds fern identical to NodeManipulationTest::ExpandTree()
		using namespace clau;
		node.left().set_leaf_bin(1);
		Fern<2>::Division division(true, 2);
		EXPECT_TRUE(node.split_leaf(division));
		//check aftermath of split
		ASSERT_FALSE(node.is_leaf());
		EXPECT_FALSE(node.is_root());
		EXPECT_FALSE(node.is_ghost());
		EXPECT_TRUE(node.get_fork_bit());
		EXPECT_EQ(2, node.get_fork_dimension());
		
		//check new leaf children
		node.left();
		EXPECT_TRUE(node.is_leaf());
		EXPECT_EQ(1, node.get_leaf_bin());
		
		node.up().right();
		EXPECT_TRUE(node.is_leaf());
		EXPECT_EQ(1, node.get_leaf_bin());
		
		//split new right child, verify new fork properties
		division.bit = false;
		EXPECT_TRUE(node.split_leaf(division));
		EXPECT_FALSE(node.get_fork_bit());
		EXPECT_EQ(2, node.get_fork_dimension());
		
		//split root.right, verify new fork properties
		node.root();
		ASSERT_TRUE(node.is_root());
		node.right();
		EXPECT_TRUE(node.set_leaf_bin(2));
		division.bit = true;
		division.dimension = 1;
		EXPECT_TRUE(node.split_leaf(division));
		EXPECT_TRUE(node.get_fork_bit());
		EXPECT_EQ(1, node.get_fork_dimension());
		
		//check new leaf children
		node.left();
		EXPECT_TRUE(node.is_leaf());
		EXPECT_EQ(2, node.get_leaf_bin());
		
		node.up().right();
		EXPECT_TRUE(node.is_leaf());
		EXPECT_EQ(2, node.get_leaf_bin());
	}
	
	TEST_F(NodeManipulationTest, Pruning) {
		using namespace clau;
		ExpandFern();
	}
	/*
	class FernTest : public ::testing::Test {
	protected:
		clau::Interval span1, span2;
		clau::Region<2> region;
		clau::bin_type num_bins;
		clau::Fern<2> fern;
		
		FernTest() : span1(0.0, 1.0), span2(2.0, 4.0), region(), num_bins(3), fern(num_bins) {
			region(1) = span1;
			region(2) = span2;
			fern.set_bounds(region);
		}
		
		void GrowTree() {
			auto node = fern.begin();
			node.left().split()
		}
		
		//virtual void SetUp() {}
		//virtual void TearDown() {}

	  	virtual ~FernTest() {
	    	// You can do clean-up work that doesn't throw exceptions here.
	  	}
	};
	/*
	TEST_F(FernTest, MethodBarDoesAbc) {

		Fern f;
		EXPECT_EQ(0, f.Bar(input_filepath, output_filepath));
	}

	// Tests that Foo does Xyz.
	TEST_F(FernTest, DoesXyz) {
	  // Exercises the Xyz feature of Foo.
	}
	*/
}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

