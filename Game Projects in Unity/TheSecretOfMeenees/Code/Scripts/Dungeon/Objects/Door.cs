using System.Collections;
using System.Collections.Generic;
using System.Net;
using System.Xml;
using Unity.VisualScripting;
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

    private float minRotation;
    private float maxRotation;

    void Start(){
        child = transform.GetChild(0);
        Block = transform.GetChild(1).gameObject;

        if ( (child.rotation.eulerAngles.y - Range)%360 < 0 ) {
            minRotation = 360 + (child.rotation.eulerAngles.y - Range)%360; 
        }
        else{
            minRotation = (child.rotation.eulerAngles.y - Range)%360;
        }

        maxRotation = child.rotation.eulerAngles.y;
    }
    void Update(){
        if ( Locked ) {
            child.rotation = Quaternion.Euler ( 0, maxRotation, 0 );
        }
        else if(Active){
            if ( Open ) {
                if ( child.rotation.eulerAngles.y > minRotation - 1.0f && child.rotation.eulerAngles.y < minRotation + 1.0f ) {
                    Active = false;
                }

                child.Rotate(new Vector3(0, -80f, 0)*Time.deltaTime);
            }
            else {
                if ( child.rotation.eulerAngles.y < maxRotation + 1.0f && child.rotation.eulerAngles.y > maxRotation - 1.0f ){
                    Block.SetActive(true);
                    Active = false;
                }

                child.Rotate(new Vector3(0, 80f, 0)*Time.deltaTime);
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
        Spotlight.SetActive(true); // spotlight on
        Locked = false;
    }

    public void Lock()
    {
        Spotlight.SetActive(false); // spotlight on
        Locked = true;
    }
}
