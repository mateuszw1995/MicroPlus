#include "header.h"

TEST(CircularBuffer, IsEmptyInitially) {
	circular_buffer<int> q(1);
	EXPECT_TRUE(q.empty());
}

TEST(CircularBuffer, DequeueWorks1) {
	circular_buffer<int> q(1);
	EXPECT_TRUE(q.empty());
	q.push(1);
	EXPECT_EQ(1, q.front());
	EXPECT_EQ(1, q.back());
	q.pop();
	EXPECT_TRUE(q.empty());
	q.push(4);
	EXPECT_EQ(4, q.front());
	EXPECT_EQ(4, q.back());
	q.pop();
	EXPECT_TRUE(q.empty());
	q.push(443);
	EXPECT_EQ(443, q.front());
	EXPECT_EQ(443, q.back());
}

TEST(CircularBuffer, DequeueWorks2) {
	circular_buffer<int> q(13);
	q.push(1);
	q.push(1);
	q.push(1);
	q.push(4);
	q.push(1);
	q.push(1);
	q.push(1);
	q.push(1);
	q.push(1);
	q.push(1);
	q.push(4);
	q.push(4);
	q.push(4);
	EXPECT_EQ(1, q.front());
	EXPECT_EQ(4, q.back());
	q.pop();
	q.push(6);
	EXPECT_EQ(1, q.front());
	EXPECT_EQ(6, q.back());
	q.pop();
	q.push(443);
	EXPECT_EQ(1, q.front());
	EXPECT_EQ(443, q.back());
}