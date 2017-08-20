#include "gtest/gtest.h"

#include <surface_mesh/PointSet.h>
#include <vector>

using namespace surface_mesh;

class PointSetTest : public ::testing::Test
{
public:
    PointSet ps;
    Point p0{0,0,0};
    Point p1{1,1,1};
};

// test with new in order to catch all created functions
TEST_F(PointSetTest, instantiate)
{
    PointSet* ps = new PointSet();
    EXPECT_EQ(ps->nVertices(), 0);
}

TEST_F(PointSetTest, operatorEq)
{
    ps.addVertex(p0);
    ps.addVertex(p1);
    PointSet ps2 = ps;
    EXPECT_EQ(ps2.nVertices(), 2);
}

TEST_F(PointSetTest, assignment)
{
    ps.addVertex(p0);
    ps.addVertex(p1);
    PointSet ps2;
    ps2.assign(ps);
    EXPECT_EQ(ps2.nVertices(), 2);
}

TEST_F(PointSetTest, addRemove)
{
    PointSet ps;
    ps.reserve(2);
    auto  v1 = ps.addVertex(p0);
    ps.addVertex(p1);
    EXPECT_EQ(ps.nVertices(), 2);
    EXPECT_EQ(ps.pointVector().size(), 2);
    ps.deleteVertex(v1);
    ps.garbageCollection();
    EXPECT_EQ(ps.nVertices(), 1);
    EXPECT_EQ(ps.pointVector().size(), 1);
    ps.clear();
    EXPECT_EQ(ps.nVertices(), 0);
    EXPECT_EQ(ps.pointVector().size(), 0);
}

TEST_F(PointSetTest, iterators)
{
    size_t   nv(0);
    ps.addVertex(p0);
    ps.addVertex(p1);
    for (auto v : ps.vertices())
    {
        SM_ASSERT(v.isValid());
        nv++;
    }
    EXPECT_EQ(nv, 2);
}

TEST_F(PointSetTest, skipDeleted)
{
    size_t   nv(0);
    auto     v1 = ps.addVertex(p0);
    ps.addVertex(p1);
    ps.deleteVertex(v1);
    for (auto v : ps.vertices())
    {
        SM_ASSERT(v.isValid());
        nv++;
    }
    EXPECT_EQ(nv, 1);
}

TEST_F(PointSetTest, vertexProperties)
{
    // explicit add
    auto v0 = ps.addVertex(p1);
    auto vidx = ps.addVertexProperty<int>("v:idx");
    vidx[v0] = 0;
    EXPECT_EQ(ps.vertexProperties().size(), 4);
    ps.removeVertexProperty(vidx);
    EXPECT_EQ(ps.vertexProperties().size(), 3);

    // implicit add
    vidx = ps.vertexProperty<int>("v:idx2");
    EXPECT_EQ(ps.vertexProperties().size(), 4);
    ps.removeVertexProperty(vidx);
    EXPECT_EQ(ps.vertexProperties().size(), 3);
}

TEST_F(PointSetTest, write)
{
    ps.addVertex(p0);
    ps.addVertex(p1);
    ps.write("test.xyz");
    ps.clear();
    EXPECT_EQ(ps.nVertices(), 0);
}

TEST_F(PointSetTest, read)
{
    ps.read("test.xyz"); // bad test dependency
    EXPECT_EQ(ps.nVertices(), 2);
}

TEST_F(PointSetTest, readFailure)
{
    ASSERT_FALSE(ps.read("test.off"));
}
