using System.Collections.Generic;
using UnityEngine;

public class CLoopAnimation : MonoBehaviour
{
    public bool m_Animate = true;
    private bool m_AnimatePrev;
    public float m_Frequency = 0.5f;

    private int m_SpriteIndex = 0;
    private int m_SpriteCount;

    // list of sprites to be looped
    [SerializeField] List<Sprite> m_Sprites;
    
    public int GetNumberOfSprites()
    {
        return m_Sprites.Count;    
    }

    void Start() {
        m_SpriteCount = m_Sprites.Count;
        m_AnimatePrev = m_Animate;

        if ( m_SpriteCount == 0 ) {
            Debug.LogError ( $"Object {gameObject.name} has 0 sprites in its animation sheet." );    
        }

        if ( m_Animate ) {
            InvokeRepeating ( nameof ( LoopSprite ), 0, m_Frequency ); 
        }
    }

    void LoopSprite() {
        gameObject.GetComponent<SpriteRenderer>().sprite = m_Sprites[m_SpriteIndex];

        m_SpriteIndex += 1;
        if ( m_SpriteIndex == m_SpriteCount ) {
            m_SpriteIndex = 0;
        }
    }

    void Update() {
        // stop animating
        if ( m_AnimatePrev && !m_Animate ) {
            CancelInvoke();
        }

        // start animating
        if ( !m_AnimatePrev && m_Animate ) {
            InvokeRepeating ( nameof ( LoopSprite ), 0, m_Frequency );
        }

        m_AnimatePrev = m_Animate;
    }
}
