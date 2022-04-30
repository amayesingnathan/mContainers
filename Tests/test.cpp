#include "pch.h"

#include "TestObject.h"

namespace Containers {

	class VectorFixtures : public ::testing::Test
	{
	protected:
		Vector<Vec3> defaultConstruct;
		Vector<Vec3> valueConstruct;

		Vec3 testObj1;
		Vec3 testObj2;

		int scalar = 35;

		VectorFixtures()
			: defaultConstruct(), valueConstruct(8),
			testObj1(4, 5, 6), testObj2(6, 1, 6) {}

		virtual void SetUp() override
		{
			defaultConstruct.push_back(testObj1);
			defaultConstruct.push_back(testObj2);
			defaultConstruct.emplace_back(scalar);

			valueConstruct.push_back(testObj1);
			valueConstruct.push_back(testObj2);
			valueConstruct.emplace_back(scalar);
		}

		void Reset()
		{
			defaultConstruct.push_back(testObj1);
			defaultConstruct.push_back(testObj2);
			defaultConstruct.emplace_back(scalar);

			valueConstruct.push_back(testObj1);
			valueConstruct.push_back(testObj2);
			valueConstruct.emplace_back(scalar);
		}
	};

	TEST_F(VectorFixtures, VecConstructors)
	{
		EXPECT_TRUE(defaultConstruct.size() == 3);
		EXPECT_TRUE(defaultConstruct.capacity() >= 3);

		EXPECT_TRUE(valueConstruct.size() == 11);
		EXPECT_TRUE(valueConstruct.capacity() >= 11);
	}

	TEST_F(VectorFixtures, VecModifiers)
	{
		EXPECT_TRUE(defaultConstruct[0] == testObj1);
		EXPECT_TRUE(defaultConstruct[1] == testObj2);
		EXPECT_TRUE(defaultConstruct[2] == scalar);

		EXPECT_TRUE(valueConstruct[0] == 0);
		EXPECT_TRUE(valueConstruct[8] == testObj1);
		EXPECT_TRUE(valueConstruct[9] == testObj2);
		EXPECT_TRUE(valueConstruct[10] == scalar);
	}

}
