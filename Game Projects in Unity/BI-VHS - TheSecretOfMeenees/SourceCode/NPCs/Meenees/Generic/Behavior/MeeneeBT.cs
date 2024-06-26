using System.Collections;
using System.Collections.Generic;
using System.Xml.Linq;
using BehaviorTree;
using Unity.AI.Navigation;
using UnityEngine;
using UnityEngine.AI;

public class MeeneeBT : BTree
{
    public UnityEngine.Transform myHome;
    public GameObject POI_Container;
    public UnityEngine.Transform[] POI; //this should get loaded from somewhere,
                                        //especially since the POI
                                        //might change as the game progresses

    private NavMeshAgent agent;
    public MeeneeDataSO meeneeData;
    public string element;
    public Dialogues meeneeDialogues;
    public BillboardCanvas _mySpeakingSpace;

    public int energyCurr;
    public int playfulnessCurr;

    protected override Node SetupTree()
    {
        POI_Container = GameObject.FindWithTag("NPCPOI");
        POI = new Transform[POI_Container.transform.childCount];
        for(int i = 0; i < POI_Container.transform.childCount; i++)
        {
            POI[i] = POI_Container.transform.GetChild(i).transform;
        }
        energyCurr = Random.Range(180, meeneeData.maxEnergy);
        playfulnessCurr = Random.Range(180, meeneeData.maxPlayfulness);
        agent = gameObject.GetComponent<NavMeshAgent>();
        agent.speed = meeneeData.speed;
        element = meeneeData.meeneeElement;

        Node root = new Selector(new List<Node>
        {
            // new TaskGoVisit(transform, LOCATION, agent)
            // new TaskWork()

            //task - check busy - ie., dead, in forced covo etc.

            new Sequence ( new List<Node>{
              new TaskInteractInRange(transform, meeneeData.whatIsInteractable),
              new TaskGoToTarget(transform, agent, 3.0f),
              new TaskTalk(transform, _mySpeakingSpace)
              //play?
            }),
             new TaskWander(transform, agent, POI),
             new Sequence( new List<Node>
             {
                 new TaskGoToTarget(transform, myHome, agent, 5.0f),
                 new TaskRest(gameObject),
             })
             
        });
        //set agent position somewhere according to our current enery and playfulness

        root.SetData("meeneeData", meeneeData);
        root.SetData("Energy", energyCurr);
        root.SetData("Playfulness", playfulnessCurr);
        root.SetData("Dialogues", meeneeDialogues);

        _mySpeakingSpace.SetSpeedAndPitch(meeneeData.talkingSpeed, meeneeData.talkingPitch);

        return root;
    }
    /**
     * Returns true if we want to talk and sets them as conversation partner
     * else returns false
     */
    public bool WantsToTalk(Transform t)
    {
        int paramCurr = (int)_root.GetData("Energy"), paramHigh = 200;
        if (_root.fuzzyWants(paramCurr, paramHigh) && t != (Transform)_root.GetData("ConversationNon") && _root.GetData("ConversationPartner") == null)
        {
            _root.SetData("ConversationPartner", t);
            return true;
        }
        else _root.SetData("ConversationNon", t);
        return false;
    }

    public void ForceTopic(List<string> topic)
    {
        _root.SetData("Topic", topic);
    }
}
