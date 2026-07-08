using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class VillageController : MonoBehaviour
{
    public GameObject Houses;
    public GameObject BrokenHouses;
    public GameObject Meenees;



    // Start is called before the first frame update
    void Start()
    {
        for(int i = 0; i < 4; i++)
        {
            for(int u = 0; u < Mathf.Min(PersistData.rescuedMeeneeGeneric[i], 4); u++)
            {
                Meenees.transform.GetChild(i).transform.GetChild(u).gameObject.SetActive(true);
            }
            if(PersistData.rescuedMeeneeNPC[i+1])
            {
                Meenees.transform.GetChild(i).transform.GetChild(Meenees.transform.childCount).gameObject.SetActive(true); 
            }
        }
        if(PersistData.rescuedMeeneeNPC[3]) //if we have Bob, build
        {
            for (int i = 0; i < 4; i++)
            {
                for (int u = 0; u < Mathf.Min(PersistData.rescuedMeeneeGeneric[i], 4); u++)
                {
                    Houses.transform.GetChild(i).transform.GetChild(u).gameObject.SetActive(true);
                    BrokenHouses.transform.GetChild(i).transform.GetChild(u).gameObject.SetActive(false);
                }
                Houses.transform.GetChild(i).transform.GetChild(Meenees.transform.childCount).gameObject.SetActive(true);
                BrokenHouses.transform.GetChild(i).transform.GetChild(Meenees.transform.childCount).gameObject.SetActive(false);
            }
        }
    }

}
