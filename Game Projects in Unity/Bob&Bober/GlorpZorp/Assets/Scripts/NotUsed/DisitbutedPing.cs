using System.Collections.Generic;
using Unity.Netcode;
using UnityEngine;
using System.Collections;

public class DistributedPing : NetworkBehaviour
{
    [SerializeField]
    private float PingInterval = 1f;
    Dictionary<ulong, float> pingTable = new Dictionary<ulong, float>();

    public float GetOneWayPing(ulong clientId) => pingTable.GetValueOrDefault(clientId, -1f);

    private void Update()
    {
        ulong localID = NetworkManager.Singleton.LocalClientId;
        foreach(var targetID in pingTable.Keys)
        {
            if (targetID == localID)
                Debug.Log($"RTT {localID} -> {targetID}: "+ GetOneWayPing(targetID));
        }
    }

    public override void OnNetworkSpawn()
    {
        if (HasAuthority)
            StartCoroutine(PingRoutine());
    }

    IEnumerator PingRoutine()
    {
        while (IsSpawned && NetworkManager.IsConnectedClient)
        {
            foreach (ulong clientId in NetworkManager.Singleton.ConnectedClientsIds)
            {
                if (clientId == NetworkManager.Singleton.LocalClientId)
                    continue;

                float sentTime = NetworkManager.ServerTime.TimeAsFloat;
                PingRpc(sentTime, RpcTarget.Single(clientId, RpcTargetUse.Temp));
            }
            yield return WaitFor.Seconds(1.0f / PingInterval);
        }
    }

    [Rpc(SendTo.SpecifiedInParams)]
    void PingRpc(float sentTime, RpcParams rpcParams = default)
    {
        pingTable[rpcParams.Receive.SenderClientId] = NetworkManager.ServerTime.TimeAsFloat - sentTime;
        PongRpc(sentTime, RpcTarget.Single(rpcParams.Receive.SenderClientId, RpcTargetUse.Temp));
    }

    [Rpc(SendTo.SpecifiedInParams)]
    void PongRpc(float sentTime, RpcParams rpcParams = default)
    {
        float rtt = NetworkManager.ServerTime.TimeAsFloat - sentTime;
        ulong senderId = rpcParams.Receive.SenderClientId;
    }
}