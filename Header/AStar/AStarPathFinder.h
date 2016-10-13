#pragma once

#include <vector>
#include <set>
#include <map>
using namespace std;

struct PathPoint 
{
	PathPoint() : x(0), z(0)
	{

	}

	bool operator == (const PathPoint& PathPoint) const
	{
		return ((x == PathPoint.x) && (z == PathPoint.z));
	}
	PathPoint & operator = ( const PathPoint & t )
	{
		x = t.x;
		z = t.z;
		return ( *this ) ;
	}
	int x, z;
};
typedef vector<PathPoint> PATHVEC;
typedef std::vector< std::vector<int> > VecMapData;

struct PathNode
{
	PathNode() : mParent(NULL)
	{

	}

	PathPoint	mPathPoint;		// 节点所处的平面点
	float		mF;				// 节点估价值, f = g + h
	float		mG;				// g值, 与TILESIZE相关
	float		mH;				// h值，为当前点到目标点的距离
	PathNode*	mParent;		// 该节点的父节点
};


struct NodeSort
{
	bool operator() (const PathNode* node1, const PathNode* node2) const
	{
		return node1->mF < node2->mF;
	}
};

class AStarPathFinder 
{
public:
	typedef multiset<PathNode*, NodeSort> NODESET;
	typedef NODESET::iterator NODESETITER;
	typedef map<DWORD, PathNode*> NODEFINDMAP;
	typedef NODEFINDMAP::iterator NODEMAPITER;

	AStarPathFinder();
	~AStarPathFinder();

	// 清理
	void clear();
	// 搜寻路径
	// @sx, sy 起始点坐标， @dx， dy目标点坐标
	bool findPath(int sx, int sz, int dx, int dz, VecMapData& mapData);
	// 得到路径容器
	PATHVEC& getPath();

private:
	// 生成最优点的子节点
	// @bestNode 最优点
	void generateNodes(PathNode* bestNode, int dx, int dz, VecMapData& mapData );
	bool CanMove( VecMapData& mapData, int x, int z );
	// 判断最优点，同时更新OPEN和CLOSED表
	void updateNode(PathNode* bestNode, int sx, int sz, int dx, int dz, float tileSize);
	// 是否是目标点
	bool isDestNode(PathNode* node);
	// 得到最优点，通过返回已排序的map结构的第一个节点获得
	PathNode* getBestNode();
	// 设置路径，通过获得目标点的连续父节点直到起始点，反过来就是路径
	void setPath(PathNode* node, int sx, int sz);

	void insertNodeInOpen(PathNode* node);
	void deleteNodeFromeOpen(int sx, int sz);
	void insertNodeInClosed(PathNode* node);
	void deleteNodeFromeClosed(int sx, int sz);
	PathNode* findOpenNode(PathNode* node);
	PathNode* findClosedNode(PathNode* node);

private:
	// OPEN表， 存储未遍历过的节点, 第一位是节点数据
	// 用于排序
	NODESET	mOpen;
	// CLOSED表，存储遍历过的节点，第一位是节点数据
	// 用于排序
	NODESET	mClosed;
	NODEFINDMAP mFindOpen;
	NODEFINDMAP mFindClosed;

	// 保存最优路径
	PATHVEC	mPath;
	// 展开节点时的参考步长，既向四方1个单位展开节点
	float TILESIZE;
	float TILESIZE2;
};
