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
		EXPECT_EQ(region, fern.get_region());
		
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
		EXPECT_EQ(region, fern.get_region());
		
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
		EXPECT_EQ(region, fern.get_region());
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
			node.right().set_leaf_bin(0);
		
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
		EXPECT_TRUE(node.right().set_leaf_bin(0));
		
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
		
		//merge 2nd and 3rd leaves, preserving 3rd leaf's bin
		node.left().right();
		node.merge_fork();
		EXPECT_FALSE(node.is_ghost());
		EXPECT_FALSE(node.is_root());
		EXPECT_TRUE(node.is_leaf());
		EXPECT_EQ(0, node.get_leaf_bin());
		
		//merge 1st and 2nd leaves, preserving 1st leaf's bin
		node.up().merge_fork();
		EXPECT_EQ(1, node.get_leaf_bin());
	}
	
	TEST_F(NodeManipulationTest, Splicing) {
		using namespace clau;
		ExpandFern();
		
		//construct origin fern, differing only by root.left.value.bit
		Fern<2> origin_fern(region, num_bins);
		auto node2 = origin_fern.begin();
		node2.left();
		EXPECT_TRUE(node2.set_leaf_bin(1));
		Fern<2>::Division division(false, 2);
		EXPECT_TRUE(node2.split_leaf(division));
		EXPECT_TRUE(node2.right().split_leaf(division));
		EXPECT_TRUE(node2.right().set_leaf_bin(0));
		EXPECT_TRUE(node2.root().right().set_leaf_bin(2));
		division.bit = true;
		division.dimension = 1;
		EXPECT_TRUE(node2.split_leaf(division));
		node2.root().left();
		
		//splice at root.left and check results
		node.left();
		EXPECT_TRUE(node.splice(node2));
		EXPECT_FALSE(node.is_ghost());
		EXPECT_FALSE(node2.is_ghost());
		EXPECT_FALSE(node.is_root());
		EXPECT_FALSE(node.get_fork_bit());
		EXPECT_FALSE(node2.get_fork_bit());
	}
	
	class FernTest : public ::testing::Test { //very similar to NodeManipulationTest fixture
	protected:
		clau::Interval span1, span2;
		clau::Region<2> region;
		clau::bin_type num_bins;
		clau::Fern<2> fern;
		clau::Fern<2>::node_handle node;
		
		FernTest() : span1(0.0, 1.0), span2(2.0, 4.0), 
				region(), num_bins(3), 
				fern(num_bins), node() {
			region(1) = span1;
			region(2) = span2;
			fern.set_bounds(region);
			node = fern.begin();
		}
		
		void ExpandFern() {
			using namespace clau;
			node.left().set_leaf_bin(1);
			Fern<2>::Division division(true, 2);
			node.split_leaf(division);
			node.left().set_leaf_bin(0);
			node.up();
		
			node.right();
			division.bit = false;
			node.split_leaf(division);
			node.right().set_leaf_bin(0);
		
			node.root().right();
			node.set_leaf_bin(2);
			division.bit = true;
			division.dimension = 1;
			node.split_leaf(division);
			node.right().set_leaf_bin(0);
			node.root();
		}
		
		bool CheckEqual(clau::Fern<2>& one, clau::Fern<2>& two) {
			EXPECT_EQ(one.get_region(), two.get_region());
			EXPECT_EQ(one.get_num_bins(), two.get_num_bins());
		
			auto iter = one.sbegin();
			auto iter2 = two.sbegin();
			while( !iter.is_null() && !iter2.is_null() ) {
				EXPECT_EQ(iter.is_leaf(), iter2.is_leaf());
				if( iter.is_leaf() ) {
					if( iter.get_leaf_bin() != iter2.get_leaf_bin() ) 
						return false;
				} else {
					if( iter.get_fork_boundary() != iter2.get_fork_boundary() ) 
						return false;
				}
				++iter; ++iter2;
			}
			EXPECT_TRUE( iter.is_null() && iter2.is_null() );
			return true;
		}
		
		void CheckQuery() {
			using namespace clau;
			Point<2> point;
			point(1) = 0.38;
			point(2) = 3.234;
			EXPECT_EQ(0, fern.query(point));
			
			point(2)= 3.238;
			EXPECT_EQ(1, fern.query(point));
			
			point(2) = 3.526;
			EXPECT_EQ(1, fern.query(point));
			
			point(2) = 3.53;
			EXPECT_EQ(0, fern.query(point));
			
			point(1) = .384;
			point(2) = 2.618;
			EXPECT_EQ(2, fern.query(point));
			
			point(2) = 3.382;
			EXPECT_EQ(2, fern.query(point));
			
			point(2) = 3.764;
			EXPECT_EQ(2, fern.query(point));
			
			point(1) = .762;
			point(2) = 3.0;
			EXPECT_EQ(2, fern.query(point));
			
			point(1) = .766;
			EXPECT_EQ(0, fern.query(point));
		}
		
		//virtual void SetUp() {}
		//virtual void TearDown() {}

	  	virtual ~FernTest() {
	    	// You can do clean-up work that doesn't throw exceptions here.
	  	}
	};
	
	TEST_F(FernTest, Querying) {
		using namespace clau;
		ExpandFern();
		CheckQuery();
		//check what happens after split and merge? 
		//check what happens after copy?
	}

	TEST_F(FernTest, Copying) {
		using namespace clau;
		ExpandFern();
		Fern<2> fern2(fern);
		EXPECT_TRUE(CheckEqual(fern, fern2));
		
		//flip a bit and check boundaries again
		node.left();
		EXPECT_TRUE(node.set_fork_bit(false));
		EXPECT_FALSE(CheckEqual(fern, fern2));
		
		//copy assignment to reverse the previous change
		fern = fern2;
		EXPECT_TRUE(CheckEqual(fern, fern2));
	}
	
	//write CheckValid() to check random operations?
	//check for randomness?
	
}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

