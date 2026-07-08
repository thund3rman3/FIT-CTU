using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RoomColider : MonoBehaviour
{
    public Dungeon_Room Link;
    // Start is called before the first frame update
    void Start(){
        Link = transform.parent.GetComponent<Dungeon_Room>();
    }
     private void OnTriggerEnter(Collider other) {
        switch(other.tag){
            case "Dungeon Tile":
                Debug.Log("RoomDetected");
                if (Link.ColUpdate < 0) Link.ColUpdate = 0;
                Vector3 pos = ((transform.position - other.transform.position) / 9) + new Vector3(2,0,2);
                Link.collisions |= (uint) 0x1 << (int)System.Math.Round(4 - pos.x + pos.z*5);
                //Instantiate(Link.Test, new Vector3(0, 12, 0) + other.transform.position, Quaternion.identity).transform.parent = transform;
                break;
            default:
                break;
        }
    }
}
