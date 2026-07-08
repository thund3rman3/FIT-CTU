using UnityEngine;
using System.Collections.Generic;
using Unity.Netcode;
using UnityUtils;


public class ObjectSpawner : NetworkBehaviour
{
    public GameObject prefab;
    public int cntPrefabs = 10;

    void Start()
    {
        if (!HasAuthority || !NetworkManager.LocalClient.IsSessionOwner)
            return;

        List<Vector3> rndPoints = new List<Vector3>();
        for(int i = 0; i < cntPrefabs; i++)
            rndPoints.Add(Vector3.zero.RandomPointInAnnulus(5, 10));
        
        for(int i = 0; i<cntPrefabs; ++i)
        {
            var instance = Instantiate(prefab);
            var networkObject = instance.GetComponent<NetworkObject>();
            instance.transform.position = rndPoints[i];
            networkObject.Spawn();
        }
    }
}