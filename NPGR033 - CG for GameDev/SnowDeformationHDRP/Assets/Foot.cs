using UnityEngine;

public class Foot : MonoBehaviour
{
    private SnowPathDrawer pathDrawer;
    private Vector3 lastDrawPosition;

    public float footSpotSize = 0.4f;
    public LayerMask groundLayer;

    public float footTouchThreshold = 0.15f;

    void Start()
    {
        pathDrawer = GetComponentInParent<SnowPathDrawer>();
    }

    void Update()
    {
        Vector3 rayOrigin = transform.position + Vector3.up * 1.0f;

        if (Physics.Raycast(rayOrigin, Vector3.down, out RaycastHit hit, 2f, groundLayer))
        {
            float distanceFromGround = transform.position.y - hit.point.y;

            if (distanceFromGround < footTouchThreshold)
            {
                    if (pathDrawer != null)
                    {
                        pathDrawer.DrawFootprintAt(hit.textureCoord, footSpotSize, hit.collider);
                        lastDrawPosition = hit.point;
                    }
            }
        }
    }
}