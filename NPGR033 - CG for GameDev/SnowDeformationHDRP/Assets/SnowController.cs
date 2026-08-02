using UnityEngine;

public class SnowController : MonoBehaviour
{
    public ComputeShader snowComputeShader;
    public RenderTexture snowRT;
    public float colorValueToAdd;

    private string snowImageProperty = "snowImage";
    private string colorValueProperty = "colorValueToAdd";
    private string resolutionProperty = "resolution";
    private string positionXProperty = "positionX";
    private string positionYProperty = "positionY";
    private string spotSizeProperty = "spotSize";

    private string csMainKernel = "CSMain";
    private string fillWhiteKernel = "FillWhite";

    //private Terrain terrain;

    public int resolution = 2048;

    private void Awake()
    {
        CreateRenderTexture();
        SetRTColorToWhite();
        SetMaterialTexture();
        InvokeRepeating(nameof(AddSnowLayer), 0.1f, 0.1f);

    }

    void CreateRenderTexture()
    {
        snowRT = new RenderTexture(resolution, resolution, 24);
        snowRT.enableRandomWrite = true;
        snowRT.Create();
    }

    void SetRTColorToWhite()
    {
        int kernel_handle = snowComputeShader.FindKernel(fillWhiteKernel);
        snowComputeShader.SetTexture(kernel_handle, snowImageProperty, snowRT);
        snowComputeShader.SetFloat(colorValueProperty, colorValueToAdd);
        snowComputeShader.SetFloat(resolutionProperty, resolution);
        snowComputeShader.SetFloat(positionXProperty, 0);
        snowComputeShader.SetFloat(positionYProperty, 0);
        snowComputeShader.SetFloat(spotSizeProperty, 0);
        snowComputeShader.Dispatch(kernel_handle, snowRT.width / 8, snowRT.height / 8, 1);
    }

    //void SetMaterialTexture()
    //{
    //    terrain = GetComponent<Terrain>();

    //    if (terrain != null && terrain.materialTemplate != null)
    //    {
    //        Material instancedMat = new Material(terrain.materialTemplate);

    //        instancedMat.SetTexture("_PathTexture", snowRT);

    //        terrain.materialTemplate = instancedMat;
    //    }
    //    else
    //    {
    //        Debug.LogError("Terrain component missing");
    //    }
    //}

    void SetMaterialTexture()
    {
        Renderer rend = GetComponent<Renderer>();

        if (rend != null && rend.material != null)
        {
            rend.material.SetTexture("_PathTexture", snowRT);
        }
        else
        {
            Debug.LogError("Renderer component missing!");
        }
    }

    void AddSnowLayer()
    {
        int kernel_handle = snowComputeShader.FindKernel(csMainKernel);
        snowComputeShader.SetTexture(kernel_handle, snowImageProperty, snowRT);
        snowComputeShader.SetFloat(colorValueProperty, colorValueToAdd);
        snowComputeShader.SetFloat(resolutionProperty, resolution);
        snowComputeShader.SetFloat(positionXProperty, 0);
        snowComputeShader.SetFloat(positionYProperty, 0);
        snowComputeShader.SetFloat(spotSizeProperty, 0);
        snowComputeShader.Dispatch(kernel_handle, snowRT.width / 8, snowRT.height / 8, 1);
    }
}