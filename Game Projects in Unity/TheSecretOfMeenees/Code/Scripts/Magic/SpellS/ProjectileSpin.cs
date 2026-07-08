using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ProjectileSpin : MonoBehaviour
{
    // Start is called before the first frame update
    [SerializeField] float AnimTime;
    float timeStamp;
    [SerializeField] float AnimationSpeed = 227;
    [SerializeField] float SpinSpeed = 337;
    float maxTime = 300;
    void Start()
    {
        AnimTime = 0;
    }

    // Update is called once per frame
    void Update()
    {
        AnimTime += (timeStamp = Time.deltaTime) * AnimationSpeed;
        if (AnimTime > maxTime) AnimTime = 0;
        transform.Rotate(new Vector3(GetV(AnimTime+100) * timeStamp, GetV(AnimTime) * timeStamp, GetV(AnimTime-100) * timeStamp));
    }
    private float GetV(float time){
        time = (time % maxTime + maxTime) % maxTime;
        if(time < 300){
            if (time < 100)         return SpinSpeed * (time / 100);
            else if (time > 200)    return SpinSpeed * (1-((time - 200) / 100));
            else                    return SpinSpeed;
        }
        return 0;
    }
}
