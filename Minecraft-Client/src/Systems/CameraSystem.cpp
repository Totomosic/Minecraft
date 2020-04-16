#include "mcpch.h"
#include "CameraSystem.h"

namespace Minecraft
{

	CameraSystem::CameraSystem(float springiness)
		: m_CurrentPos(), m_PrevPos(), m_Springiness(springiness)
	{
	}

	void CameraSystem::Update(EntityManager& entities, TimeDelta dt)
	{
		static const double SCALE_FACTOR = std::log(0.5);
		Vector3f real = Input::Get().MousePosition();
		double d = 1.0 - std::exp(SCALE_FACTOR * m_Springiness * dt.Seconds());
		m_CurrentPos += (real - m_CurrentPos) * d;
		Vector3f delta = m_CurrentPos - m_PrevPos;
		if (delta.LengthSqrd() >= 0.01f)
		{
			for (EntityHandle entity : entities.GetEntitiesWith<CCameraController, Transform>())
			{
				ComponentHandle transform = entity.GetTransform();
				ComponentHandle controller = entity.GetComponent<CCameraController>();
				transform->Rotate(-delta.x * controller->MouseSensitivity.x, Vector3f::Up(), Space::World);
				transform->Rotate(delta.y * controller->MouseSensitivity.y, Vector3f::Right(), Space::Local);
			}
		}
		m_PrevPos = m_CurrentPos;
	}

}
