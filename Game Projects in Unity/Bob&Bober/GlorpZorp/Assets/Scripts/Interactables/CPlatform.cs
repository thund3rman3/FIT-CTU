using System.Collections.Generic;
using Unity.Netcode;
using UnityEngine;
using UnityUtils;

public class CPlatform : NetworkBehaviour
{
    // is active
    [System.NonSerialized] public NetworkVariable<bool> m_IsOn = new NetworkVariable<bool>(true,
        NetworkVariableReadPermission.Everyone,
        NetworkVariableWritePermission.Server
        );

    // Initial values
    [SerializeField] public bool m_IsOnInitVal = true;
    [SerializeField] bool m_GoIntoBackground = true;
    [SerializeField] bool m_DisableCollidersWhenInBackground = true;
    [SerializeField] float m_BackgroundV = 0.5f;

    private List<GameObject> m_ObjectsOnPlatform = new List<GameObject>();
    private Vector3 m_PreviousPosition;


    private void FixedUpdate()
    {
        if ( transform.position != m_PreviousPosition ) {
            ApplyForce ( ( transform.position - m_PreviousPosition ) / Time.fixedDeltaTime );

            m_PreviousPosition = transform.position;
        }
    }


    public virtual void Toggle() {
        if(IsServer)
            m_IsOn.Value = !m_IsOn.Value;
    }


    private void ApplyForce ( Vector2 force )
    {
        if(!m_IsOn.Value)
            return;

        foreach ( GameObject obj in m_ObjectsOnPlatform ) {
            CRigidBodyHandler rigidBodyHandler = obj.GetComponent<CRigidBodyHandler>();
            
            if ( rigidBodyHandler != null ) {
                if ( force.y == 0 ) {
                    rigidBodyHandler.AddForce ( force ); 
                }
                else {
                    // force doesn't work well with vertically moving platforms, simply change position
                    rigidBodyHandler.AddPositionY ( force.y * Time.fixedDeltaTime );    
                }
            }
        }
    }



    void OnCollisionEnter2D ( Collision2D collision )
    {
        if(!IsServer)
            return;
        // when an object on the platform changes body type back to dynamic
        if ( m_ObjectsOnPlatform.Contains ( collision.gameObject ) ) { 
            return;
        }

        Vector2 direction = collision.GetContact ( 0 ).normal;

        // collision at the top and the collider has a rigidbody
        if ( direction.y <= -0.7 && collision.gameObject.GetComponent<Rigidbody2D>() != null ) {
            m_ObjectsOnPlatform.Add ( collision.gameObject );
        }
    }

    void OnCollisionExit2D ( Collision2D collision )
    {
        if(!IsServer)
            return; 
        if ( m_ObjectsOnPlatform.Contains ( collision.gameObject ) ) { 
            Rigidbody2D rigidBody = collision.gameObject.GetComponent<Rigidbody2D>();

            // object that was standing on the platform changed body type to kinematic, keep the object
            if ( rigidBody && rigidBody.bodyType == RigidbodyType2D.Kinematic ) {
                return;
            }

            m_ObjectsOnPlatform.Remove ( collision.gameObject ); 
        }
    }


    private void UpdateVisuals(bool isOn)
    {
        if (m_GoIntoBackground)
        {
            if ( m_DisableCollidersWhenInBackground ) {
                BoxCollider2D[] childrenBoxes = transform.GetComponentsInChildren<BoxCollider2D>();

                foreach ( BoxCollider2D box in childrenBoxes ) {
                    box.enabled = isOn;
                }
            }
            
            SpriteRenderer[] childrenSprites = transform.GetComponentsInChildren<SpriteRenderer>();

            foreach ( Transform child in transform.Children() ) {
                SpriteRenderer spriteRenderer = child.GetComponent<SpriteRenderer>();
                
                if ( spriteRenderer ) {
                    float V = isOn ? 1.0f : m_BackgroundV;
                    float H = 0.0f;
                    float S = 0.0f;
                    float _;

                    Color.RGBToHSV ( spriteRenderer.color, out H, out S, out _ );
                    UnityEngine.Color color = UnityEngine.Color.HSVToRGB( H, S, V );

                    color.a = spriteRenderer.color.a;
                    spriteRenderer.color = color;  
                }
            }
        }
        else
        {
            foreach ( Transform child in transform.Children() ) {
                child.gameObject.SetActive ( isOn );
            }
        }
    }


    //Network
    //---------------------------------------------------------------------------//
    public override void OnNetworkSpawn()
    {
        base.OnNetworkSpawn();
        m_PreviousPosition = transform.position;
        if (IsServer)
        {
            m_IsOn.Value = m_IsOnInitVal;
        }
        UpdateVisuals(m_IsOn.Value);

        m_IsOn.OnValueChanged += OnStateChanged;
    }

    public override void OnNetworkDespawn()
    {
        base.OnNetworkDespawn();
        m_IsOn.OnValueChanged -= OnStateChanged;
    }


    //Callbacks
    //---------------------------------------------------------------------------//
    private void OnStateChanged(bool previous, bool current)
    {
        UpdateVisuals(current);
    }
}
