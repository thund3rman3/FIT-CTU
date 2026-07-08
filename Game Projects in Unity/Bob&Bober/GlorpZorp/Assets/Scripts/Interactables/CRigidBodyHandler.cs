using UnityEngine;


public class CRigidBodyHandler : MonoBehaviour
{
    private Rigidbody2D m_RigidBody;

    // forces accumulated between fixed updates
    private Vector2 m_Force;
    private Vector2 m_PreviousForce;

    public bool m_KeepVerticalVelocity = true;  // keep vertical velocity between velocity updates
    public bool m_KeepHorizontalVelocity = false;  // keep horizontal velocity between velocity updates


    void Start()
    {
        m_RigidBody = GetComponent<Rigidbody2D>();
        m_Force = Vector2.zero;
        m_PreviousForce = m_Force;
    }

    void FixedUpdate()
    {
        // kept forces from previous calculations
        Vector2 keptForce = Vector2.zero;

        // subtract previous FixedUpdate forces so the forces don't add up and explode
        if ( m_KeepHorizontalVelocity ) {
            keptForce += new Vector2 ( m_RigidBody.linearVelocity.x - m_PreviousForce.x, 0 );
        }
        if ( m_KeepVerticalVelocity ) {
            keptForce += new Vector2 ( 0, m_RigidBody.linearVelocity.y - m_PreviousForce.y );
        }

        Vector2 resultForce = keptForce + m_Force ;

        if ( m_RigidBody.bodyType == RigidbodyType2D.Dynamic ) {
            m_RigidBody.linearVelocity = resultForce;
        }
        else {
            transform.position += new Vector3 ( resultForce.x, resultForce.y, 0 ) * Time.fixedDeltaTime;    
        }

        m_PreviousForce = m_Force;
        m_Force = Vector2.zero;
    }

    public void AddPositionX(float position)
    {
        transform.position = transform.position + new Vector3(position, 0, 0);
    }

    public void AddPositionY(float position)
    {
        transform.position = transform.position + new Vector3(0, position, 0);
    }

    public void SetLinearVelocity(Vector2 force)
    {
        m_RigidBody.linearVelocity = force;
    }

    public Vector2 GetLinearVelocity()
    {
        return m_RigidBody.linearVelocity;
    }

    public void SetAngularVelocity(float speed)
    {
        m_RigidBody.angularVelocity = speed;
    }

    public void AddForce(Vector2 force)
    {
        m_Force += force;
    }
}
