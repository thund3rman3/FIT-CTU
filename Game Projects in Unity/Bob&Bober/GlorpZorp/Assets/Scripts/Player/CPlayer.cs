using System;
using System.Collections;
using System.Collections.Generic;
using System.Drawing;
using Unity.Cinemachine;
using Unity.Netcode;
using UnityEngine;

public class CPlayer : NetworkBehaviour
{
    // general
    [SerializeField] float m_MovementSpeed = 4.0f;
    [SerializeField] float m_SoulSpeed = 8.0f;
    [SerializeField] float m_JumpForce = 9.0f;
    [SerializeField] float m_SoulLifetime = 4.0f;
    [NonSerialized] public bool m_Paused = false;
    
    private CCamera m_CCameraScript = null;

    // bober related
    [SerializeField] GameObject m_ActiveBober = null;
    public CBober m_ActiveBoberClass = null;
    private GameObject m_PreviouslyActiveBober = null;

    // player colors
    UnityEngine.Color m_Color = UnityEngine.Color.HSVToRGB ( 32.0f/360, 0.92f, 0.99f );
    UnityEngine.Color m_ColorDark = UnityEngine.Color.HSVToRGB ( 0.32f/360, 0.92f, 0.79f );

    // soul object and class
    [SerializeField] private GameObject m_Soul;
    private Transform m_SoulTarget;

    // orientation
    private float m_Horizontal;
    private float m_Vertical;

    //=================================================================================================//

    void Awake()
    {
        if (m_ActiveBober)
        {
            ActivateBober(m_ActiveBober);
            transform.position = m_ActiveBober.transform.position;
        }
    }

    void Update()
    {
        if (!IsOwner || !IsSpawned) return;

        if (m_CCameraScript == null && Camera.main != null )
        {
            m_CCameraScript = Camera.main.GetComponent<CCamera>();
        }

        if (m_ActiveBober == null && m_SoulTarget == null)
        {
            return;
        }

        if (m_ActiveBober)
        {
            HandleInput();
        }
        else
        { // follow the position of the soul
            if (m_SoulTarget != null)
                m_CCameraScript.SetPosition(m_SoulTarget.position);
        }
    }

    //=================================================================================================//

    private void HandleInput()
    {
        m_Horizontal = m_Paused ? 0 : Input.GetAxis("Horizontal");
        m_Vertical = Input.GetAxis("Vertical");

        // move player and camera with the bober
        transform.position = m_ActiveBober.transform.position;
        m_ActiveBoberClass.SetHorizontal(m_Horizontal);
        //   Debug.Log($"[Client Update] CPlayer Pos: {transform.position} | Bober Pos: {m_ActiveBober.transform.position}");
        m_SoulTarget = m_ActiveBober.transform;
        m_CCameraScript.SetPosition(transform.position);

        if ( m_Paused )
        {
            return;
        }

        // shoot soul
        if (Input.GetKey(KeyCode.LeftShift) || Input.GetKey(KeyCode.RightShift))
        {
            ShootSoul();
        }
        else
        {
            if (Input.GetKeyDown(KeyCode.Space))
            { // jump
                m_ActiveBoberClass.JumpFromGround();
            }
            else if (Input.GetKeyUp(KeyCode.Space))
            {
                m_ActiveBoberClass.JumpInAir();
            }

            if (Input.GetKeyDown(KeyCode.E))
            { // pressed E to interact with a lever
                m_ActiveBoberClass.ActivateNearbyInteractable();
            }
            if (Input.GetKeyDown(KeyCode.Q))
            { // pressed Q to throw a bomb
                m_ActiveBoberClass.ThrowBombServerRpc(m_ActiveBoberClass.m_IsFacingRight);
            }
        }
    }

    public void ActivateBober ( GameObject bober )
    {
        m_ActiveBober = bober;
        m_ActiveBoberClass = m_ActiveBober.GetComponent<CBober>();

        transform.position = m_ActiveBober.transform.position;
        m_ActiveBoberClass.Activate( m_MovementSpeed, m_JumpForce, m_Color, m_ColorDark );

        m_SoulTarget = m_ActiveBober.transform;
        //Debug.Log($"[CPlayer] Aktivován Bobr: {bober.GetComponent<NetworkObject>().NetworkObjectId}. On: {transform.position}");
    }

    public void DeactivateBober()
    {
        m_PreviouslyActiveBober = m_ActiveBober;
        m_ActiveBoberClass.Deactivate();

        m_ActiveBober = null;
        m_ActiveBoberClass = null;
    }

    public void ReactivateBober()
    {
        if (m_PreviouslyActiveBober == null)
        {
            //Debug.LogWarning("ReactivateBober called but PreviouslyActiveBober is null!");
            return;
        }

        m_ActiveBober = m_PreviouslyActiveBober;
        m_ActiveBoberClass = m_ActiveBober.GetComponent<CBober>();

        transform.position = m_ActiveBober.transform.position;
        m_ActiveBoberClass.Activate(m_MovementSpeed, m_JumpForce, m_Color, m_ColorDark );
        m_SoulTarget = m_ActiveBober.transform;
    }

