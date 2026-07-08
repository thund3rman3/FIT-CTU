using Unity.Netcode;
using UnityEngine;
using UnityEngine.InputSystem;


/// <summary>
/// Spawns <see cref="PrefabToSpawn"/> GameObject when the Space Bar is pressed.
/// If you want to modify this Script please copy it into your own project and add it to your Player Prefab.
/// </summary>
public class Spawner : NetworkBehaviour
{
    /// <summary>
    /// Prefab that will get spawned.
    /// </summary>
    public GameObject PrefabToSpawn;

    void Update()
    {
        if (!IsSpawned || !HasAuthority)
        {
            return;
        }

        if (Keyboard.current.spaceKey.wasReleasedThisFrame)
        {
            SpawnSphere();
        }
    }

    void SpawnSphere()
    {
        var instance = Instantiate(PrefabToSpawn);
        instance.transform.position = transform.position;
        var instanceNetworkObject = instance.GetComponent<NetworkObject>();
        instanceNetworkObject.Spawn();
    }
}
