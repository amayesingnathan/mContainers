#include "pch.h"

#include "TestObject.h"

namespace mContainers {

	class VectorFixtures : public ::testing::Test
	{
	protected:
		mDynArray<Vec3> defaultConstruct;

		Vec3 testObj1;
		Vec3 testObj2;

		int scalar = 35;

		VectorFixtures()
			: defaultConstruct(),
			testObj1(4, 5, 6), testObj2(6, 1, 6) {}

		virtual void SetUp() override
		{
			defaultConstruct.push_back(testObj1);
			defaultConstruct.push_back(testObj2);
			defaultConstruct.emplace_back(scalar);
		}

		void Reset()
		{
			defaultConstruct.push_back(testObj1);
			defaultConstruct.push_back(testObj2);
			defaultConstruct.emplace_back(scalar);
		}
	};

	TEST_F(VectorFixtures, VecConstructors)
	{
		EXPECT_TRUE(defaultConstruct.size() == 3);
		EXPECT_TRUE(defaultConstruct.capacity() >= 3);
	}

	TEST_F(VectorFixtures, VecModifiers)
	{
		EXPECT_TRUE(defaultConstruct[0] == testObj1);
		EXPECT_TRUE(defaultConstruct[1] == testObj2);
		EXPECT_TRUE(defaultConstruct[2] == scalar);
	}

}
