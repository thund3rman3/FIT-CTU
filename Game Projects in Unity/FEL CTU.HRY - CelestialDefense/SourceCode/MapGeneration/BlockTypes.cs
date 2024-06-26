using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BlockType
{
    public int ID;
    //Top color, bottom color
    public Color[] colors = new Color[2];

    public BlockType(int ID, Color topColor, Color bottomColor)
    {
        this.ID = ID;
        this.colors[0] = topColor;
        this.colors[1] = bottomColor;
    }
}

// 0 - Water
// 1 - Volcano
// 2 - Plains
// 3 - Beach
// 4 - Forest
// 5 - mountain
// 6 - Wasteland
// 7 - Burned forest

public class BlockLibrary
{
    
    public static BlockType[] blockTypes = new BlockType[11]
    {
        //Air/void
        new BlockType(0, new Color(), new Color()),
        
        //Grass
        new BlockType(1, new Color(0.219f, 0.6f, 0.160f), new Color(0.505f, 0.290f, 0.070f)),
        //Grass darker
        new BlockType(2, new Color(0.149f, 0.458f, 0.082f), new Color(0.505f, 0.290f, 0.070f)),
        //Dirt
        new BlockType(3,new Color(0.505f, 0.290f, 0.070f), new Color(0.505f, 0.290f, 0.070f)),
        //Burned grass
        new BlockType(4, new Color(0.254f, 0.290f, 0.250f), new Color(0.188f, 0.156f, 0.090f)),
        //new BlockType(4, new Color(0.219f, 0.294f, 0.207f), new Color(0.188f, 0.156f, 0.090f)),
        //Sand
        new BlockType(5, new Color(0.894f, 0.858f, 0.247f), new Color(0.894f, 0.858f, 0.247f)),
        //Stone
        new BlockType(6, new Color(0.541f, 0.541f, 0.541f), new Color(0.541f, 0.541f, 0.541f)),
        //Stone 2
        new BlockType(7, new Color(0.309f, 0.309f, 0.309f), new Color(0.309f, 0.309f, 0.309f)),
        //Lava stone
        new BlockType(8, new Color(0.149f, 0.149f, 0.149f), new Color(0.050f, 0.050f, 0.050f)),
        //Wasteland dirt
        new BlockType(9, new Color(0.674f, 0.627f, 0.325f), new Color(0.674f, 0.627f, 0.325f)),
    
        //Lava
        new BlockType(10, new Color(204/255f,18/255f,28/255f), new Color(204/255f,18/255f,28/255f))

    };

}
