using System;
using System.Collections;
using System.Collections.Generic;

using Unity.Netcode;
using Unity.Netcode.Components;
using UnityEngine;

// Struct for bober client prediction
public struct BoberState : INetworkSerializable
{
    public Vector2 m_Position;
    public Vector2 m_Velocity;
    public float m_Timestamp;
    public bool m_IsFacingRight;

    public void NetworkSerialize<T>(BufferSerializer<T> serializer) where T : IReaderWriter
    {
        serializer.SerializeValue(ref m_Position);
        serializer.SerializeValue(ref m_Velocity);
        serializer.SerializeValue(ref m_Timestamp);
        serializer.SerializeValue(ref m_IsFacingRight);
    }
}

public class CBober : NetworkBehaviour
{
    [SerializeField] private Rigidbody2D m_RigidBody;
    [SerializeField] private CRigidBodyHandler m_RigidBodyHandler;
    [SerializeField] private SpriteRenderer m_SpriteRenderer;
    private Collider2D m_Collider;
    [SerializeField] private AudioClip m_DeathClip;
    public NetworkVariable<bool> m_Active = new NetworkVariable<bool>(
        false,
        NetworkVariableReadPermission.Everyone,
        NetworkVariableWritePermission.Server
    );

    // movement
    private float m_MovementSpeed = 0.0f;
    private float m_JumpForce = 0.0f;
    [SerializeField] public bool m_IsFacingRight = true;
    [System.NonSerialized] public bool m_Grounded = false;
    [SerializeField] private LayerMask m_GroundLayer;

    // Soul
    [System.NonSerialized] public bool m_SoulCooldown = false;
    private float m_SoulCooldownSeconds = 1.0f;

    // interaction
    private float m_OnePixel = 1.0f / 12.0f;
    private Vector2 m_BombPositionOffset = new Vector3 ( 0.3f, 0.0f, 0.0f );

    // nearby interactables
    private CLever m_NearbyLeverClass = null;
    private GameObject m_NearbyPressurePlate = null;

    // nearby bomb
    private GameObject m_NearbyBomb = null;
    public NetworkVariable<bool> m_HoldingBomb = new NetworkVariable<bool>(
        false,
        NetworkVariableReadPermission.Everyone,
        NetworkVariableWritePermission.Server
    );
    private GameObject m_NearbyBombCrate = null;

    // orientation (left or right)
    private float m_Horizontal;

    // list to keep track of collisions with ground objects
    [NonSerialized] public List <GameObject> m_GroundCollisions = new List <GameObject> ();

    // death stuff
    private List <Vector2> m_SafePositions = new List <Vector2> (); // list to keep track of safe positions for respawning
    private int m_SafePositionsSize = 20; // how many steps back will the bober respawn (50 fps FixedUpdate)
    private int m_UnsafeGroundCollisions = 0;
    private bool m_IsInSafety = false;
    private int m_WaterCollisions = 0;
    private float m_DeathAnimationLength = 1.0f;
    public NetworkVariable<bool> m_IsDead = new NetworkVariable<bool>(
        false,
        NetworkVariableReadPermission.Everyone,
        NetworkVariableWritePermission.Server
    );

    // Arrow
    private GameObject m_Arrow;
    private SpriteRenderer m_ArrowLightRenderer;
    private SpriteRenderer m_ArrowDarkRenderer;

    // Animation
    private CLoopAnimation m_WalkingAnimation;
    [SerializeField] private Sprite m_FallingSprite;
    private Sprite m_IdleSprite;

    // Extrapolation
    private NetworkVariable<BoberState> m_NetState = new NetworkVariable<BoberState>(
        writePerm: NetworkVariableWritePermission.Owner
    );
    private float m_LastRpcTime = 0.0f;
    private Vector2 m_ExtrapolatedPosition;
    //=================================================================================================//

