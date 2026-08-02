using UnityEngine;

public class body : MonoBehaviour
{
    private SnowPathDrawer pathDrawer;
    private float firstPlayerY;

    public float bodySpotSize = 1.8f;
    public LayerMask groundLayer;

    public float noiseScale = 0.05f;
    public Vector2 snowHeightRange = new Vector2(0f, 1.5f);

    void Start()
    {
        pathDrawer = GetComponentInParent<SnowPathDrawer>();
        firstPlayerY = transform.position.y;
    }

    void Update()
    {
        if (Physics.Raycast(transform.position, Vector3.down, out RaycastHit hit, 3f, groundLayer))
        {
            Vector3 groundPoint = hit.point;

            float x = groundPoint.x;
            float y = groundPoint.y;
            float z = groundPoint.z;
            Vector2 uv = new Vector2(2f * x + y, y + 2f * z);
            uv *= noiseScale;

            float noiseVal = ExactShaderGraphNoise(uv);

            float localSnowHeight = Remap(noiseVal, 0f, 1f, snowHeightRange.x, snowHeightRange.y);
            localSnowHeight = Mathf.Clamp(localSnowHeight, snowHeightRange.x, snowHeightRange.y);
            float snowWorldY = groundPoint.y + (localSnowHeight * hit.normal.y);

            float playerPasLocalHeight = transform.position.y - groundPoint.y;

            float snowTriggerThresholdY = groundPoint.y + (playerPasLocalHeight * 0.5f);
            //Debug.Log("Y snow>" + (snowWorldY ) + "< Y transform:"+ snowTriggerThresholdY);
            if (snowWorldY  > snowTriggerThresholdY)
            {
                    if (pathDrawer != null)
                    {
                        pathDrawer.DrawFootprintAt(hit.textureCoord, bodySpotSize, hit.collider);
                    }
            }
        }
    }

    float Remap(float value, float inMin, float inMax, float outMin, float outMax)
    {
        return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin);
    }

    float Frac(float v) { return v - Mathf.Floor(v); }
    Vector2 Frac(Vector2 v) { return new Vector2(v.x - Mathf.Floor(v.x), v.y - Mathf.Floor(v.y)); }
    float Mod(float a, float b) { return a - b * Mathf.Floor(a / b); }
    Vector2 Mod(Vector2 a, float b) { return new Vector2(Mod(a.x, b), Mod(a.y, b)); }

    Vector2 UnityGradientNoiseDir(Vector2 p)
    {
        p = Mod(p, 289f);
        float x = Mod((34f * p.x + 1f) * p.x, 289f) + p.y;
        x = Mod((34f * x + 1f) * x, 289f);
        x = Frac(x / 41f) * 2f - 1f;
        return new Vector2(x - Mathf.Floor(x + 0.5f), Mathf.Abs(x) - 0.5f).normalized;
    }

    float ExactShaderGraphNoise(Vector2 p)
    {
        Vector2 ip = new Vector2(Mathf.Floor(p.x), Mathf.Floor(p.y));
        Vector2 fp = Frac(p);

        float d00 = Vector2.Dot(UnityGradientNoiseDir(ip), fp);
        float d01 = Vector2.Dot(UnityGradientNoiseDir(ip + new Vector2(0, 1)), fp - new Vector2(0, 1));
        float d10 = Vector2.Dot(UnityGradientNoiseDir(ip + new Vector2(1, 0)), fp - new Vector2(1, 0));
        float d11 = Vector2.Dot(UnityGradientNoiseDir(ip + new Vector2(1, 1)), fp - new Vector2(1, 1));

        Vector2 blend = new Vector2(
            fp.x * fp.x * fp.x * (fp.x * (fp.x * 6f - 15f) + 10f),
            fp.y * fp.y * fp.y * (fp.y * (fp.y * 6f - 15f) + 10f)
        );

        float lerpX1 = Mathf.Lerp(d00, d10, blend.x);
        float lerpX2 = Mathf.Lerp(d01, d11, blend.x);
        return Mathf.Lerp(lerpX1, lerpX2, blend.y) + 0.5f;
    }
}