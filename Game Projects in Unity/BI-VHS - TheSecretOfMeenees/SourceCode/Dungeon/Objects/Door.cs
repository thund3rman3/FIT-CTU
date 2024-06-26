using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Door : MonoBehaviour
{
    [SerializeField] bool Active = false;
    [SerializeField] bool Open = false;
    [SerializeField] bool Locked = true;//then change
    [SerializeField] int Range = 120;
    Transform child;  // rotated doors
    [SerializeField] GameObject Spotlight; // black wall
    GameObject Block; // black wall
    void Start(){
        child = transform.GetChild(0);
        Block = transform.GetChild(1).gameObject;
    }
    void Update(){
        if(Active){
            if(Open){
                child.Rotate(new Vector3(0, -80f, 0)*Time.deltaTime);
                if ((child.rotation.eulerAngles.y + 268)%360  < 360-Range)
                    Active = false;
            } else {
                child.Rotate(new Vector3(0, 80f, 0)*Time.deltaTime);
                //Debug.Log(child.rotation.eulerAngles.y);
                if ((child.rotation.eulerAngles.y + 270)%360 < 1){
                    Block.SetActive(true);
                    Active = false;
                }
            }
        }
    }
    private void OnTriggerEnter(Collider other) {
        if(other.tag == "Player" && !Open && !Locked){
            Open = true;
            Active = true;
            Block.SetActive(false);
        }
    }
    private void OnTriggerExit(Collider other) {
        if(other.tag == "Player" && Open){
            Open = false;
            Active = true;
        }
    }

    public void Unlock()
    {
        Debug.Log("before");
        Spotlight.SetActive(true); // spotlight on
        Locked = false;
    }
}
