#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <iostream>
#include <vector>
#include <queue>

#include "Block.hpp"
#include "Settings.hpp"

#include <MyGL/VertexObject.hpp>

#include <glm/glm.hpp>


struct LightBFSNode
{
	int Pos[3];
	DLightLevel Value;
	bool Origin;
};

class Chunk
{
private:
	Block Grid[CHUNK_INFO_SIZE];
	DLightLevel Light[CHUNK_INFO_SIZE];

	std::queue<LightBFSNode> LightList[2], LightRemovalList[2];

	friend class ChunkLoadingInfo;
	friend class ChunkMeshingInfo;
	friend class ChunkSunLightingInfo;
public:
	bool InitializedSunlight, LoadedTerrain, Meshed;
	static inline int XYZ(const glm::ivec3 &pos);
	static inline int XYZ(int x, int y, int z);
	static inline bool IsValidPosition(const glm::ivec3 &pos);

	glm::ivec3 Position;

	MyGL::VertexObjectPtr VertexBuffer;

	explicit Chunk(const glm::ivec3 &pos);
	void SetBlock(const glm::ivec3 &pos, Block b);
	Block GetBlock(const glm::ivec3 &pos) const;
};
using ChunkPtr = Chunk*;

class ChunkInfo
{
public:
	bool Done = false;
	virtual void Process()= 0;
};

class ChunkLoadingInfo : public ChunkInfo
{
private:
	Block Result[WORLD_HEIGHT * CHUNK_INFO_SIZE];
	glm::ivec2 Position;

public:
	explicit ChunkLoadingInfo(const glm::ivec2 &pos);
	void Process() override;
	void ApplyTerrain(ChunkPtr (&chk)[WORLD_HEIGHT]);
};


struct ChunkRenderVertex
{
	float X, Y, Z, U, V, Tex, Face, AO, SunLight, TorchLight;
};

struct FaceLighting
{
	LightLevel SunLight[4], TorchLight[4], AO[4];
	void SetValues(
			int face,
			const Block (&neighbours)[27],
			const LightLevel (&sunlightNeighbours)[27],
			const LightLevel (&torchlightNeighbours)[27]);
	bool operator== (const FaceLighting &f) const;
	bool operator!= (const FaceLighting &f) const;
};

class ChunkMeshingInfo : public ChunkInfo
{
private:
	Block Grid[EXCHUNK_INFO_SIZE];
	DLightLevel Light[EXCHUNK_INFO_SIZE];
	glm::ivec3 Position;

	static inline int ExXYZ(int x, int y, int z);
	inline Block GetBlock(int x, int y, int z);
	inline LightLevel GetSunLight(int x, int y, int z);
	inline LightLevel GetTorchLight(int x, int y, int z);
	inline bool ShowFace(Block now, Block neighbour);

	std::vector<ChunkRenderVertex> Result;

public:
	explicit ChunkMeshingInfo(ChunkPtr (&chk)[27]);
	void Process() override;
	void ApplyMesh(ChunkPtr chk);
};


class ChunkSunLightingInfo : public ChunkInfo
{
private:
	LightLevel Grid[WORLD_HEIGHT * CHUNK_INFO_SIZE];
	LightLevel Result[WORLD_HEIGHT * CHUNK_INFO_SIZE];

public:
	explicit ChunkSunLightingInfo(ChunkPtr (&chk)[WORLD_HEIGHT]);
};

#endif // CHUNK_HPP