    void Awake()
    {
        // set arrow related variables
        m_Arrow = transform.Find("Arrow").gameObject;
        m_ArrowLightRenderer = m_Arrow.transform.Find("Arrow_Light").gameObject.GetComponent<SpriteRenderer>();
        m_ArrowDarkRenderer = m_Arrow.transform.Find("Arrow_Dark").gameObject.GetComponent<SpriteRenderer>();

        // animation stuff
        m_IdleSprite = transform.GetComponent<SpriteRenderer>().sprite;
        m_WalkingAnimation = transform.GetComponent<CLoopAnimation>();

        m_Arrow.SetActive(false);
    }

    void Update()
    {
        if (m_Active.Value)
        {
            // check ground collisions
            if (m_GroundCollisions.Count != 0)
            { // on ground
                m_Grounded = true;
            }
            else
            { // falling or mid-jump
                m_Grounded = false;
            }

            if (IsOwner)
                CheckNearbyPressurePlate();
            else
            {
                CalculateExtrapolation();
                bool serverFacingRight = m_NetState.Value.m_IsFacingRight;

                // Pokud se liší od toho, co máme teď, otočíme ho
                if (m_IsFacingRight != serverFacingRight)
                {
                    m_IsFacingRight = serverFacingRight;
                    UpdateVisualRotation();
                }
            }

            if (m_HoldingBomb.Value)
            {
                Vector3 bombOffset = (m_IsFacingRight ? 1.0f : -1.0f) * m_BombPositionOffset;
                m_NearbyBomb.transform.position = transform.position + bombOffset;
                m_NearbyBomb.transform.rotation = Quaternion.identity;
            }
        }
    }

    void FixedUpdate()
    {
        if (m_Active.Value && !m_IsDead.Value)
        {
            if (IsOwner)
            {
                HandleAnimations(m_Horizontal);
                float targetSpeed = m_Horizontal * m_MovementSpeed;

                m_RigidBodyHandler.AddForce(new Vector2(targetSpeed, 0));

                m_NetState.Value = new BoberState()
                {
                    m_Position = m_RigidBody.position,
                    m_Velocity = m_RigidBody.linearVelocity,
                    m_Timestamp = (float)NetworkManager.Singleton.ServerTime.Time,
                    m_IsFacingRight = m_IsFacingRight
                };
                AddSafePosition();
            }
        }
        else if (IsOwner) // sometimes an inactive bober was moving slightly   
            m_RigidBody.linearVelocity = new Vector2(0.0f, 0.0f);

        // die if is in water and is not in safety
        if (IsOwner && m_WaterCollisions != 0 && !m_IsInSafety && !m_IsDead.Value)
        {
            //Debug.Log($"[Bober] Drowning!");
            DieServerRpc(m_IsInSafety);
        }
    }

    public void Activate(float movementSpeed, float jumpForce, Color light, Color dark )
    {
        if (!IsOwner)
        {
            //Debug.LogWarning($"[CBober] Pokus o aktivaci, ale nejsem Owner! OwnerId: {OwnerClientId}, LocalId: {NetworkManager.Singleton.LocalClientId}");
            return;
        }

        m_SoulCooldown = true;
        m_MovementSpeed = movementSpeed;
        m_JumpForce = jumpForce;
        m_RigidBody.bodyType = RigidbodyType2D.Dynamic;
        UpdateColors ( light, dark );
        m_Arrow.SetActive ( true );
        FillOutSafePositions();
 
        StartCoroutine(SoulCooldownRoutine());

        m_IsFacingRight = transform.localScale.x < 0 ? false : true;

        ActivateServerRpc ( light, dark );
    }

    [Rpc(SendTo.Server)]
    private void ActivateServerRpc ( Color light, Color dark )
    {
        if (!IsOwner)
        {
            m_RigidBody.bodyType = RigidbodyType2D.Kinematic;
            m_RigidBody.linearVelocity = Vector2.zero;
        }

        m_Active.Value = true;
    }

