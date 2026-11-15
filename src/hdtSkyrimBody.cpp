#include "hdtSkyrimBody.h"
#include "hdtSkyrimSystem.h"

namespace hdt
{
	SkyrimBody::SkyrimBody()
	{
		m_mesh = nullptr;
		m_shared = SharedType::SHARED_PUBLIC;
	}

	SkyrimBody::~SkyrimBody()
	{
	}

	bool SkyrimBody::canCollideWith(const SkinnedMeshBody* rhs) const
	{
		auto body = (SkyrimBody*)rhs;
		if (m_disabled || body->m_disabled)
			return false;

		switch (m_shared) {
		case SharedType::SHARED_PUBLIC:
			break;
		case SharedType::SHARED_INTERNAL:
			if (m_mesh->m_skeleton != body->m_mesh->m_skeleton)
				return false;
			break;
		case SharedType::SHARED_EXTERNAL:
			if (m_mesh->m_skeleton == body->m_mesh->m_skeleton)
				return false;
			break;
		case SharedType::SHARED_PRIVATE:
			if (m_mesh != body->m_mesh)
				return false;
			break;
		default:
			return false;
		}

		return SkinnedMeshBody::canCollideWith(rhs);
	}

	void SkyrimBody::refreshMorphedPositions()
	{
		if (m_origVertexIndex.size() != m_vertices.size())
			return;

		for (auto& src : m_morphSources) {
			auto* shape = src.shape.get();
			if (!shape || !src.vertexCount)
				continue;

			auto& rd = shape->GetDynamicTrishapeRuntimeData();
			RE::BSSpinLockGuard guard(rd.lock);

			const auto* data = static_cast<const uint8_t*>(rd.dynamicData);
			if (!data)
				continue;
			// dynamicData holds one 16-byte entry (x, y, z, pad) per vertex; skip if the buffer is smaller than expected
			if (rd.dataSize && rd.dataSize < src.vertexCount * 16 && rd.dataSize != src.vertexCount)
				continue;

			const uint32_t begin = src.vertexStart;
			const uint32_t end = src.vertexStart + src.vertexCount;
			for (size_t k = 0; k < m_vertices.size(); ++k) {
				const uint32_t o = m_origVertexIndex[k];
				if (o < begin || o >= end)
					continue;
				const auto* p = reinterpret_cast<const RE::NiPoint3*>(data + static_cast<size_t>(o - begin) * 16);
				m_vertices[k].m_skinPos = convertNi(*p);
			}
		}
	}

	void SkyrimBody::internalUpdate()
	{
		if (m_disabled)
			return;
		if (!m_morphSources.empty())
			refreshMorphedPositions();
		SkinnedMeshBody::internalUpdate();
	}
}
