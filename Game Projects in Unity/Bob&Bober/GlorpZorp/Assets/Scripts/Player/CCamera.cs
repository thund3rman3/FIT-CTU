using UnityEngine;

public class CCamera : MonoBehaviour
{
    [SerializeField] private Camera m_Camera;
    [SerializeField] float m_PositionZ = -10.0f;
    [SerializeField] float m_MinX;
    [SerializeField] float m_MaxX;
    [SerializeField] float m_MinY;
    [SerializeField] float m_MaxY;

    // bounds corners
    private Vector2 m_TopRight;
    private Vector2 m_BottomLeft;

    void Awake()
    {
        m_TopRight = new Vector2 ( m_MaxX, m_MaxY );
        m_BottomLeft = new Vector2 ( m_MinX, m_MinY );

        if( m_Camera == null ) {
            m_Camera = GetComponent<Camera>();
            //if (m_Camera == null)
            //{
            //    Debug.LogError("[CCamera] Camera not found!");
            //}
        }
    }

    public void SetPosition ( Vector2 position )
    {
        // first move the camera
        transform.position = new Vector3 ( position.x, position.y, m_PositionZ ); 
        
        // then correct position
        Vector2 topRight = m_Camera.ViewportToWorldPoint ( new Vector3 ( 1.0f, 1.0f, - m_PositionZ ) );
        Vector2 bottomLeft = m_Camera.ViewportToWorldPoint ( new Vector3 ( 0.0f, 0.0f, - m_PositionZ ) );
        Vector2 centerPosition = m_Camera.ViewportToWorldPoint ( new Vector3 ( 0.5f, 0.5f, - m_PositionZ ) );
        Vector2 middleDifference = topRight - centerPosition;

        if ( topRight.y > m_TopRight.y ) { // top
            position.y = m_TopRight.y - middleDifference.y;
        }
        if ( bottomLeft.y < m_BottomLeft.y ) { // bottom
            position.y = m_BottomLeft.y + middleDifference.y;
        }
        if ( topRight.x > m_TopRight.x ) { // right
            position.x = m_TopRight.x - middleDifference.x;
        }
        if ( bottomLeft.x < m_BottomLeft.x ) { // left
            position.x = m_BottomLeft.x + middleDifference.x;
        }

        transform.position = new Vector3 ( position.x, position.y, m_PositionZ );   
    }
}