    public void Deactivate()
    {
        if (!IsOwner) 
            return;

        m_MovementSpeed = 0.0f;
        m_JumpForce = 0.0f;
        m_Arrow.SetActive ( false );

        // stop moving
        m_WalkingAnimation.m_Animate = false;
        m_SpriteRenderer.sprite = m_IdleSprite;

        // move bober upwards (plate will reset)
        if ( m_NearbyPressurePlate ) {
            float time = m_NearbyPressurePlate.GetComponent<CPressurePlate>().m_DeactivationTime;
            StartCoroutine(MoveUpAfterRoutine ( time, m_OnePixel ));
        }
        else { // set the bodyType to kinematic right away and stop motion
            m_RigidBody.linearVelocity = Vector2.zero; 
        }

        DeactivateServerRpc();
    }

    [Rpc(SendTo.Server)]
    private void DeactivateServerRpc()
    {
        m_Active.Value = false;
        m_RigidBody.linearVelocity = Vector2.zero;
    }

    // Network
    //=================================================================================================//
    public override void OnNetworkSpawn()
    {
        if (!m_IsFacingRight)
            transform.rotation = Quaternion.Euler(0, 180, 0);

        FillOutSafePositions();

        if (m_SpriteRenderer == null)
            m_SpriteRenderer = GetComponent<SpriteRenderer>();

        m_Collider = GetComponent<Collider2D>();

        if (!IsOwner)
        {
            m_ExtrapolatedPosition = transform.position;
        }

        m_HoldingBomb.OnValueChanged += OnHoldingBombStateChanged;
    }

    public override void OnNetworkDespawn()
    {
        m_HoldingBomb.OnValueChanged -= OnHoldingBombStateChanged;
    }

    public void ActivateNearbyInteractable()
    {
        if (!IsOwner) return;

        if (m_NearbyBomb)
        {
            NetworkObject netObj = m_NearbyBomb.GetComponent<NetworkObject>();
            if (netObj != null)
                InteractNearbyObjectServerRpc(netObj);
        }
        else if (m_NearbyBombCrate)
        {
            NetworkObject netObj = m_NearbyBombCrate.GetComponent<NetworkObject>();
            if (netObj != null)
                InteractNearbyObjectServerRpc(netObj);
        }
        else if (m_NearbyLeverClass)
        {
            NetworkObject netObj = m_NearbyLeverClass.GetComponent<NetworkObject>();
            if (netObj != null)
                InteractNearbyObjectServerRpc(netObj);
        }
    }
    [Rpc(SendTo.Server)]
    private void InteractNearbyObjectServerRpc(NetworkObjectReference targetRef)
    {
        if (targetRef.TryGet(out NetworkObject targetObject))
        {
            GameObject targetGO = targetObject.gameObject;

            if (targetGO.GetComponent<CBomb>())
            {
                m_NearbyBomb = targetGO;
                m_HoldingBomb.Value = true;
                targetGO.GetComponent<CBomb>().ResetRotation();
            }
            else if (targetGO.GetComponent<CBombSpawner>())
            {
                targetGO.GetComponent<CBombSpawner>().SpawnBomb();
            }
            else if (targetGO.GetComponent<CLever>())
            {
                targetGO.GetComponent<CLever>().Toggle();
            }
        }
    }

    [Rpc(SendTo.Server)]
    public void ThrowBombServerRpc(bool facingRight)
    {
        if (m_HoldingBomb.Value)
        {
            m_HoldingBomb.Value = false;
            m_NearbyBomb.GetComponent<CBomb>().Throw(facingRight);
        }
    }

    [Rpc(SendTo.Server)]
    public void DropBombServerRpc()
    {
        if (m_HoldingBomb.Value)
        {
            m_HoldingBomb.Value = false;
            m_NearbyBomb.GetComponent<CBomb>().Drop();
        }
    }

    private void OnHoldingBombStateChanged(bool previousValue, bool isHolding)
    {
        if (m_NearbyBomb != null)
        {
            var netTransform = m_NearbyBomb.GetComponent<NetworkTransform>();
            if (netTransform != null)
            {
                netTransform.enabled = !isHolding;
            }
        }
    }

    [Rpc(SendTo.Server)]
    void DieServerRpc(bool m_IsInSafety)
    {
        DropBombServerRpc();

        // "is not shielded from dying" or already dead
        if ( !m_IsInSafety && !m_IsDead.Value ) {
            m_IsDead.Value = true;
            m_HoldingBomb.Value = false;
            DieClientRpc();
        }
    }

