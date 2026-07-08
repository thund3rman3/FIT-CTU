using System;
using System.Collections;
using Unity.Netcode;
using UnityEngine;

public class CBomb : NetworkBehaviour
{

    // NetworkVariables
    public NetworkVariable<bool> m_IsActive = new NetworkVariable<bool>(false);
    public NetworkVariable<bool> m_IsExploded = new NetworkVariable<bool>(false);

    // Bomb properties
    private float m_TimeToExplode = 4.5f;
    private float m_LingerTime = 0.5f; // how long the explosion lingers
    private float m_ExplosionRadius = 1.5f;
    private float m_ThrowAngleSpeed = -100.0f;
    private float m_HorizontalSpeed = 5.0f;
    private float m_VerticalSpeed = 7.0f;

    // Explosion visuals
    [SerializeField] Sprite m_ExplosionSprite;
    private CircleCollider2D m_ExplosionCollider;
    private SpriteRenderer m_SpriteRenderer;
    private CLoopAnimation m_LoopAnimation;

    // Physics
    private Rigidbody2D m_RigidBody;
    private CRigidBodyHandler m_RigidbodyHandler;

    // Audio
    private AudioSource m_Audio;
    [SerializeField] private AudioClip m_ExplosionClip;


    void Awake()
    {
        // set radius of explosion, divided by pixel scale
        m_ExplosionCollider = transform.Find ( "Sprite" ).transform.GetComponent<CircleCollider2D>();
        m_ExplosionCollider.radius = m_ExplosionRadius / ( 100.0f / 12.0f );
        m_ExplosionCollider.enabled = false;

        m_SpriteRenderer = transform.Find ( "Sprite" ).transform.GetComponent<SpriteRenderer>();
        m_LoopAnimation = transform.Find ( "Sprite" ).transform.GetComponent<CLoopAnimation>();

        m_RigidBody = transform.GetComponent<Rigidbody2D>();
        m_RigidbodyHandler = transform.GetComponent<CRigidBodyHandler>();
        m_Audio = FindFirstObjectByType<AudioSource>();

    }

    public void ResetRotation()
    {
        if(!IsServer)
            return;

        m_RigidBody.angularVelocity = 0.0f;
        transform.rotation = Quaternion.identity;
    }

    public void Throw ( bool isFacingRight )
    {
        if(!IsServer)
            return;

        m_RigidbodyHandler.SetLinearVelocity ( new Vector2 ( m_HorizontalSpeed * ( isFacingRight ? 1.0f : -1.0f ), m_VerticalSpeed ) );
        m_RigidbodyHandler.SetAngularVelocity ( m_ThrowAngleSpeed );

        // if the bomb is thrown again after already being activated
        if ( !m_IsActive.Value ) {
            Activate();    
        }
    }

    public void Drop()
    {
        if(!IsServer)
            return;

        m_RigidbodyHandler.SetLinearVelocity ( new Vector2 ( 0, 0 ) );
        m_RigidbodyHandler.SetAngularVelocity ( m_ThrowAngleSpeed );

        // if the bomb is thrown again after already being activated
        if ( !m_IsActive.Value ) {
            Activate();    
        }
    }

    public void Activate()
    {
        if(!IsServer)
            return;

        m_IsActive.Value = true;
        StartCoroutine ( Explode() );
    }

    private void SetAlpha ( float alpha )
    {
        Color tmp = m_SpriteRenderer.color;
        tmp.a = alpha;
        m_SpriteRenderer.color = tmp;
    }

    IEnumerator Explode()
    {
        yield return new WaitForSeconds ( m_TimeToExplode );
        PlaySoundClientRpc();
        m_RigidBody.linearVelocity = new Vector2 ( 0.0f, 0.0f );
        m_RigidBody.angularVelocity = 0.0f;
        m_RigidBody.bodyType = RigidbodyType2D.Kinematic; // lock the bomb in place

        m_IsExploded.Value = true;

        yield return new WaitForSeconds(m_LingerTime);

        GetComponent<NetworkObject>().Despawn();
    }

    [Rpc(SendTo.ClientsAndHost)]
    private void PlaySoundClientRpc()
    {
        m_Audio.PlayOneShot(m_ExplosionClip);
    }

    IEnumerator ExplosionFade()
    {
        for ( float t = 0.0f; t < m_LingerTime; t += Time.deltaTime ) {
            float alpha = 1.0f - t / m_LingerTime;

            SetAlpha ( alpha );
            
            yield return null;
        }

        SetAlpha ( 0.0f );
        m_ExplosionCollider.enabled = false; // deactivate the trigger collider

    }

    //Network
    //---------------------------------------------------------------------------//
    public override void OnNetworkSpawn()
    {
        m_IsActive.OnValueChanged += OnActiveChanged;
        m_IsExploded.OnValueChanged += OnExplodedChanged;
    }

    public override void OnNetworkDespawn()
    {
        m_IsActive.OnValueChanged -= OnActiveChanged;
        m_IsExploded.OnValueChanged -= OnExplodedChanged;
    }


    //Callbacks
    //---------------------------------------------------------------------------//
    private void OnActiveChanged(bool prev, bool current)
    {
        if (current) 
            StartAnimationLocally();
    }

    private void OnExplodedChanged(bool prev, bool current)
    {
        if (current)
        {
            SetExplosionVisualsLocally();
            StartCoroutine(ExplosionFade());
        }
    }

    private void SetExplosionVisualsLocally()
    {
        m_LoopAnimation.m_Animate = false;
        m_SpriteRenderer.sprite = m_ExplosionSprite;
        m_ExplosionCollider.enabled = true;
    }

    private void StartAnimationLocally()
    {
        m_LoopAnimation.m_Animate = true;
        float frequency = m_TimeToExplode / (m_LoopAnimation.GetNumberOfSprites() - 1);
        m_LoopAnimation.m_Frequency = frequency + 0.01f;
    }
}