    private void ShootSoul() 
    {
        Vector2 direction = Vector2.zero;

        if ( m_Horizontal != 0 ) {
            direction = new Vector2 ( Mathf.Sign ( m_Horizontal ), 0.0f );
        }
        else if ( m_Vertical != 0 ) {
            direction = new Vector2 ( 0.0f, Mathf.Sign ( m_Vertical ) );
        }

        if ( direction != Vector2.zero
                && m_ActiveBober != null
                && m_ActiveBoberClass.m_Grounded
                && !m_ActiveBoberClass.m_SoulCooldown
                && !m_ActiveBoberClass.m_IsDead.Value ) {
            m_ActiveBoberClass.m_SoulCooldown = true; // This fixes the issue that non-host player would shoot 10 souls
            ShootSoulServerRpc(direction, transform.position);
        }
    }

    public void SetColor ( UnityEngine.Color color )
    {
        m_Color = color;

        SetDarkColor();

        if ( m_ActiveBoberClass ) {
            m_ActiveBoberClass.UpdateColors ( m_Color, m_ColorDark );  
        }
    }
    
    public UnityEngine.Color GetColor()
    {
        return m_Color;    
    }

    private void SetDarkColor()
    {
        float H, S, V;

        UnityEngine.Color.RGBToHSV ( m_Color, out H, out S, out V );

        m_ColorDark = UnityEngine.Color.HSVToRGB ( H, S, V - 0.2f );
    }

    // Network
    //=================================================================================================//

    public override void OnNetworkSpawn()
    {
        SetDarkColor();

        if (IsOwner)
        {
            if (Camera.main != null)
                m_CCameraScript = Camera.main.GetComponent<CCamera>();

            if (m_CCameraScript == null)
                Debug.LogError("[CPlayer] MainCamera nemá komponentu CCamera!");
        }
    }

    [Rpc(SendTo.Owner)]
    private void DeactivateBoberClientRpc()
    {
        //Debug.Log("[Client] Deactivating Bober client-side.");
        DeactivateBober();
    }

    [Rpc(SendTo.Owner)]
    private void SetCameraTargetToSoulClientRpc(NetworkObjectReference soulRef)
    {
        if (soulRef.TryGet(out NetworkObject soulNetObj))
        {
            m_SoulTarget = soulNetObj.transform;
        }
    }



    [Rpc(SendTo.Server)]
    public void PossessBoberServerRpc(ulong targetId, ulong currentId)
    {
        //Debug.Log($"[Server] Player {OwnerClientId} swap request: bober ID {currentId} -> bober ID {targetId}");

        if (NetworkManager.Singleton.SpawnManager.SpawnedObjects.TryGetValue(targetId, out NetworkObject targetNetObj))
        {
            if (targetNetObj.OwnerClientId != OwnerClientId)
            {
                targetNetObj.ChangeOwnership(OwnerClientId);
                //Debug.Log("Changed ownership");
            }
            SetBoberClientRpc(targetId);
        }
    }

    [Rpc(SendTo.Owner)]
    private void SetBoberClientRpc(ulong boberId)
    {
        if (NetworkManager.Singleton.SpawnManager.SpawnedObjects.TryGetValue(boberId, out NetworkObject targetNetObj))
        {
            if (m_ActiveBober != null) 
                DeactivateBober();

            ActivateBober(targetNetObj.gameObject);

            //Debug.Log($"[Client] Switched to bober ID: {boberId}");
        }
    }



    [Rpc(SendTo.Server)]
    private void ShootSoulServerRpc(Vector2 dir, Vector2 shooterPos)
    {
        // offset so the soul doesn't spawn inside of the bober
        Vector2 position = shooterPos + dir * 0.75f;
        NetworkObject soulNetObj = NetworkManager.Singleton.SpawnManager.InstantiateAndSpawn(
            m_Soul.GetComponent<NetworkObject>(),
            NetworkManager.ServerClientId,
            true, // DestroyWithScene
            false, // IsPlayerObject
            false,
            position,   
            Quaternion.identity
        );
        DeactivateBoberClientRpc();
        if (soulNetObj.TryGetComponent(out CSoul soulScript))
        {
            //Debug.Log($"[Server] Shooting soul from position {position} in direction {dir}.");
            soulScript.ShootClientsRpc(dir, position, m_SoulSpeed, m_SoulLifetime);
            soulScript.SetupShooter(OwnerClientId);
            soulScript.SetSoulVisualsClientRpc(m_Color, m_ColorDark, m_SoulLifetime);
        }
        SetCameraTargetToSoulClientRpc(soulNetObj);
    }

    [Rpc(SendTo.Owner)]
    public void WakeUpClientRpc()
    {
        //Debug.Log("[Client] Duše se vrátila (nedoletìla/náraz), probouzím Bobra.");
        ReactivateBober();
        // Vrátíme kameru zpátky na Bobra
        if (m_ActiveBober != null)
        {
            m_SoulTarget = m_ActiveBober.transform;
        }
    }
}
