#include "gtest/gtest.h"

#include <surface_mesh/PointSet.h>

using namespace surface_mesh;

TEST(GeometryObjectTest, properties)
{
    PointSet ps;

    // explicit add
    auto midx = ps.addObjectProperty<int>("m:idx");
    midx[0] = 0;
    EXPECT_EQ(ps.objectProperties().size(), 1);
    ps.removeObjectProperty(midx);
    EXPECT_EQ(ps.objectProperties().size(), 0);

    // implicit add
    midx = ps.objectProperty<int>("m:idx2");
    EXPECT_EQ(ps.objectProperties().size(), 1);
    ps.removeObjectProperty(midx);
    EXPECT_EQ(ps.objectProperties().size(), 0);
}


//=============================================================================
