using UnityEngine;
using Unity.Netcode;

public class OwnershipPickup : NetworkBehaviour
{
    [SerializeField] private float pickupRadius = 2f;
    [SerializeField] private string pickupTag = "Pickup";

    private void Update()
    {
        if(!HasAuthority || !IsSpawned)
            return;

        var nearbyColliders = Physics.OverlapSphere(transform.position, pickupRadius);
        foreach (Collider collider in nearbyColliders)
        {
            if(!collider.CompareTag(pickupTag))
                continue;

            NetworkObject networkObject = collider.GetComponent<NetworkObject>();
            if(networkObject == null || !networkObject.IsSpawned)
                continue;

            if (!networkObject.IsOwner)
            {
                Debug.Log($"Transferring ownership of {networkObject.name} to {NetworkManager.LocalClientId}");
                networkObject.ChangeOwnership(NetworkManager.LocalClientId);
            }
        }
    }

    private void OnDrawGizmos()
    {
        Gizmos.color = Color.yellow;
        Gizmos.DrawWireSphere(transform.position, pickupRadius);
    }
}
