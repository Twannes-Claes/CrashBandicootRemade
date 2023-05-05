#include "stdafx.h"
#include "BoneObject.h"

BoneObject::BoneObject(BaseMaterial* pMaterial, const float length)
:m_Length(length),
m_pMat(pMaterial)
{

}

void BoneObject::AddBone(BoneObject* pBone)
{
	//The incoming bone (pBone) must be translated along the X - axis so it is located next to the parent(this) bone.The length of the parent bone is defined by m _Length
	pBone->GetTransform()->Translate(m_Length, 0.0f, 0.0f);

	//Add pBone as a child to the parent BoneObject(this).This creates the hierarchical relation between both bone,pBone is now a child and ‘follows’ the transformation of its parent.
	AddChild(pBone);
}

void BoneObject::CalculateBindPose()
{

	const XMMATRIX inverseMatrix = XMMatrixInverse(nullptr,  XMLoadFloat4x4( &GetTransform()->GetWorld() ));

	XMStoreFloat4x4(&m_BindPose, inverseMatrix);

	for(BoneObject* child : GetChildren<BoneObject>())
	{
		child->CalculateBindPose();
	}

}

void BoneObject::Initialize(const SceneContext&)
{
	//Create an GameObject (pEmpty)
	GameObject* pEmpty = new GameObject();

	//Add pEmpty as a child to the BoneObject
	AddChild(pEmpty);

	// Add a ModelComponent to pEmpty (pModel), use Bone.ovm
	ModelComponent* pModel = pEmpty->AddComponent(new ModelComponent{ L"Meshes/Bone.ovm" });

	// Assign the BoneObject’s material(m_pMaterial) to pModel
	pModel->SetMaterial(m_pMat);

	//Rotate pEmpty - 90 degrees around the Y - axis(This transform will orient the bone along the X - axis)
	//Uniformly scale pEmpty to match the BoneObject’s length(m_Length)
	pEmpty->GetTransform()->Rotate(0.0f, -90.0f, 0.0f);
	pEmpty->GetTransform()->Scale(m_Length);
}
