using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// Class <c>CCoordinates</c> holds information about a 2D position.
/// </summary>
public class CCoordinates
{
    /// <value><c>m_X</c>: Position X.</value>
    public int m_X { get; set; }
    /// <value><c>m_Y</c>: Position Y.</value>
    public int m_Y { get; set; }

    //==================================================================================================================//

    /// <summary>
    /// Default constructor.
    /// </summary>
    public CCoordinates()
    {
    }

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="x">Position X.</param>
    /// <param name="y">Position Y.</param>
    public CCoordinates ( int x, int y )
    {
        this.m_X = x;
        this.m_Y = y;
    }
};
