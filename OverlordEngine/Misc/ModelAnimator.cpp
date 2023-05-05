#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
	m_pMeshFilter{pMeshFilter}
{
	SetAnimation(0);
}

void ModelAnimator::Update(const SceneContext& sceneContext)
{
	//TODO_W6_();

	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		auto passedTicks = sceneContext.pGameTime->GetElapsed() * m_CurrentClip.ticksPerSecond * m_AnimationSpeed;
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)

		passedTicks = fmod(passedTicks, m_CurrentClip.duration);

		//2. 
		//IF m_Reversed is true
		if(m_Reversed)
		{
			//Subtract passedTicks from m_TickCount
			m_TickCount -= passedTicks;
			//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
			if(m_TickCount < 0)
			{
				m_TickCount += m_CurrentClip.duration;
			}
		}
		//ELSE
		else
		{
			//	Add passedTicks to m_TickCount
			m_TickCount += passedTicks;
			//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
			if(m_TickCount > m_CurrentClip.duration)
			{
				m_TickCount -= m_CurrentClip.duration;
			}
		}

		//3.
		//Find the enclosing keys
		AnimationKey keyA = m_CurrentClip.keys.front();
		AnimationKey keyB = m_CurrentClip.keys.back();

		//Iterate all the keys of the clip and find the following keys:

		//keyA > Closest Key with Tick before/smaller than m_TickCount
		//keyB > Closest Key with Tick after/bigger than m_TickCount
		for (const auto key : m_CurrentClip.keys)
		{
			if (key.tick <= m_TickCount)
			{
				keyA = key;
			}
			else
			{
				keyB = key;
				break;
			}
		}


		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)

		const float blendFactor = (m_TickCount - keyA.tick) / (keyB.tick - keyA.tick);

		//FOR every boneTransform in a key (So for every bone)

		for(int i{}; i < m_pMeshFilter->m_BoneCount; ++i)
		{
		//	Retrieve the transform from keyA (transformA)
			const auto& transformA = keyA.boneTransforms[i];
			
		// 	Retrieve the transform from keyB (transformB)
			const auto& transformB = keyB.boneTransforms[i];

		//	Decompose both transforms

			XMVECTOR scaleA;
			XMVECTOR rotA;
			XMVECTOR transA;

			XMMatrixDecompose(&scaleA, &rotA, &transA, XMLoadFloat4x4(&transformA));

			XMVECTOR scaleB;
			XMVECTOR rotB;
			XMVECTOR transB;

			XMMatrixDecompose(&scaleB, &rotB, &transB, XMLoadFloat4x4(&transformB));


		//	Lerp between all the transformations (Position, Scale, Rotation)

			const XMVECTOR lerpedScale{ XMVectorLerp(scaleA, scaleB, blendFactor) };
			const XMVECTOR lerpedRot{ XMQuaternionSlerp(rotA, rotB, blendFactor) };
			const XMVECTOR lerpedTrans{ XMVectorLerp(transA, transB, blendFactor) };

		//	Compose a transformation matrix with the lerp-results

			const XMMATRIX finalTransform{ XMMatrixScalingFromVector(lerpedScale) * XMMatrixRotationQuaternion(lerpedRot) * XMMatrixTranslationFromVector(lerpedTrans) };

		//	Add the resulting matrix to the m_Transforms vector

			XMStoreFloat4x4(&m_Transforms[i], finalTransform);
		}

	}
}

void ModelAnimator::SetAnimation(const std::wstring& clipName)
{
	//TODO_W6_()
	//Set m_ClipSet to false
	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	//If found,
	//	Call SetAnimation(Animation Clip) with the found clip
	//Else
	//	Call Reset
	//	Log a warning with an appropriate message

	m_ClipSet = false;

	for (auto clip : m_pMeshFilter->GetAnimationClips())
	{

		if(clip.name == clipName)
		{
			SetAnimation(clipName);
		}
		else
		{
			Reset();
			Logger::LogWarning(L"No right clip");
		}

	}
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	//TODO_W6_()
	//	//Set m_ClipSet to false
	//	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	//	//If not,
	//		//	Call Reset
	//		//	Log a warning with an appropriate message
	//		//	return
	//	//else
	//		//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
	//		//	Call SetAnimation(AnimationClip clip)

	m_ClipSet = false;

	if(clipNumber < m_pMeshFilter->GetAnimationClips().size())
	{
		const auto clip = m_pMeshFilter->GetAnimationClips()[clipNumber];

		SetAnimation(clip);
	}
}

void ModelAnimator::SetAnimation(const AnimationClip& clip)
{
	//TODO_W6_()
	////Set m_ClipSet to true
	////Set m_CurrentClip
	//
	////Call Reset(false)

	m_ClipSet = true;

	m_CurrentClip = clip;

	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	//TODO_W6_()
	////If pause is true, set m_IsPlaying to false
	//
	////Set m_TickCount to zero
	////Set m_AnimationSpeed to 1.0f
	//
	////If m_ClipSet is true
	////	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
	////	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
	////Else
	////	Create an IdentityMatrix 
	////	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)
	///

	if (pause == true)  m_IsPlaying = false;

	m_TickCount = 0;
	m_AnimationSpeed = 1.f;

	if(m_ClipSet)
	{
		auto transforms = m_CurrentClip.keys[0].boneTransforms;

		m_Transforms.assign(transforms.begin(), transforms.end());
	}
	else
	{
		const XMMATRIX identiy = XMMatrixIdentity();

		XMFLOAT4X4 temp{};

		XMStoreFloat4x4(&temp, identiy);

		m_Transforms.assign(m_pMeshFilter->m_BoneCount, temp);
	}

}
