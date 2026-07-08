using UnityEngine;
using System.Collections.Generic;
using System.Collections;
using UnityUtils;


public class CDestructible : MonoBehaviour
{
    private bool m_Destroyed = false;
    private float m_FadeOutTime = 1.0f;
    private bool m_DoneFading = false;

    [SerializeField] List<GameObject> m_Reveals;  // array of objects that fade OUT when the object is destroyed
    [SerializeField] List<GameObject> m_Hides;  // array of objects that fade IN when the object is destroyed

    private void SetAlphaToSpriteRenderer ( SpriteRenderer spriteRenderer, float alpha )
    {
        Color tmp = spriteRenderer.color;
        tmp.a = alpha;
        spriteRenderer.color = tmp;
    }

    private void SetAlphaToObject ( GameObject obj, float alpha )
    {
        // object itself has a sprite renderer
        if ( obj.GetComponent<SpriteRenderer>() ) {
            SetAlphaToSpriteRenderer ( obj.GetComponent<SpriteRenderer>(), alpha );
        }
        // otherwise get sprite renderers of children and apply to those
        else {
            Component[] childrenSpriteRenderers = obj.GetComponentsInChildren<SpriteRenderer>();

            foreach ( SpriteRenderer spriteRenderer in childrenSpriteRenderers ) {
                SetAlphaToSpriteRenderer ( spriteRenderer, alpha );
            }
        }
    }

    // sets alpha for all children objects
    private void SetAlpha ( float alpha )
    {
        // reveal
        foreach ( GameObject obj in m_Reveals ) {
            // shadows can already be destroyed by other destructible objects
            if ( obj != null ) {  
                SetAlphaToObject ( obj, alpha );
            }
        }

        // hide
        foreach ( GameObject obj in m_Hides ) {
            SetAlphaToObject ( obj, 1 - alpha );
        }

        // destructible objects themselves
        SetAlphaToObject ( this.gameObject, alpha );
    }

    public void DestroySelf()
    {
        m_Destroyed = true;

        StartCoroutine ( Fade() ); 
    }

    IEnumerator Fade()
    {
        for ( float t = 0.0f; t < m_FadeOutTime; t += Time.deltaTime ) {
            float alpha = 1.0f - t / m_FadeOutTime;

            SetAlpha ( alpha );
            
            yield return null;
        } 

        SetAlpha ( 0.0f );
        m_DoneFading = true;

        yield return null;
    }

    void FixedUpdate()
    {
        if ( m_DoneFading ) {
            // destroy shadow objects
            foreach ( GameObject obj in m_Reveals ) {
                Destroy ( obj );
            }

            Destroy ( this.gameObject );  
        }
    }

    void OnTriggerEnter2D ( Collider2D collision )
    {
        // the circle collider (explosion collider) of the bomb
        if ( collision.tag == "Bomb" && !m_Destroyed && collision.GetType() == typeof ( CircleCollider2D ) ) {
            DestroySelf();
        }
    }
}
