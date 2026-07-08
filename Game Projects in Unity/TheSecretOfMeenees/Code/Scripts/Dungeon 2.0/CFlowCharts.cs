using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// Class <c>CLoadPrefabs</c> contains definitions of the flows used by Dungeon generator 2.
/// </summary>
public class CFlowCharts : MonoBehaviour
{
    /// <value><c>m_Graph</c>: Graph of the flow chart.</value>
    private List<CNode> m_Graph;

    /// <summary>
    /// Loads the desired flow chart.
    /// </summary>
    /// <param name="graphIndex">Index of which chart should be loaded.</param>
    /// <returns>The desired flow chart in the form of a List.</returns>
    public List<CNode> LoadFlow ( int graphIndex )
    {
        m_Graph = new List<CNode>();

        switch ( graphIndex ) {
            case 0: { //===============================================================================================//
                // 6 room loop + trees
                m_Graph.Add ( new CNode ( "StartRoom" ) );
                m_Graph.Add ( new CNode ( "HubRoom" ) ); 
                m_Graph.Add ( new CNode ( "GenericRoom" ) ); 
                m_Graph.Add ( new CNode ( "HubRoom" ) );
                m_Graph.Add ( new CNode ( "GenericRoom" ) );
                m_Graph.Add ( new CNode ( "HubRoom" ) );
                m_Graph.Add ( new CNode ( "GenericRoom" ) ); 
                m_Graph.Add ( new CNode ( "GenericRoom" ) ); 
                m_Graph.Add ( new CNode ( "HubRoom" ) );
                m_Graph.Add ( new CNode ( "HubRoom" ) ); 
                m_Graph.Add ( new CNode ( "GenericRoom" ) ); 
                m_Graph.Add ( new CNode ( "GenericRoom" ) );
                m_Graph.Add ( new CNode ( "HubRoom" ) );
                m_Graph.Add ( new CNode ( "BossRoom" ) ); 

                m_Graph[0].m_Neighbors = new List<CNode> { m_Graph[1] };
                m_Graph[1].m_Neighbors = new List<CNode> { m_Graph[0], m_Graph[2], m_Graph[3] };
                m_Graph[2].m_Neighbors = new List<CNode> { m_Graph[1] };
                m_Graph[3].m_Neighbors = new List<CNode> { m_Graph[1], m_Graph[4], m_Graph[7] };
                m_Graph[4].m_Neighbors = new List<CNode> { m_Graph[3], m_Graph[5] };
                m_Graph[5].m_Neighbors = new List<CNode> { m_Graph[4], m_Graph[6], m_Graph[8] };
                m_Graph[6].m_Neighbors = new List<CNode> { m_Graph[5], m_Graph[7] };
                m_Graph[7].m_Neighbors = new List<CNode> { m_Graph[3], m_Graph[6] };
                m_Graph[8].m_Neighbors = new List<CNode> { m_Graph[5], m_Graph[9], m_Graph[12] };
                m_Graph[9].m_Neighbors = new List<CNode> { m_Graph[8], m_Graph[10], m_Graph[11] };
                m_Graph[10].m_Neighbors = new List<CNode> { m_Graph[9] };
                m_Graph[11].m_Neighbors = new List<CNode> { m_Graph[9] };
                m_Graph[12].m_Neighbors = new List<CNode> { m_Graph[8], m_Graph[13] };
                m_Graph[13].m_Neighbors = new List<CNode> { m_Graph[12] };
            }; break;
            case 1: { //===============================================================================================//
                // 6 room loop
                m_Graph.Add ( new CNode ( "StartRoom" ) );
                m_Graph.Add ( new CNode ( "HubRoom" ) ); 
                m_Graph.Add ( new CNode ( "GenericRoom" ) ); 
                m_Graph.Add ( new CNode ( "HubRoom" ) );
                m_Graph.Add ( new CNode ( "GenericRoom" ) );
                m_Graph.Add ( new CNode ( "GenericRoom" ) );
                m_Graph.Add ( new CNode ( "GenericRoom" ) ); 
                m_Graph.Add ( new CNode ( "GenericRoom" ) ); 
                m_Graph.Add ( new CNode ( "BossRoom" ) ); 

                m_Graph[0].m_Neighbors = new List<CNode> { m_Graph[1] };
                m_Graph[1].m_Neighbors = new List<CNode> { m_Graph[0], m_Graph[2], m_Graph[6] };
                m_Graph[2].m_Neighbors = new List<CNode> { m_Graph[1], m_Graph[3] };
                m_Graph[3].m_Neighbors = new List<CNode> { m_Graph[2], m_Graph[4], m_Graph[7] };
                m_Graph[4].m_Neighbors = new List<CNode> { m_Graph[3], m_Graph[5] };
                m_Graph[5].m_Neighbors = new List<CNode> { m_Graph[4], m_Graph[6] };
                m_Graph[6].m_Neighbors = new List<CNode> { m_Graph[1], m_Graph[5] };
                m_Graph[7].m_Neighbors = new List<CNode> { m_Graph[3], m_Graph[8] };
                m_Graph[8].m_Neighbors = new List<CNode> { m_Graph[7] };
            }; break;
            case 2: { //===============================================================================================//
                // 5 room loop
                m_Graph.Add ( new CNode ( "StartRoom" ) );
                m_Graph.Add ( new CNode ( "HubRoom" ) ); 
                m_Graph.Add ( new CNode ( "GenericRoom" ) ); 
                m_Graph.Add ( new CNode ( "HubRoom" ) );
                m_Graph.Add ( new CNode ( "GenericRoom" ) );
                m_Graph.Add ( new CNode ( "GenericRoom" ) ); 
                m_Graph.Add ( new CNode ( "GenericRoom" ) ); 
                m_Graph.Add ( new CNode ( "BossRoom" ) ); 

                m_Graph[0].m_Neighbors = new List<CNode> { m_Graph[1] };
                m_Graph[1].m_Neighbors = new List<CNode> { m_Graph[0], m_Graph[2], m_Graph[5] };
                m_Graph[2].m_Neighbors = new List<CNode> { m_Graph[1], m_Graph[3] };
                m_Graph[3].m_Neighbors = new List<CNode> { m_Graph[2], m_Graph[4], m_Graph[6] };
                m_Graph[4].m_Neighbors = new List<CNode> { m_Graph[3], m_Graph[5] };
                m_Graph[5].m_Neighbors = new List<CNode> { m_Graph[1], m_Graph[4] };
                m_Graph[6].m_Neighbors = new List<CNode> { m_Graph[3], m_Graph[7] };
                m_Graph[7].m_Neighbors = new List<CNode> { m_Graph[6] };
            }; break;
            case 3: { //===============================================================================================//
                // 5 and 4 room loops, EtG copy
                m_Graph.Add ( new CNode ( "StartRoom" ) );
                m_Graph.Add ( new CNode ( "HubRoom" ) ); 
                m_Graph.Add ( new CNode ( "GenericRoom" ) ); 
                m_Graph.Add ( new CNode ( "HubRoom" ) );
                m_Graph.Add ( new CNode ( "GenericRoom" ) );

                m_Graph.Add ( new CNode ( "GenericRoom" ) );
                m_Graph.Add ( new CNode ( "HubRoom" ) ); 
                m_Graph.Add ( new CNode ( "GenericRoom" ) ); 
                m_Graph.Add ( new CNode ( "BossRoom" ) );
                m_Graph.Add ( new CNode ( "HubRoom" ) ); 

                m_Graph.Add ( new CNode ( "GenericRoom" ) ); 
                m_Graph.Add ( new CNode ( "GenericRoom" ) );
                m_Graph.Add ( new CNode ( "GenericRoom" ) );
                m_Graph.Add ( new CNode ( "GenericRoom" ) );
                m_Graph.Add ( new CNode ( "GenericRoom" ) );

                m_Graph[0].m_Neighbors = new List<CNode> { m_Graph[1] };
                m_Graph[1].m_Neighbors = new List<CNode> { m_Graph[0], m_Graph[2], m_Graph[5] };
                m_Graph[2].m_Neighbors = new List<CNode> { m_Graph[1], m_Graph[3] };
                m_Graph[3].m_Neighbors = new List<CNode> { m_Graph[2], m_Graph[4], m_Graph[6] };
                m_Graph[4].m_Neighbors = new List<CNode> { m_Graph[3], m_Graph[5] };

                m_Graph[5].m_Neighbors = new List<CNode> { m_Graph[1], m_Graph[4] };
                m_Graph[6].m_Neighbors = new List<CNode> { m_Graph[3], m_Graph[7], m_Graph[9] };
                m_Graph[7].m_Neighbors = new List<CNode> { m_Graph[6], m_Graph[8] };
                m_Graph[8].m_Neighbors = new List<CNode> { m_Graph[7] };
                m_Graph[9].m_Neighbors = new List<CNode> { m_Graph[6], m_Graph[10], m_Graph[12], m_Graph[13] };

                m_Graph[10].m_Neighbors = new List<CNode> { m_Graph[9], m_Graph[11] };
                m_Graph[11].m_Neighbors = new List<CNode> { m_Graph[10], m_Graph[12] };
                m_Graph[12].m_Neighbors = new List<CNode> { m_Graph[9], m_Graph[11] };
                m_Graph[13].m_Neighbors = new List<CNode> { m_Graph[9], m_Graph[14] };
                m_Graph[14].m_Neighbors = new List<CNode> { m_Graph[13] };
            }; break;
        }

        return m_Graph;
    }

    void Start()
    {
    }

    void Update()
    {
    }
}
