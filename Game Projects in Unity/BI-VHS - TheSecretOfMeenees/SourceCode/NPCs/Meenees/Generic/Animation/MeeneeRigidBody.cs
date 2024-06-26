using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;
using UnityEngine.XR;
//borrowed> https://dotnetfiddle.net/v7ieoM
public class MeeneeRigidBody : MonoBehaviour
{    public enum MeeneeState
    {
        Normal,
        Ragdoll,
        StandUp,
        ResettingBones
    }

    private class BoneTransform
    {
        public Vector3 Position { get; set; }

        public Quaternion Rotation { get; set; }
    }

    public float timeToWakeUp;
    public float timeToResetBones;
    public Transform mainBone;
    public AnimationClip clip;

    private Rigidbody[] _ragdollRigidbodies;
    private Animator _animator;
    private NavMeshAgent _navMeshAgent;
    public MeeneeState _state = MeeneeState.Normal;
    private float _timeToWakeUp;

    private BoneTransform[] _standUpBoneTransforms;
    private BoneTransform[] _ragdollBoneTransforms;
    private Transform[] _bones;
    private float _elapsedResetBonesTime = 0.0f;

    void Awake()
    {
        _ragdollRigidbodies = GetComponentsInChildren<Rigidbody>();
        _animator = GetComponent<Animator>();
        _navMeshAgent = GetComponent<NavMeshAgent>();


        _bones = mainBone.GetComponentsInChildren<Transform>();
        _standUpBoneTransforms = new BoneTransform[_bones.Length];
        _ragdollBoneTransforms = new BoneTransform[_bones.Length];

        for (int boneIndex = 0; boneIndex < _bones.Length; boneIndex++)
        {
            _standUpBoneTransforms[boneIndex] = new BoneTransform();
            _ragdollBoneTransforms[boneIndex] = new BoneTransform();
        }

        PopulateAnimationStartBoneTransforms(_standUpBoneTransforms);
    }

    // Update is called once per frame
    void Update()
    {
        if(_animator.GetBool("FuckingDIE") && _state == MeeneeState.Normal)
        {
            EnableRagdoll();
            _state = MeeneeState.Ragdoll;
        }
        else if(_state == MeeneeState.Ragdoll)
        {
            RagdollBehaviour();
        }
        else if (_state == MeeneeState.ResettingBones)
        {
            ResettingBonesBehaviour();
        }

    }
    private void RagdollBehaviour()
    {
        _timeToWakeUp -= Time.deltaTime;

        if (_timeToWakeUp <= 0)
        {
            AlignRotationTomainBone();
            AlignPositionTomainBone();

            PopulateBoneTransforms(_ragdollBoneTransforms);
            
            _elapsedResetBonesTime = 0.0f;
            _state = MeeneeState.ResettingBones;
        }
    }
    private void DisableRagdoll()
    {
        foreach (var rigidbody in _ragdollRigidbodies)
        {
            rigidbody.isKinematic = true;
        }
        _animator.enabled = true;
        _navMeshAgent.enabled = true;
    }
    private void EnableRagdoll()
    {
        foreach (var rigidbody in _ragdollRigidbodies)
        {
            rigidbody.isKinematic = false;
        }

        _animator.enabled = false;
        _navMeshAgent.enabled = false;
        _timeToWakeUp = timeToWakeUp;
    }

    private void ResettingBonesBehaviour()
    {
        _elapsedResetBonesTime += Time.deltaTime;
        float elapsedPercentage = _elapsedResetBonesTime / timeToResetBones;

        for (int boneIndex = 0; boneIndex < _bones.Length; boneIndex++)
        {
            _bones[boneIndex].localPosition = Vector3.Lerp(
                _ragdollBoneTransforms[boneIndex].Position,
                _standUpBoneTransforms[boneIndex].Position,
                elapsedPercentage);

            _bones[boneIndex].localRotation = Quaternion.Lerp(
                _ragdollBoneTransforms[boneIndex].Rotation,
                _standUpBoneTransforms[boneIndex].Rotation,
                elapsedPercentage);
        }

        if (elapsedPercentage >= 1)
        {
            DisableRagdoll();
            
            _animator.Play(clip.name);
            _state = MeeneeState.StandUp;
        }
    }
    private void AlignRotationTomainBone()
    {
        Vector3 originalHipsPosition = mainBone.position;
        Quaternion originalHipsRotation = mainBone.rotation;

        Vector3 desiredDirection = mainBone.up * -1;
        desiredDirection.y = 0;
        desiredDirection.Normalize();

        Quaternion fromToRotation = Quaternion.FromToRotation(transform.forward, desiredDirection);
        transform.rotation *= fromToRotation;

        mainBone.position = originalHipsPosition;
        mainBone.rotation = originalHipsRotation;
    }
    private void AlignPositionTomainBone()
    {
        Vector3 originalHipsPosition = mainBone.position;
        transform.position = mainBone.position;

        Vector3 positionOffset = _standUpBoneTransforms[0].Position;
        positionOffset.y = 0;
        positionOffset = transform.rotation * positionOffset;
        transform.position -= positionOffset;

        //if (Physics.Raycast(transform.position, Vector3.down, out RaycastHit hitInfo))
        //{
        //    transform.position = new Vector3(transform.position.x, hitInfo.point.y, transform.position.z);
        //}

        mainBone.position = originalHipsPosition;
    }
    private void PopulateBoneTransforms(BoneTransform[] boneTransforms)
    {
        for (int boneIndex = 0; boneIndex < _bones.Length; boneIndex++)
        {
            boneTransforms[boneIndex].Position = _bones[boneIndex].localPosition;
            boneTransforms[boneIndex].Rotation = _bones[boneIndex].localRotation;
        }
    }

    private void PopulateAnimationStartBoneTransforms( BoneTransform[] boneTransforms)
    {
        Vector3 positionBeforeSampling = transform.position;
        Quaternion rotationBeforeSampling = transform.rotation;

        clip.SampleAnimation(gameObject, 0);
        PopulateBoneTransforms(_standUpBoneTransforms);

        transform.position = positionBeforeSampling;
        transform.rotation = rotationBeforeSampling;
    }

    private void StandUpFin()
    {
        _animator.SetBool("FuckingDIE", false);
        _state = MeeneeState.Normal;
    }

}
