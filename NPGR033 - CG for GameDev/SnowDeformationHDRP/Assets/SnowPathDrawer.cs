using UnityEngine;

public class SnowPathDrawer : MonoBehaviour
{
    public ComputeShader snowComputeShader;
    public RenderTexture snowRT;

    private string snowImageProperty = "snowImage";
    private string colorValueProperty = "colorValueToAdd";
    private string resolutionProperty = "resolution";
    private string positionXProperty = "positionX";
    private string positionYProperty = "positionY";
    private string spotSizeProperty = "spotSize";
    private string drawSpotKernel = "DrawSpot";

    private Vector2Int position = new Vector2Int(256, 256);
    public float spotSize = 0.5f;

    //private Terrain terrain;

    //private void Start()
    //{
    //    terrain = Terrain.activeTerrain;
    //}

    void DrawSpot()
    {
        if (snowRT == null || snowComputeShader == null)
        {
            Debug.LogWarning("Snow RenderTexture or ComputeShader is not assigned. Cannot draw spot.");
            return;
        }

        int kernel_handle = snowComputeShader.FindKernel(drawSpotKernel);
        snowComputeShader.SetTexture(kernel_handle, snowImageProperty, snowRT);

        snowComputeShader.SetFloat(colorValueProperty, 0); // 0 = kreslíme èernou díru
        snowComputeShader.SetFloat(resolutionProperty, snowRT.width);
        snowComputeShader.SetFloat(positionXProperty, position.x);
        snowComputeShader.SetFloat(positionYProperty, position.y);
        snowComputeShader.SetFloat(spotSizeProperty, spotSize);
        snowComputeShader.Dispatch(kernel_handle, snowRT.width / 8, snowRT.height / 8, 1);
    }


    bool GetPositionOnTerrain(Vector3 hitPoint, Collider snowCollider)
    {
        //Vector3 terrainPos = terrain.transform.position;
        //Vector3 size = terrain.terrainData.size;

        //float relativeX = hitPoint.x - terrainPos.x;
        //float relativeZ = hitPoint.z - terrainPos.z;
        if (snowCollider == null)
        {
            Debug.LogWarning("Snow Collider is null. Cannot calculate position on terrain.");
            return false;   
        }

        Vector3 minBounds = snowCollider.bounds.min;
        Vector3 size = snowCollider.bounds.size;   

        float relativeX = hitPoint.x - minBounds.x;
        float relativeZ = hitPoint.z - minBounds.z;

        if (relativeX < 0 || relativeX > size.x || relativeZ < 0 || relativeZ > size.z)
            return false;

        float u = relativeX / size.x;
        float v = relativeZ / size.z;

        int posX = (int)(u * snowRT.width);
        int posY = (int)(v * snowRT.height);

        position = new Vector2Int(posX, posY);
        return true;
    }

    public void DrawFootprintAt(Vector2 hitUV, float customSpotSize, Collider groundCollider)
    {
        if (groundCollider == null) return;

        SnowController sc = groundCollider.GetComponentInParent<SnowController>();
        if (sc != null)
        {
            snowRT = sc.snowRT;
        }
        else return;

        if (snowComputeShader == null) 
            snowComputeShader = sc.snowComputeShader;

        if (snowRT != null && snowComputeShader != null)
        {
            int posX = (int)(hitUV.x * snowRT.width);
            int posY = (int)(hitUV.y * snowRT.height);

            position = new Vector2Int(posX, posY);
            spotSize = customSpotSize;
            DrawSpot();
        }
    }
}