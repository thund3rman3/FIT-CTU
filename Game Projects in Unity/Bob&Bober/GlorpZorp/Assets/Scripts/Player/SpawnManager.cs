using System.Collections;
using System.Collections.Generic;
using Unity.Netcode;
using UnityEngine;
using UnityEngine.SceneManagement;

public class SpawnManager : NetworkBehaviour
{
    [Header("Spawning Settings")]
    [SerializeField] private GameObject m_BoberPrefab; 
    private GameObject m_MyBoberInstance;

    public override void OnNetworkSpawn()
    {
        if (IsServer)
        {
            NetworkManager.Singleton.SceneManager.OnLoadEventCompleted += OnSceneLoadedServer;
            SpawnMyBober();
        }
    }

    public override void OnNetworkDespawn()
    {
        if (IsServer && NetworkManager.Singleton != null)
        {
            NetworkManager.Singleton.SceneManager.OnLoadEventCompleted -= OnSceneLoadedServer;
        }
    }

    private void OnSceneLoadedServer(string sceneName, LoadSceneMode loadSceneMode, System.Collections.Generic.List<ulong> clientsCompleted, System.Collections.Generic.List<ulong> clientsTimedOut)
    {
        if (clientsCompleted.Contains(OwnerClientId))
        {
            SpawnMyBober();
        }
    }

    private void SpawnMyBober()
    {
        if (m_MyBoberInstance != null) 
            return;

        //Debug.Log($"[Server] Spawnuje se Bober pro hráèe {OwnerClientId}");

        Transform spawnPoint = GameObject.FindGameObjectWithTag("Spawn")?.transform;
        Vector3 pos = (spawnPoint != null) ? spawnPoint.position : Vector3.zero;

        if (spawnPoint == null)
        {
            //Debug.Log("SpawnManager: spawn was not found");
            return;
        }

        pos += new Vector3(Random.Range(0f, 2f), 0, 0);


        NetworkObject boberNetObj = Instantiate(m_BoberPrefab, pos, Quaternion.identity).GetComponent<NetworkObject>();

        boberNetObj.SpawnWithOwnership(OwnerClientId, true);

        m_MyBoberInstance = boberNetObj.gameObject;

        SetupBoberClientRpc(boberNetObj);
    }

    [Rpc(SendTo.Owner)] 
    private void SetupBoberClientRpc(NetworkObjectReference boberRef)
    {
        StartCoroutine(ClientSetupRoutine(boberRef));
    }

    private IEnumerator ClientSetupRoutine(NetworkObjectReference boberRef)
    {
        NetworkObject boberObj = null;
        float timeout = 5f;

        while (timeout > 0)
        {
            if (boberRef.TryGet(out boberObj)) break;
            timeout -= Time.deltaTime;
            yield return null;
        }

        if (boberObj != null)
        {
            //Debug.Log("[Client] Mám Bobra! Nastavuji kameru a ovládání.");

            GetComponent<CPlayer>().ActivateBober(boberObj.gameObject);

            CCamera cam = Camera.main?.GetComponent<CCamera>();
            if (cam != null)
            {
                cam.SetPosition(boberObj.transform.position);
            }

            GameObject.Find("UI")?.SetActive(false);
        }
        else
        {
            Debug.LogError("[Client] Timeout: Bober se nespawnul na klientovi.");
        }
    }
}