#include "stdafx.h"
#include "CameraComponent.h"

#include "Utils/FmodHelper.h"

CameraComponent::CameraComponent() :
	m_FarPlane(2500.0f),
	m_NearPlane(0.1f),
	m_FOV(XM_PIDIV4),
	m_Size(25.0f),
	m_PerspectiveProjection(true)
{
	XMStoreFloat4x4(&m_Projection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_View, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewInverse, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjectionInverse, XMMatrixIdentity());
}

void CameraComponent::Update(const SceneContext& sceneContext)
{
	// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	XMMATRIX projection{};

	if (m_PerspectiveProjection)
	{
		projection = XMMatrixPerspectiveFovLH(m_FOV, sceneContext.aspectRatio, m_NearPlane, m_FarPlane);
	}
	else
	{
		const float viewWidth = (m_Size > 0) ? m_Size * sceneContext.aspectRatio : sceneContext.windowWidth;
		const float viewHeight = (m_Size > 0) ? m_Size : sceneContext.windowHeight;
		projection = XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}

	const XMVECTOR worldPosition = XMLoadFloat3(&GetTransform()->GetWorldPosition());
	const XMVECTOR lookAt = XMLoadFloat3(&GetTransform()->GetForward());
	const XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

	const XMMATRIX view = XMMatrixLookAtLH(worldPosition, worldPosition + lookAt, upVec);
	const XMMATRIX viewInv = XMMatrixInverse(nullptr, view);
	const XMMATRIX viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);

	SetSoundSettings(sceneContext);
}

void CameraComponent::SetSoundSettings(const SceneContext& sceneContext)
{
	//Convert to FMOD UGH
	const auto pCamTransform{ GetTransform() };
	const auto pos = FmodHelper::ToFmod(pCamTransform->GetWorldPosition());
	const auto forward = FmodHelper::ToFmod(pCamTransform->GetForward());
	const auto up = FmodHelper::ToFmod(pCamTransform->GetUp());

	//Calc velocity prev frame
	FMOD_VECTOR vel{};
	const float deltaT = sceneContext.pGameTime->GetElapsed();
	vel.x = (pos.x - m_PrevCamPos.x) / deltaT;
	vel.y = (pos.y - m_PrevCamPos.y) / deltaT;
	vel.z = (pos.z - m_PrevCamPos.z) / deltaT;
	m_PrevCamPos = pos;

	//Set the attributes for the listener
	SoundManager::Get()->GetSystem()->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
}

void CameraComponent::SetActive(bool active)
{
	if (m_IsActive == active) return;

	const auto pGameObject = GetGameObject();
	ASSERT_IF(!pGameObject, L"Failed to set active camera. Parent game object is null");

	if (!pGameObject) return; //help the compiler... (C6011)
	const auto pScene = pGameObject->GetScene();
	ASSERT_IF(!pScene, L"Failed to set active camera. Parent game scene is null");

	m_IsActive = active;
	pScene->SetActiveCamera(active?this:nullptr); //Switch to default camera if active==false
}

GameObject* CameraComponent::Pick(CollisionGroup ignoreGroups) const
{
	TODO_W7(L"Implement Picking Logic")

	const POINT mousePos = InputManager::GetMousePosition();

	PxVec2 ndcPos;

	const float halfWidth  = m_pScene->GetSceneContext().windowWidth / 2;
	const float halfHeight = m_pScene->GetSceneContext().windowHeight / 2;

	ndcPos.x = (mousePos.x - halfWidth) / halfWidth;
	ndcPos.y = (halfHeight - mousePos.y) / halfHeight;

	// Retrieve the inverse of the viexprojection matrix
	const auto& inverseCache{ XMLoadFloat4x4(&GetViewProjectionInverse()) };

	// Calculate the ray start and end coordinates
	const XMVECTOR rayStart{ XMVector3TransformCoord(XMVECTOR{ ndcPos.x, ndcPos.y, 0.0f }, inverseCache) };
	const XMVECTOR rayEnd{ XMVector3TransformCoord(XMVECTOR{ ndcPos.x, ndcPos.y, 1.0f }, inverseCache) };

	// Store the XMVECTOR ray data in XMFLOAT3 variables
	XMFLOAT3 nearPoint{};
	XMStoreFloat3(&nearPoint, rayStart);
	XMFLOAT3 farPoint{};
	XMStoreFloat3(&farPoint, rayEnd);

	const PxVec3 startPoint = {nearPoint.x, nearPoint.y, nearPoint.z};
	const PxVec3 dir = { farPoint.x - startPoint.x, farPoint.y - startPoint.y, farPoint.z - startPoint.z };

	PxRaycastBuffer	hit{};

	PxQueryFilterData filterData{};

	filterData.data.word0 = ~static_cast<UINT>(ignoreGroups);

	if(m_pScene->GetPhysxProxy()->Raycast(startPoint, dir.getNormalized(), PX_MAX_F32, hit, PxHitFlag::eDEFAULT, filterData))
	{
		const BaseComponent* pComponent = static_cast<BaseComponent*>(hit.block.actor->userData);

		return pComponent->GetGameObject();
	}

	return nullptr;
}