    [Rpc(SendTo.ClientsAndHost)]
    private void DieClientRpc()
    {
        GetComponent<AudioSource>().PlayOneShot(m_DeathClip);
        StartCoroutine(DeathAnimationRoutine());
        if (IsOwner && !m_Active.Value)
        {
            // respawn inactive bober in the same position
            FillOutSafePositions();
            m_RigidBody.bodyType = RigidbodyType2D.Kinematic;
        }
    }

    IEnumerator DeathAnimationRoutine()
    {
        // change color over time
        for (float t = 0.0f; t < m_DeathAnimationLength; t += Time.deltaTime) {
            float saturation = t / m_DeathAnimationLength;

            UnityEngine.Color color = m_SpriteRenderer.color;
            float H, S, V;

            UnityEngine.Color.RGBToHSV(color, out H, out S, out V);
            S = saturation;
            m_SpriteRenderer.color = UnityEngine.Color.HSVToRGB(H, S, V);

            yield return null;
        }

        m_SpriteRenderer.color = new UnityEngine.Color(1.0f, 1.0f, 1.0f);

        if (IsOwner)
            RespawnServerRpc(m_SafePositions[0]);

    } 
    
    [Rpc(SendTo.Server)]
    void RespawnServerRpc(Vector2 pos)
    {
        m_WaterCollisions = 0;
        m_IsDead.Value = false;
        
        RespawnClientRpc(pos);
    }

    [Rpc(SendTo.ClientsAndHost)]
    void RespawnClientRpc(Vector2 pos)
    {
        if ( IsOwner ) {
            m_WaterCollisions = 0;
            m_RigidBody.bodyType = RigidbodyType2D.Dynamic;
        }

        transform.position = pos;
    }

    // Collision handling and gameplay logic
    //=================================================================================================//

    public void SetHorizontal(float horizontal)
    {
        m_Horizontal = horizontal;
    }

    public void JumpFromGround()
    {
        if (IsOwner && m_Grounded && !m_IsDead.Value)
        {
            m_RigidBodyHandler.SetLinearVelocity(new Vector2(m_RigidBody.linearVelocity.x, m_JumpForce));
            m_Grounded = false;
        }
    }

    public void JumpInAir()
    {
        if (IsOwner && m_RigidBody.linearVelocity.y > 0)
        {
            m_RigidBodyHandler.SetLinearVelocity(new Vector2(m_RigidBody.linearVelocity.x, m_RigidBody.linearVelocity.y * 0.5f));
        }
    }


    private void HandleAnimations(float horizontalInput)
    {
        if (horizontalInput > 0) m_IsFacingRight = true;
        else if (horizontalInput < 0) m_IsFacingRight = false;

        // flip sprite
        UpdateVisualRotation();

        // select current animation
        float horizontal = m_RigidBody.linearVelocity.x;
        float vertical = m_RigidBody.linearVelocity.y;

        // walking
        if (Math.Abs(horizontal) >= 1.0f)
        {
            // and falling
            if (vertical <= -1.0f)
            {
                //determine which one should be applied
                if (Math.Abs(horizontal) > Math.Abs(vertical))
                {
                    m_WalkingAnimation.m_Animate = true;
                }
                else
                {
                    m_WalkingAnimation.m_Animate = false;
                    m_SpriteRenderer.sprite = m_FallingSprite;
                }
            }
            else
            {
                m_WalkingAnimation.m_Animate = true;
            }
        }
        else if (vertical <= -1.0f)
        { // only falling
            m_WalkingAnimation.m_Animate = false;
            m_SpriteRenderer.sprite = m_FallingSprite;
        }
        else
        { // not moving - idle
            m_WalkingAnimation.m_Animate = false;
            m_SpriteRenderer.sprite = m_IdleSprite;
        }
    }

    private void UpdateVisualRotation()
    {
        if (m_IsFacingRight)
            transform.rotation = Quaternion.Euler(0, 0, 0);
        else
            transform.rotation = Quaternion.Euler(0, 180, 0);
    }

