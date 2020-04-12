#pragma once
#include "CCameraController.h"

namespace Minecraft
{

	class CameraSystem : public System<CameraSystem>
	{
	private:
		Vector3f m_CurrentPos;
		Vector3f m_PrevPos;
		float m_Springiness;

	public:
		CameraSystem(float springiness = 50);
		virtual void Update(EntityManager& entities, TimeDelta dt) override;
	};

}