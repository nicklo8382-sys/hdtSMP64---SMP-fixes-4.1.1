#pragma once

#include "hdtConvertNi.h"
#include "hdtSkinnedMesh/hdtSkinnedMeshBody.h"
#include "hdtSkyrimBone.h"

namespace hdt
{
	class SkyrimSystem;

	class SkyrimBody : public SkinnedMeshBody
	{
	public:
		SkyrimBody();
		~SkyrimBody();

		enum class SharedType
		{
			SHARED_PUBLIC,
			SHARED_INTERNAL,
			SHARED_EXTERNAL,
			SHARED_PRIVATE,
		};

		SkyrimSystem* m_mesh;
		SharedType m_shared;
		bool m_disabled = false;
		int m_disablePriority = 0;
		RE::BSFixedString m_disableTag;

		bool canCollideWith(const SkinnedMeshBody* body) const override;
		void internalUpdate() override;

		// [FacialMorphCollision] Face geometry (BSDynamicTriShape) gets its vertices rewritten by the game every time
		// facial morphs change (expressions, phonemes, MFEE, PPA...). Stock FSMP only reads those positions once when the
		// system is built, so collisions use the neutral face. We keep a reference to the dynamic shape and re-read the
		// morphed positions before every skinning update.
		struct MorphSource
		{
			RE::NiPointer<RE::BSDynamicTriShape> shape;
			uint32_t vertexStart = 0;  // first vertex of this mesh in the (pre-compaction) body vertex list
			uint32_t vertexCount = 0;
		};
		std::vector<MorphSource> m_morphSources;
		void refreshMorphedPositions();
	};
}