    public void UpdateColors(Color light, Color dark)
    {
        m_ArrowLightRenderer.color = light;
        m_ArrowDarkRenderer.color = dark;
    }

    public void CheckNearbyPressurePlate()
    {
        if (m_NearbyPressurePlate)
        {
            CPressurePlate PPclass = m_NearbyPressurePlate.GetComponent<CPressurePlate>();
            if (!PPclass.m_IsPressed.Value)
            {
                PPclass.PressDownServerRpc();
                m_LastRpcTime = Time.time;
            }
            else if (Time.time > m_LastRpcTime + 0.1f) // RPC call reduction to 0.1s
            {
                PPclass.ResetTimerServerRpc();
                m_LastRpcTime = Time.time;
            }
        }
    }

    IEnumerator SoulCooldownRoutine()
    {
        yield return new WaitForSeconds(m_SoulCooldownSeconds);

        m_SoulCooldown = false;
    }

    IEnumerator MoveUpAfterRoutine(float time, float distanceY)
    {
        m_RigidBody.bodyType = RigidbodyType2D.Kinematic;

        yield return new WaitForSeconds(time);

        transform.position += new Vector3(0.0f, distanceY, 0.0f);
    }


    void AddSafePosition()
    {
        // if the player is in a spot where it's safe to respawn
        if (m_UnsafeGroundCollisions == 0
            && m_Grounded
            && m_WaterCollisions == 0
            && (m_HoldingBomb.Value ? !m_NearbyBomb.GetComponent<CBomb>().m_IsActive.Value : true))
        {

            m_SafePositions.Add(transform.position); // adds to the end of the list
            m_SafePositions.RemoveAt(0); // pop front
        }
    }

    void FillOutSafePositions()
    {
        for (int i = 0; i < m_SafePositionsSize; i++)
        {
            m_SafePositions.Add(transform.position);
        }
    }

    private void CalculateExtrapolation()
    {
        BoberState state = m_NetState.Value;
        if (state.m_Timestamp == 0) 
            return;

        // 1. Target position based on last known position, velocity and time since last update
        float timeSinceUpdate = (float)NetworkManager.Singleton.ServerTime.Time - state.m_Timestamp;
        timeSinceUpdate = Mathf.Clamp(timeSinceUpdate, 0f, 0.25f);

        Vector2 targetPos = state.m_Position + (state.m_Velocity * timeSinceUpdate);

        // Gravity
        if (Mathf.Abs(state.m_Velocity.y) > 0.1f)
        {
            Vector2 gravity = Physics2D.gravity * m_RigidBody.gravityScale;
            targetPos += (0.5f * gravity * timeSinceUpdate * timeSinceUpdate);
        }

        // 2. If the target position is below the current position
        if (targetPos.y < state.m_Position.y || state.m_Velocity.y < 0)
        {
            Vector2 rayOrigin = targetPos;
            rayOrigin.y += 0.5f; // offset

            RaycastHit2D hit = Physics2D.Raycast(
                rayOrigin,
                Vector2.down,
                2.0f, // ray length
                m_GroundLayer
            );

            if (hit.collider != null)
            {
                // hit.point is the point where the raycast hit the ground
                float feetOffset = m_Collider.bounds.extents.y;
                float groundY = hit.point.y + feetOffset; // where the player should stand

                // if the extrapolated position is below the ground, snap it to the ground
                if (targetPos.y < groundY)
                {
                    targetPos.y = groundY;
                }
            }
        }

        // 3. Reconcile with current position
        float distance = Vector2.Distance(transform.position, targetPos);

        // Zone 1: TELEPORT
        if (distance > 3.0f)
        {
            transform.position = targetPos;
            m_ExtrapolatedPosition = targetPos;
            return;
        }

        // Zone 2: LERP
        if (distance > 0.5f)
        {
            transform.position = Vector2.Lerp(transform.position, targetPos, Time.deltaTime * 10f);
            m_ExtrapolatedPosition = targetPos;
            return;
        }

        // Zone 3: Low latency extrapolation with collision detection
        Vector2 finalPos = targetPos;
        Vector2 moveVector = targetPos - (Vector2)transform.position;
        float moveDist = moveVector.magnitude;

        if (moveDist > 0.001f)
        {
            RaycastHit2D hit = Physics2D.BoxCast(
                transform.position,
                m_Collider.bounds.size * 0.85f,
                0f,
                moveVector.normalized,
                moveDist,
                m_GroundLayer
            );

            if (hit.collider != null && hit.normal.y < 0.3f) // ignore slopes
            {
                finalPos = (Vector2)transform.position + (moveVector.normalized * Mathf.Max(0, hit.distance - 0.01f));
            }
        }

        m_ExtrapolatedPosition = finalPos;
        transform.position = Vector2.Lerp(transform.position, m_ExtrapolatedPosition, Time.deltaTime * 20f);
    }

 

