using UnityEngine;
using UnityEngine.SceneManagement;

public class CParallax : MonoBehaviour
{
    [SerializeField] private float m_MovementScalar = 0.25f;

    private Camera m_Camera;
    private Vector3 m_PreviousCameraPosition;

    void Start()
    {
        m_Camera = Camera.main;

        m_PreviousCameraPosition = m_Camera.transform.position;
    }

    // Update is called once per frame
    void Update()
    {
        Vector3 cameraPosition = m_Camera.transform.position;

        // camera moved
        if ( m_PreviousCameraPosition != cameraPosition ) {
            transform.position = transform.position + m_MovementScalar * ( cameraPosition - m_PreviousCameraPosition );
        }

        m_PreviousCameraPosition = cameraPosition;
    }
}
