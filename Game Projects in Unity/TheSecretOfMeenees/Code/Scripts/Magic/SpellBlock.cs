using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BlockSpell : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    public void DeactivatePlacing(){
        Destroy(transform.GetComponent<BlockPlacing>());
        Destroy(transform.GetChild(0).gameObject);
        Destroy(transform.GetChild(1).gameObject);
    }
}
