#include "gtest/gtest.h"

#include <surface_mesh/EdgeSet.h>
#include <vector>

using namespace surface_mesh;

class EdgeSetTest : public ::testing::Test
{
public:
    EdgeSet es;
};

TEST_F(EdgeSetTest, operatorNew)
{
    EdgeSet* es = new EdgeSet();
    EXPECT_EQ(es->nVertices(), 0);
    EXPECT_EQ(es->nEdges(), 0);
}

TEST_F(EdgeSetTest, operatorEq)
{
    EdgeSet es2 = es;
    EXPECT_EQ(es2.nVertices(), 0);
}

TEST_F(EdgeSetTest, assignment)
{
    EdgeSet es2;
    es2.assign(es);
    EXPECT_EQ(es2.nVertices(), 0);
}

TEST_F(EdgeSetTest, addRemove)
{
    Point p0(0,0,0);
    Point p1(1,0,0);
    Point p2(2,0,0);
    auto v0  = es.addVertex(p0);
    auto v1  = es.addVertex(p1);
    auto v2  = es.addVertex(p2);
    EXPECT_EQ(es.nVertices(), 3);

    auto e0 = es.insertEdge(v0,v1);
    es.insertEdge(v1,v2);
    EXPECT_EQ(es.nEdges(), 2);

    size_t nv(0);
    for (auto v : es.vertices())
    {
        if (v.isValid())
            nv++;
    }
    EXPECT_EQ(nv, 3);

    size_t ne(0);
    for (auto e : es.edges())
    {
        if (e.isValid())
            ne++;
    }
    EXPECT_EQ(ne, 2);

    size_t nh(0);
    for (auto h : es.halfedges())
    {
        if (h.isValid())
            nh++;
    }
    EXPECT_EQ(nh, 4);

    size_t nvv = 0;
    for (auto vv : es.vertices(v1))
    {
        if (vv.isValid())
            nvv++;
    }
    EXPECT_EQ(nvv, 2);

    size_t nvh = 0;
    for (auto vh : es.halfedges(v1))
    {
        if (vh.isValid())
            nvh++;
    }
    EXPECT_EQ(nvh, 2);

    es.deleteEdge(es.edge(e0));
    EXPECT_TRUE(es.isDeleted(e0));
    es.garbageCollection();
    EXPECT_EQ(es.nEdges(), 1);

    es.clear();
    EXPECT_EQ(es.nVertices(), 0);
}


//=============================================================================
