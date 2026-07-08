using UnityEngine;

public class Vertexbobbing : MonoBehaviour
{
    public Vector3 tmp = new Vector3(0.0f, 0.0f, 0.0005f);
    void Update()
    {
        Mesh mesh = GetComponent<MeshFilter>().mesh;
        Vector3[] vertices = mesh.vertices;
	
        for (var i = 0; i < vertices.Length; i++)
        {
	    float result = 10.0f * ( vertices[i].x + vertices[i].y );
            vertices[i] += tmp * Mathf.Sin( 2 * ( Time.time + result ) );
        }

       mesh.vertices = vertices;
    }
}