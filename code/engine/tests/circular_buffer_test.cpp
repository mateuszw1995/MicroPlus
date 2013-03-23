#include "header.h"

TEST(CircularBuffer, IsEmptyInitially) {
	circular_buffer<int> q(1);
	EXPECT_TRUE(q.empty());
	circular_buffer<int> q2(231);
	EXPECT_TRUE(q2.empty());
	circular_buffer<int> q3(2431);
	EXPECT_TRUE(q3.empty());
}

TEST(CircularBuffer, FrontBackEqualWhenOneElement1) {
	circular_buffer<int> q(13);
	EXPECT_TRUE(q.empty());
	q.push_back(1);
	EXPECT_EQ(1, q.front());
	EXPECT_EQ(1, q.back());
}

TEST(CircularBuffer, FrontBackEqualWhenOneElement2) {
	circular_buffer<int> q(1);
	EXPECT_TRUE(q.empty());
	q.push_back(1);
	EXPECT_EQ(1, q.front());
	EXPECT_EQ(1, q.back());
	q.pop_back();
	EXPECT_TRUE(q.empty());
	q.push_back(4);
	EXPECT_EQ(4, q.front());
	EXPECT_EQ(4, q.back());
	q.pop_front();
	EXPECT_TRUE(q.empty());
	q.push_back(443);
	EXPECT_EQ(443, q.front());
	EXPECT_EQ(443, q.back());
}

TEST(CircularBuffer, DequeueWorks1) {
	circular_buffer<int> q(14);
	q.push_back(1);
	q.push_back(1);
	q.push_back(1);
	q.push_back(4);
	q.push_back(1);
	q.push_back(1);
	q.push_back(1);
	q.push_back(1);
	q.push_back(1);
	q.push_back(1);
	q.push_back(4);
	q.push_back(4);
	q.push_back(4);
	EXPECT_EQ(1, q.front());
	EXPECT_EQ(4, q.back());
	q.pop_back();
	q.push_back(6);
	EXPECT_EQ(1, q.front());
	EXPECT_EQ(6, q.back());
	q.pop_back();
	q.push_back(443);
	EXPECT_EQ(1, q.front());
	EXPECT_EQ(443, q.back());
}


TEST(CircularBuffer, DequeueWorks2) {
	circular_buffer<int> q(13);
	q.push_back(1);
	q.push_back(2);
	q.push_back(3);
	q.push_back(4);
	q.push_back(1);
	q.push_back(1);
	q.push_back(1);
	q.push_back(1);
	q.push_back(1);
	q.push_back(1);
	q.push_back(4);
	q.push_back(4);
	q.push_back(4);
	EXPECT_EQ(1, q.front());
	EXPECT_EQ(4, q.back());
	q.pop_front();
	q.pop_back();
	q.push_back(6);
	EXPECT_EQ(2, q.front());
	EXPECT_EQ(6, q.back());
	q.pop_front();
	q.pop_back();
	q.push_back(443);
	EXPECT_EQ(3, q.front());
	EXPECT_EQ(443, q.back());
}


TEST(CircularBuffer, MultiplePush) {
	circular_buffer<int> q(2);
	int vals [] = { 0, 1, 2, 3, 4, 5 };
	q.push_back(vals, 2);
	EXPECT_EQ(0, q.front());
	EXPECT_EQ(1, q.back());
	q.pop_back();
	EXPECT_EQ(0, q.front());
	EXPECT_EQ(0, q.back());
	q.push_back(vals+1, 1);
	EXPECT_EQ(0, q.front());
	EXPECT_EQ(1, q.back());
}


TEST(CircularBuffer, MultiplePushAndOperator) {
	circular_buffer<int> q(10);
	int vals [] = { 0, 1, 2, 3, 4, 5 };
	q.push_back(vals, 6);
	EXPECT_EQ(0, q.front());
	EXPECT_EQ(2, q[2]);
	EXPECT_EQ(5, q.back());
	q.pop_back();
	EXPECT_EQ(0, q.front());
	EXPECT_EQ(4, q.back());
	q.push_back(vals+5, 1);
	EXPECT_EQ(0, q.front());
	EXPECT_EQ(5, q.back());
	q.pop_front();
	q.pop_front();
	q.pop_front();
	q.pop_front();
	q.pop_front();
	EXPECT_EQ(5, q.front());
	EXPECT_EQ(5, q.back());
	q.push_back(vals, 6);
	EXPECT_EQ(5, q.front());
	EXPECT_EQ(5, q.back());
	q.pop_front();
	EXPECT_EQ(q.size(), 6);
	EXPECT_EQ(0, q[0]);
	EXPECT_EQ(1, q[1]);
	EXPECT_EQ(2, q[2]);
	EXPECT_EQ(3, q[3]);
	EXPECT_EQ(4, q[4]);
	EXPECT_EQ(5, q[5]);
}

TEST(CircularBuffer, AllTogetherPlusClears) {
	circular_buffer<int> q(10);
	EXPECT_TRUE(q.empty());
	q.clear();
	EXPECT_TRUE(q.empty());
	q.clear();
	EXPECT_TRUE(q.empty());

	int vals [] = { 0, 1, 2, 3, 4, 5 };
	q.push_back(vals, 6);
	EXPECT_EQ(0, q.front());
	EXPECT_EQ(2, q[2]);
	EXPECT_EQ(5, q.back());
	q.pop_back();
	EXPECT_EQ(0, q.front());
	EXPECT_EQ(4, q.back());
	q.push_back(vals+5, 1);
	EXPECT_EQ(0, q.front());
	EXPECT_EQ(5, q.back());
	q.pop_front();
	q.pop_front();
	q.pop_front();
	q.pop_front();
	q.pop_front();
	EXPECT_EQ(5, q.front());
	EXPECT_EQ(5, q.back());
	q.push_back(vals, 6);
	EXPECT_EQ(5, q.front());
	EXPECT_EQ(5, q.back());
	q.pop_front();
	EXPECT_EQ(q.size(), 6);
	EXPECT_EQ(0, q[0]);
	EXPECT_EQ(1, q[1]);
	EXPECT_EQ(2, q[2]);
	EXPECT_EQ(3, q[3]);
	EXPECT_EQ(4, q[4]);
	EXPECT_EQ(5, q[5]);
	q.clear();


	q.push_back(1);
	q.push_back(1);
	q.push_back(1);
	q.push_back(4);
	q.push_back(1);
	q.push_back(1);
	q.push_back(4);
	q.push_back(4);
	q.push_back(4);
	EXPECT_EQ(1, q.front());
	EXPECT_EQ(4, q.back());
	q.pop_back();
	q.push_back(6);
	EXPECT_EQ(1, q.front());
	EXPECT_EQ(6, q.back());
	q.pop_back();
	q.push_back(443);
	EXPECT_EQ(1, q.front());
	EXPECT_EQ(443, q.back());

	q.clear();


	q.push_back(vals, 2);
	EXPECT_EQ(0, q.front());
	EXPECT_EQ(1, q.back());
	q.pop_back();
	EXPECT_EQ(0, q.front());
	EXPECT_EQ(0, q.back());
	q.push_back(vals+1, 1);
	EXPECT_EQ(0, q.front());
	EXPECT_EQ(1, q.back());
}