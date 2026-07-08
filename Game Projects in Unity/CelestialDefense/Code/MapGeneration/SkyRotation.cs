using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SkyRotation : MonoBehaviour
{
    GameObject Moon = null;
    GameObject Stars = null;
    GameObject Sun = null;

    Material material = null;

    void Start()
    {
        Moon = transform.GetChild(0).gameObject;
        Stars = transform.GetChild(1).gameObject;
        Sun = transform.GetChild(2).gameObject;

        MeshRenderer renderer = GetComponent<MeshRenderer>();
        List<Material> mats = new List<Material>();
        renderer.GetMaterials(mats);

        material = mats[0];

       
    }

    // Update is called once per frame
    void Update()
    {

        Moon.transform.Rotate(new Vector3(1, 0, 0), 36 * Time.deltaTime);
        Stars.transform.Rotate(new Vector3(1, 0, 0), 36 * Time.deltaTime);
        Sun.transform.Rotate(new Vector3(1, 0, 0), 30 * Time.deltaTime);

        material.SetTextureOffset("_MainTex", new Vector2((Time.time % 10) / 10f, 0));
    }
}
