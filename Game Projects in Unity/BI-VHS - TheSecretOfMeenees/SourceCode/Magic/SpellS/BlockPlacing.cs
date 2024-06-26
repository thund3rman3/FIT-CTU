using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BlockPlacing : MonoBehaviour
{
    [SerializeField] int CollisionCount = 0;
    public GameObject Red;
    [SerializeField] GameObject Green;
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        Stabilize();
    }
    void Stabilize(){
        transform.rotation = Quaternion.Euler(90f,transform.parent.rotation.eulerAngles.y,0f);
    }
    void OnCollisionEnter(Collision other) {
        if(other.gameObject.CompareTag("Magic"))
        {
            Debug.Log("Collision Block:" + CollisionCount);
            if (CollisionCount == 0) CorrectPlacement(false);
            CollisionCount++;
        }
    }
    void OnCollisionExit(Collision other) {
        if (other.gameObject.CompareTag("Magic"))
        {
            Debug.Log("Collision Block:" + CollisionCount);
            CollisionCount--;
            if (CollisionCount == 0) CorrectPlacement(true);
        }
    }
    
    void CorrectPlacement(bool correct){
        Debug.Log("Collision Block:" + correct);
        Red.SetActive(!correct);
        Green.SetActive(correct);
    }
}
