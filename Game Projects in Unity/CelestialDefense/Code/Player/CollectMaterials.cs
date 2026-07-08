using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/*
 * Not used
 */
public class CollectMaterials : MonoBehaviour
{
    private PlayerTool _toolScript;
    public int stone, iron, wood;
    public const int WaitTime = 5;
    public int speed = 5;
    private bool _inWork = false;
    void Start()
    {
        _toolScript = GetComponent<PlayerTool>();
    }

    // Update is called once per frame
    void Update()
    {
        if (_toolScript && _toolScript.working && !_inWork)
        {
            _inWork = true;
            StartCoroutine("AddMaterial", speed);
        }

    }
    
   private IEnumerator AddMaterial(int speed)
    {
        // suspend execution for 5 seconds
        yield return new WaitForSeconds(WaitTime/speed);
        RaycastHit hit = _toolScript.Hit;
      
        if (_toolScript.working)
        {
            switch (hit.transform.tag)
            {
                case "Wall":
                    wood++;
                    break;

            }
        }

        PrintMaterials();
        _inWork = false;
 
    }

   private void PrintMaterials()
   {
       Debug.Log("Wood:"+wood+" Stone :"+stone+" Iron:"+iron);

   }
}
