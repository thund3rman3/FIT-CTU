using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BlockShapes
{
    public static List<int>[] vertexColors = new List<int>[16] {
        //0
        new List<int>{
            0, 0, 0,
            0, 0, 0,
        },
        //1
        new List<int>{
            //Top
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            //Slope 1
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            //Slope 2
            1, 1, 1,
            1, 1, 1
        },
        //2
        new List<int>{
            //Top
            0, 0, 0,
            0, 0, 0,
           
            //side
            0, 0, 0,
            0, 0, 0,

            1, 1, 1,
            1, 1, 1,
        },
        //3
        new List<int>{
            //Slope 1.1
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            //Slope 2.1
            1, 1, 1,
            1, 1, 1,

            //Top
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            //Slope 1.2
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,

            //Slope 2.2
            1, 1, 1,
            1, 1, 1,
        },
        //4
        new List<int>{
            //Top
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            //side
            0, 0, 0,
            0, 0, 0,

            1, 1, 1,
            1, 1, 1,

            //Slope 1
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            //Slope 2
            1, 1, 1,
            1, 1, 1,
        },
        //5
        new List<int>{
            //Top
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            //Slope 1.1
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            //Slope 2.1
            1, 1, 1,
            1, 1, 1,

            //Slope 1.2
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,

            //Slope 2.2
            1, 1, 1,
            1, 1, 1,
        },
        //6
        new List<int>{
            //Empty because this ID was duplicate with 8
        },
        //7
        new List<int>{
            //Top
            0, 0, 0,
            0, 0, 0,

            //side
            0, 0, 0,
            0, 0, 0,

            1, 1, 1,
            1, 1, 1,

            //side
            0, 0, 0,
            0, 0, 0,

            1, 1, 1,
            1, 1, 1,
        },
        //8
        new List<int>{
            //Top
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            //side
            0, 0, 0,
            0, 0, 0,

            1, 1, 1,
            1, 1, 1,

            //Slope 1
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,

            //Slope 2
            1, 1, 1,
            1, 1, 1,
        },
        //9
        new List<int>{
            //side
            0, 0, 0,
            0, 0, 0,

            1, 1, 1,
            1, 1, 1,

            //side
            0, 0, 0,
            0, 0, 0,

            1, 1, 1,
            1, 1, 1,

            //Top
            0, 0, 0,
            0, 0, 0,
        },
        //10
        new List<int>{
            //Slope 1.1
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            //Slope 2.1
            1, 1, 1,
            1, 1, 1,

            //Slope 1.2
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,

            //Slope 2.2
            1, 1, 1,
            1, 1, 1,

            //Slope 1.3
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,

            //Slope 2.3
            1, 1, 1,
            1, 1, 1,

            //Top
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,
        },
        //11
        new List<int>{
            //Slope 1.1
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            //Slope 2.1
            1, 1, 1,
            1, 1, 1,

            //Slope 1.2
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,

            //Slope 2.2
            1, 1, 1,
            1, 1, 1,

            //Top
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            //side
            0, 0, 0,
            0, 0, 0,

            1, 1, 1,
            1, 1, 1,
        },
        //12
        new List<int>{
            //Top
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            //side
            0, 0, 0,
            0, 0, 0,

            1, 1, 1,
            1, 1, 1,

            //side
            0, 0, 0,
            0, 0, 0,

            1, 1, 1,
            1, 1, 1,

            //Slope 1
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            //Slope 2
            1, 1, 1,
            1, 1, 1,
        },
        //13
        new List<int>{
            //Top
            0, 0, 0,
            0, 0, 0,

            //side
            0, 0, 0,
            0, 0, 0,

            1, 1, 1,
            1, 1, 1,

            //side
            0, 0, 0,
            0, 0, 0,

            1, 1, 1,
            1, 1, 1,

            //side
            0, 0, 0,
            0, 0, 0,

            1, 1, 1,
            1, 1, 1,

            //side
            0, 0, 0,
            0, 0, 0,

            1, 1, 1,
            1, 1, 1,
        },
        //14
        new List<int>{
            //Slope 1.1
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            //Slope 2.1
            1, 1, 1,
            1, 1, 1,

            //Slope 1.2
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            //Slope 2.2
            1, 1, 1,
            1, 1, 1,

            //Slope 1.3
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            //Slope 2.3
            1, 1, 1,
            1, 1, 1,

            //Slope 1.4
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            //Slope 2.4
            1, 1, 1,
            1, 1, 1,

            //Top
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,
        },
        //15
        new List<int>{
            //Top
            0, 0, 0,
            0, 0, 0,

            0, 0, 0,
            0, 0, 0,

            //side
            0, 0, 0,
            0, 0, 0,

            1, 1, 1,
            1, 1, 1,

            //side
            0, 0, 0,
            0, 0, 0,

            1, 1, 1,
            1, 1, 1,

            //side
            0, 0, 0,
            0, 0, 0,

            1, 1, 1,
            1, 1, 1,

            //side
            0, 0, 0,
            0, 0, 0,

            1, 1, 1,
            1, 1, 1,
        }
    };


    //UVS are not use for now
    public static List<Vector2>[] shapesUV = new List<Vector2>[16] {
        //0
        new List<Vector2>{
            new Vector2(0f, 0f),
            new Vector2(0f, 1f),
            new Vector2(1f, 1f),
            new Vector2(0f, 0f),
            new Vector2(1f, 1f),
            new Vector2(1f, 0f)
        },
        //1
        new List<Vector2>{
            new Vector2(1f, 1f),
            new Vector2(1f, 1.751504f),
            new Vector2(1.248496f, 1.751504f),
            new Vector2(1f, 1f),
            new Vector2(1.248496f, 1.751504f),
            new Vector2(2f, 1f),

            new Vector3(2f, 1f),
            new Vector3(1.248496f, 1.751504f),
            new Vector3(1.248496f, 2f),
            new Vector3(2f, 1f),
            new Vector3(1.248496f, 2f),
            new Vector3(2f, 2f),

            new Vector3(2, 0) + new Vector3(2f, 1f),
            new Vector3(2, 0) + new Vector3(1.248496f, 1.751504f),
            new Vector3(2, 0) + new Vector3(1.248496f, 2f),
            new Vector3(2, 0) + new Vector3(2f, 1f),
            new Vector3(2, 0) + new Vector3(1.248496f, 2f),
            new Vector3(2, 0) + new Vector3(2f, 2f),

        },
        //2
        new List<Vector2>{
           
        },
        //3
        new List<Vector2>{
           
        },
        //4
        new List<Vector2>{
            
        },
        //5
        new List<Vector2>{
            
        },
        //6
        new List<Vector2>{
            //Empty because this ID was duplicate with 8
        },
        //7
        new List<Vector2>{
            
        },
        //8
        new List<Vector2>{
           

        },
        //9
        new List<Vector2>{
            

        },
        //10
        new List<Vector2>{
           
        },
        //11
        new List<Vector2>{
           
        },
        //12
        new List<Vector2>{
            
        },
        //13
        new List<Vector2>{
            
        },
        //14
        new List<Vector2>{
            
        },
        //15
        new List<Vector2>{
            
        }
    };

    public static List<Vector3>[] shapes = new List<Vector3>[16] {
        //0
        new List<Vector3>{
            new Vector3(0f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(1f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(1f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(1f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)
        },
        //1
        new List<Vector3>{

            new Vector3(0f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.248496f, 1, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(1f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f),

            new Vector3(1f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(1f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(1f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f),

            new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f),

            new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f),

            new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f),
            
            new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f),
        },
        //2
        new List<Vector3>{
            new Vector3(0f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f),

            new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.874464f, 0.877643f, 0f) - new Vector3(0.5f, 0, 0.5f),

            new Vector3(0.874464f, 0.877643f, 0f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.874464f, 0.877643f, 0f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(1f, 0f, 0f) - new Vector3(0.5f, 0, 0.5f) 
        },
        //3
        new List<Vector3>{
            //Slope 1 step
            new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f),

            new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f),

            //Slope 2 step 
            new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f),

            new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f),

            //Top plate
            new Vector3(0.248496f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(1f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.248496f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(1f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(1f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f),

            new Vector3(0f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.248496f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f),

            //Slope 1 step
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 2 step 
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
        },
        //4
        new List<Vector3>{
            //Top plate
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),

            //Side
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(1f, 0f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            
            //Slope 1 step
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 2 step 
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

        },
        //5
        new List<Vector3>{
            //Top plate
            (new Vector3(0f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0.248496f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),

            (new Vector3(0.248496f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0.248496f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),


            //Slope 1 step 1
            (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 2 step 1
            (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

             //Slope 1 step 2
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 2 step 2
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

        },
        //6
        new List<Vector3>{
            //Empty because this ID was duplicate with 8
        },
        //7
        new List<Vector3>{
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            

            //Side 1
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(1f, 0f, 0f) - new Vector3(0.5f, 0, 0.5f)),


            //Side 2 
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.751504f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.751504f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(1f, 0f, 0f) - new Vector3(0.5f, 0, 0.5f)),


        },
        //8
        new List<Vector3>{
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),

            //Side
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(1f, 0f, 0f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 1 step
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 2 step 
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f))
        },
        //9
        new List<Vector3>{
            //Side 1
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 0f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(1f, 0f, 0f) - new Vector3(0.5f, 0, 0.5f)),

            //Side 2
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 0f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(1f, 0f, 0f) - new Vector3(0.5f, 0, 0.5f)),

            //Top side
            new Vector3(0f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(1f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(1f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(1f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f),
        },
        //10
        new List<Vector3>{
            //Slope 1 step 1
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 2 step 1
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 0, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 1 step 2
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 2 step 2
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 1 step 3
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 2 step 3
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            //TOP side
            new Vector3(0f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f),
            new Vector3(0.248496f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f))
        },
        //11
        new List<Vector3>{
            //Slope 1 step 1
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 2 step 1
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 1 step 2
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 2 step 2
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            //Top side
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),

            //side
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 0f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(1f, 0f, 0f) - new Vector3(0.5f, 0, 0.5f)) 
        },
        //12
        new List<Vector3>{
            //Top side
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(1f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 0, 0) * (new Vector3(1f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(1f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),

            //Side 1
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.751504f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 0) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 0) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 0) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(1f, 0f, 0f) - new Vector3(0.5f, 0, 0.5f)),


            //Side 2 
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(1f, 0f, 0f) - new Vector3(0.5f, 0, 0.5f)),

             //Slope 1 step
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 2 step 
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
        },
        //13
        new List<Vector3>{
            //Top side
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(1f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(1f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(1f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),

            //Side 1
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.751504f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 0) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 0) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.874464f, 0.877643f, 0) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(1f, 0f, 0f) - new Vector3(0.5f, 0, 0.5f)),


            //Side 2 
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(1f, 0f, 0f) - new Vector3(0.5f, 0, 0.5f)),

            //Side 3
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.751504f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.751504f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(1f, 0f, 0f) - new Vector3(0.5f, 0, 0.5f)),
        },
        //14
        new List<Vector3>{
            //Slope 1 step 1
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 2 step 1
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 0, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 1 step 2
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 2 step 2
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, -90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, -90, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 1 step 3
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 2 step 3
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 1 step 4
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 90, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),

            //Slope 2 step 4
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.125536f, 0.877643f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 90, 0) * (new Vector3(0.125536f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            //TOP side
            Quaternion.Euler(0, 0, 0) * (new Vector3(0f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.248496f, 1f, 0f) - new Vector3(0.5f, 0, 0.5f))
        },
        //15
        new List<Vector3>{
            //Top side
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.751504f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.751504f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.751504f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.248496f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),

            //Side 1 
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.751504f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.751504f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.751504f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 0, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 0, 0) * (new Vector3(1f, 0f, 0f) - new Vector3(0.5f, 0, 0.5f)),

            //Side 2 
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.751504f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.751504f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.751504f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 90, 0) * (new Vector3(1f, 0f, 0f) - new Vector3(0.5f, 0, 0.5f)),

            //Side 3 
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.751504f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 180, 0) * (new Vector3(1f, 0f, 0f) - new Vector3(0.5f, 0, 0.5f)),

            //Side 4 
            Quaternion.Euler(0, 270, 0) * (new Vector3(0.751504f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 270, 0) * (new Vector3(0.751504f, 1f, 0.751504f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 270, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 270, 0) * (new Vector3(0.751504f, 1f, 0.248496f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 270, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 270, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),

            Quaternion.Euler(0, 270, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 270, 0) * (new Vector3(0.874464f, 0.877643f, 0.874464f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 270, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 270, 0) * (new Vector3(0.874464f, 0.877643f, 0.122357f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 270, 0) * (new Vector3(1f, 0f, 1f) - new Vector3(0.5f, 0, 0.5f)),
            Quaternion.Euler(0, 270, 0) * (new Vector3(1f, 0f, 0f) - new Vector3(0.5f, 0, 0.5f)),


        },
    };

}