    void OnCollisionEnter2D ( Collision2D collision )
    {
        if (!IsOwner)
          return; 
            
        switch ( collision.gameObject.tag ) {
            
            case "Unsafe Ground":
                m_UnsafeGroundCollisions++;
                break;
            
            case "Void":
                DieServerRpc(m_IsInSafety);
                break;
            
            case "Spikes":
                DieServerRpc(m_IsInSafety);
                break;
        }

        Vector2 direction = collision.GetContact ( 0 ).normal;

        // collision at the bottom
        if ( direction.y >= 0.7 ) {
            m_GroundCollisions.Add ( collision.gameObject );
            m_Grounded = true;
        }
    }

    void OnCollisionExit2D ( Collision2D collision ) 
    {
        switch ( collision.gameObject.tag ) {
            

            case "Unsafe Ground":
                m_UnsafeGroundCollisions--;
                break;
        }

        if ( m_GroundCollisions.Contains ( collision.gameObject ) ) { 
            m_GroundCollisions.Remove ( collision.gameObject );
            if (m_GroundCollisions.Count == 0) 
                m_Grounded = false;
        }
    }

    void OnTriggerEnter2D ( Collider2D collision )
    {
        //Debug.Log($"[Bober] Trigger s: {collision.gameObject.name} (Tag: {collision.gameObject.tag})");
        switch ( collision.gameObject.tag ) {
            case "Lever":
                m_NearbyLeverClass = collision.gameObject.GetComponent<CLever>();
                break;
            case "Pressure plate":
                // the collider is a child of the actual pressure plate
                //Debug.Log($"[Bober] Near pressure plate: {collision.gameObject.name}");
                m_NearbyPressurePlate = collision.gameObject;
                break;

            case "Bomb":
                // interaction box of the bomb (the trigger is on the child of the actual bomb object)
                if ( collision is BoxCollider2D && !m_NearbyBomb ) {
                    m_NearbyBomb = collision.gameObject.transform.parent.gameObject;
                }
                // interaction circle of the bomb explosion
                else if ( collision is CircleCollider2D ) {
                    DieServerRpc(m_IsInSafety);  
                }
                break;

            case "Bomb Crate":
                m_NearbyBombCrate = collision.gameObject;
                break;

            case "Safety":
                m_IsInSafety = true;
                break;

            case "Waterfall":
                m_WaterCollisions++;
                break;
        }
    }

    void OnTriggerExit2D ( Collider2D collision )
    {
        switch ( collision.gameObject.tag ) {
            case "Lever":
                m_NearbyLeverClass = null;
                break;
            case "Pressure plate":
                m_NearbyPressurePlate = null;
                break;
            case "Bomb":
                // interaction box of the bomb
                if ( collision is BoxCollider2D && !m_HoldingBomb.Value ) {
                    m_NearbyBomb = null;
                }
                break;

            case "Bomb Crate":
                m_NearbyBombCrate = null;    
                break;

            case "Safety":
                m_IsInSafety = false;
                break;

            case "Waterfall":
                m_WaterCollisions--;
                if ( m_WaterCollisions < 0 )
                    m_WaterCollisions = 0;
                break;
        }
    }
}