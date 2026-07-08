using System.Collections;
using Unity.Netcode;
using UnityEngine;

public class CSoul : NetworkBehaviour
{
    private Rigidbody2D m_RigidBody;
    private float m_Speed = 0.0f;
    private Vector2 m_Direction;

    private GameObject m_Face;
    private GameObject m_Light;
    private GameObject m_Dark;

    private ulong m_ShooterClientId;

    //=================================================================================================//

    void Update()
    {
        if (IsClient)
            FixOrientation();
    }

    void FixedUpdate()
    {
        if (IsServer && m_RigidBody != null)
        {
            m_RigidBody.linearVelocity = m_Direction * m_Speed;
        }
    }

    //=================================================================================================//

    public void Shoot(Vector2 direction, Vector2 position, float speed, float lifetime)
    {
        m_Direction = direction;
        transform.position = position;
        m_Speed = speed;

        FixOrientation();
        //Debug.Log($"[CSoul] Shoot called {NetworkManager.Singleton.LocalClientId}");

        if (IsServer)
           StartCoroutine(ServerLifetimeRoutine(lifetime));
    }

  

    public void SetColors(Color light, Color dark)
    {
        m_Light.GetComponent<SpriteRenderer>().color = light;
        m_Dark.GetComponent<SpriteRenderer>().color = dark;
    }

    private void FixOrientation()
    {
        if (m_Direction.x != 0.0f)
        {
            transform.localScale = new Vector3(Mathf.Sign(m_Direction.x), 1.0f, 1.0f);
        }

        if (m_Direction.y != 0.0f)
        {
            float sign = Mathf.Sign(transform.localScale.x) * Mathf.Sign(m_Direction.y);

            transform.eulerAngles = new Vector3(0.0f, 0.0f, sign * 90.0f);
        }
        else
        {
            transform.eulerAngles = new Vector3(0.0f, 0.0f, 0.0f);
        }
    }

    private void UpdateAlpha(GameObject obj, float alpha)
    {
        Color tmp = obj.GetComponent<SpriteRenderer>().color;
        tmp.a = alpha;
        obj.GetComponent<SpriteRenderer>().color = tmp;
    }  

    void OnCollisionEnter2D(Collision2D collision)
    {
        if(!IsServer)
            return;

        // hit an inactive bober
        if (collision.gameObject.CompareTag("Bober"))
        {
            //Debug.Log($"[Server] Soul hit: {collision.gameObject.name} (Tag: {collision.gameObject.tag})");
            CBober boberClass = collision.gameObject.GetComponent<CBober>();
            NetworkObject targetNetObj = collision.gameObject.GetComponent<NetworkObject>();

            if (!boberClass.m_Active.Value && !boberClass.m_IsDead.Value && targetNetObj != null)
            {
                if(NetworkManager.Singleton.ConnectedClients.TryGetValue(m_ShooterClientId, out var clientID))
                {
                    CPlayer shooterPlayer = clientID.PlayerObject.GetComponent<CPlayer>();
                    if (shooterPlayer != null)
                    { 
                        //Debug.Log("[CSoul] posses bober called");
                        shooterPlayer.PossessBoberServerRpc(targetNetObj.NetworkObjectId, clientID.PlayerObject.GetComponent<NetworkObject>().NetworkObjectId);
                    }
                }
                //Debug.Log("[CSould] despawned soul");
                GetComponent<NetworkObject>().Despawn();
                return; 
            }
        }

        // hit spikes or void
        if (collision.gameObject.tag == "Spikes" || collision.gameObject.tag == "Void")
        {
            ReturnToBody();
            return;
        }

        Vector2 orientation = collision.GetContact(0).normal;
        m_Direction = Vector2.Reflect(m_Direction.normalized, orientation);

        m_Direction = new Vector2(Mathf.Round(m_Direction.x), Mathf.Round(m_Direction.y));
        FixOrientation();
        UpdateOrientationClientRpc(m_Direction);
    }
    //=================================================================================================//

    public override void OnNetworkSpawn()
    {
        m_RigidBody = GetComponent<Rigidbody2D>();

        m_Face = transform.Find("Soul_Face")?.gameObject;
        m_Light = transform.Find("Soul_Light")?.gameObject;
        m_Dark = transform.Find("Soul_Dark")?.gameObject;
    }
    IEnumerator ClientFadeOutRoutine(float lifetime)
    {
        for (float t = 0.0f; t < lifetime; t += Time.deltaTime)
        {
            float alpha = 1.0f - t / lifetime;

            UpdateAlpha(m_Face, alpha);
            UpdateAlpha(m_Light, alpha);
            UpdateAlpha(m_Dark, alpha);

            yield return null;
        }
    }


    [Rpc(SendTo.Everyone)]
    public void ShootClientsRpc(Vector2 direction, Vector2 position, float speed, float lifetime)
    {
        //Debug.Log($"[CSoul] ShootClientsRpc called {NetworkManager.Singleton.LocalClientId}");
        Shoot(direction, position, speed, lifetime);
    }

    [Rpc(SendTo.Everyone)]
    public void SetSoulVisualsClientRpc(UnityEngine.Color light, UnityEngine.Color dark, float lifetime)
    {
        //Debug.Log($"[Client] SetSoulVisuals RPC received! Color: {light}, Lifetime: {lifetime}");
        SetColors(light, dark);

        StartCoroutine(ClientFadeOutRoutine(lifetime));
    }

    [Rpc(SendTo.Everyone)]
    private void UpdateOrientationClientRpc(Vector2 newDirection)
    {
        m_Direction = newDirection;
        FixOrientation();
    }

    public void SetupShooter(ulong shooterId)
    {
        m_ShooterClientId = shooterId;
    }

    private IEnumerator ServerLifetimeRoutine(float lifetime)
    {
        yield return new WaitForSeconds(lifetime);
        ReturnToBody();
    }

    private void ReturnToBody()
    {
        if (!IsServer) 
            return;

        // Najdeme CPlayer st�elce
        if (NetworkManager.Singleton.ConnectedClients.TryGetValue(m_ShooterClientId, out var client))
        {
            var playerObj = client.PlayerObject;
            if (playerObj != null && playerObj.TryGetComponent(out CPlayer cPlayer))
            {
                cPlayer.WakeUpClientRpc();
            }
        }

        GetComponent<NetworkObject>().Despawn();
    }

}
