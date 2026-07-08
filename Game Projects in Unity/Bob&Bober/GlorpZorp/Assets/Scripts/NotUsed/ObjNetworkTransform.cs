using UnityEngine;
using Unity.Netcode;
using Unity.Netcode.Components;

public class ObjNetworkTransform : NetworkTransform
{
    public GameObject ObjVisual;
    [Range(0.001f, 2.0f)] public float VelocityThreshold = 0.1f;
    [SerializeField] DistributedPing ping;

    public float OwnerTimeDelta => ping.GetOneWayPing(OwnerClientId);

    NetworkVariable<Vector3> velocity = new();
    NetworkRigidbody networkRigidbody;

    protected override void Awake()
    {
        base.Awake();
        networkRigidbody = GetComponent<NetworkRigidbody>();
    }

    protected override void OnAuthorityPushTransformState(ref NetworkTransformState networkTransformState)
    {
        var objVelocity = networkRigidbody.GetLinearVelocity();
        if(Mathf.Abs(velocity.Value.magnitude - objVelocity.magnitude) >= VelocityThreshold) 
                        velocity.Value = objVelocity;

        base.OnAuthorityPushTransformState(ref networkTransformState);
    }

    public override void OnFixedUpdate()
    {
        if(IsOwner)
            return;

        var objVelocityDirection = velocity.Value.normalized;
        var objVelocityPredictedMagnitude = velocity.Value.magnitude * OwnerTimeDelta;

        ObjVisual.transform.localPosition = objVelocityDirection * objVelocityPredictedMagnitude;

        base.OnFixedUpdate();
    }
}

