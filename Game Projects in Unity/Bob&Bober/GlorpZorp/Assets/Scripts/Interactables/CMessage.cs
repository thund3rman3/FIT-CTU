using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.UIElements;

public class CMessage : MonoBehaviour
{
    public GameObject m_Message;
    public Vector3 m_MessageOffset = new Vector3 ( 0, 1, 0 );

    private SpriteRenderer m_SpriteRenderer;
    private GameObject m_MessageObject;

    private bool m_Visible = false;
    private List<CBober> m_NearbyActiveBobers = new List<CBober>();
    private List<CBober> m_NearbyNonActiveBobers = new List<CBober>();

    public void SetAlpha ( float alpha )
    {
        Color tmp = m_SpriteRenderer.color;
        tmp.a = alpha;
        m_SpriteRenderer.color = tmp; 
    }

    public void Toggle()
    {
        m_Visible = !m_Visible;

        SetAlpha ( 1 - m_SpriteRenderer.color.a );
    }

    void Start()
    {
        m_MessageObject = Instantiate ( m_Message, transform.position + m_MessageOffset, Quaternion.identity );
        m_MessageObject.transform.parent = transform.parent;
        m_SpriteRenderer = m_MessageObject.GetComponent<SpriteRenderer>();

        SetAlpha ( 0 );    
    }

    void Update()
    {
        // a nearby bober has deactivated
        foreach ( CBober boberClass in m_NearbyActiveBobers.ToList() ) {
            if ( !boberClass.m_Active.Value ) {
                m_NearbyActiveBobers.Remove ( boberClass ); 
                m_NearbyNonActiveBobers.Add ( boberClass );
            }    
        }

        // a nearby bober has activated
        foreach ( CBober boberClass in m_NearbyNonActiveBobers.ToList() ) {
            if ( boberClass.m_Active.Value ) {
                m_NearbyActiveBobers.Add ( boberClass );  
                m_NearbyNonActiveBobers.Remove ( boberClass );
            }    
        }

        // if there are no more nearby active bobers
        if ( m_NearbyActiveBobers.Count == 0 && m_Visible ) {
            Toggle();    
        }

        // if there suddenly is an active bober nearby
        if ( m_NearbyActiveBobers.Count > 0 && !m_Visible ) {
            Toggle();    
        }

        m_MessageObject.transform.position = transform.position + m_MessageOffset;
    }

    void OnTriggerEnter2D ( Collider2D collision )
    {
        CBober boberClass = collision.GetComponent<CBober>();

        if ( boberClass && boberClass.m_Active.Value ) {
            m_NearbyActiveBobers.Add ( boberClass );
        }
        else if ( boberClass && !boberClass.m_Active.Value ) {
            m_NearbyNonActiveBobers.Add ( boberClass );
        }
    }

    void OnTriggerExit2D ( Collider2D collision )
    {
        CBober boberClass = collision.GetComponent<CBober>();

        if ( boberClass && m_NearbyActiveBobers.Contains ( boberClass ) ) {
            m_NearbyActiveBobers.Remove ( boberClass );
        }

        if ( boberClass && m_NearbyNonActiveBobers.Contains ( boberClass ) ) {
            m_NearbyNonActiveBobers.Remove ( boberClass );
        }
    }
}
