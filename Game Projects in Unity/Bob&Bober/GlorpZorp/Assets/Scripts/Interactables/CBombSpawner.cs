using Unity.Netcode;
using UnityEngine;

public class CBombSpawner : MonoBehaviour
{
    private GameObject m_Bomb = null; // keeps track of the spawned bomb
    [SerializeField] GameObject m_BombPrefab = null;

    [SerializeField] Sprite m_BombCrateEmptySprite;
    private Sprite m_BombCrateFullSprite;
    private SpriteRenderer m_SpriteRenderer;

    private bool m_BombSpawned = false;

    private void Start()
    {
        m_SpriteRenderer = GetComponent<SpriteRenderer>();  
        m_BombCrateFullSprite = m_SpriteRenderer.sprite;
    }

    private void Update()
    {
        // bomb was just spawned
        if ( m_Bomb && !m_BombSpawned ) {
            m_BombSpawned = true;
            m_SpriteRenderer.sprite = m_BombCrateEmptySprite;
        }

        // bomb just exploded
        if ( !m_Bomb && m_BombSpawned ) {
            m_BombSpawned = false;
            m_SpriteRenderer.sprite = m_BombCrateFullSprite;
        }
    }

    public void SpawnBomb()
    {
        // one box can only have 1 bomb spawned in the world at a time
        if ( m_Bomb == null ) {
            m_Bomb = Instantiate ( m_BombPrefab, transform.position, Quaternion.identity );
            m_Bomb.GetComponent<NetworkObject>().Spawn();
        }     
    }
}
