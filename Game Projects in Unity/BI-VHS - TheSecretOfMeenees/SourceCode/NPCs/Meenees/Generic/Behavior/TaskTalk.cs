using BehaviorTree;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.AI;

public class TaskTalk : Node
{
    //Here we have to use a bit of fuzzy logic,  playfulness and tiredness
    private Transform _transform;
    private NavMeshAgent _agent;
    private Animator _animator;
    private List<string> topic;
    private BillboardCanvas _mySpeakingSpace;
    private float _wait;

    public TaskTalk(Transform myTransform, BillboardCanvas speakingSpace)
    {
        _transform = myTransform;
        _agent = _transform.GetComponent<NavMeshAgent>();
        _animator = _transform.GetComponent<Animator>();
        _mySpeakingSpace = speakingSpace;
    }

    public override NodeState Evaluate()
    {
        Transform t = (Transform)GetData("ConversationPartner");
        if (t == null)
        {
            state = NodeState.FAILURE;
            return state;
        }
        //establish topic (ie. load in the text strings to the objects)  ONCE
        else if(topic == null)
        {
            //we need to know if its a Meenee or player

            if(t.gameObject.layer == LayerMask.NameToLayer("Player"))
            {
                //get the layer of the object, on player we use TalkAt for topic
                Dialogues talkData = (Dialogues)GetData("Dialogues");
                topic = talkData.onlyTalkAts[Random.Range(0, talkData.onlyTalkAts.Length-1)].talky; //we get a random one-liner

                _wait = topic[0].Length * 0.1f + 1f;
                _mySpeakingSpace.GetText(topic[0], _wait);
                topic.RemoveAt(0); //topic with 0 elems != null thankfully
                _transform.LookAt(t);
                state = NodeState.RUNNING;
                return state;
            }
            else
            {
                //we have an NPC convo partner and topic is NULL-> force a topic onto them
                //but first, we check if we didnt have a topic forced onto us
                topic = (List<string>)GetData("Topic");

                if(topic == null)
                {//establish topic
                    Dialogues talkData = (Dialogues)GetData("Dialogues");
                    List<List<string>> allTopics = talkData.dialogues.Where(obj => obj.elementSecond == t.gameObject.GetComponent<MeeneeBT>().element).Select(obj => obj.talky).ToList();
                    topic = new List<string>(allTopics[Random.Range(0, allTopics.Count-1)]);
                    List<string> topicCopyForce = new List<string>(topic);
                    for(int i = 0; i < topic.Count; i+=2)
                    {
                        topicCopyForce[i] = new string(' ', topicCopyForce[i].Length);
                    }
                    for (int i = 1; i < topic.Count; i += 2)
                    {
                        topic[i] = new string(' ', topicCopyForce[i].Length);
                    }
                    t.gameObject.GetComponent<MeeneeBT>().ForceTopic(topicCopyForce);
                }
            }
        }
        else
        {
            if (_mySpeakingSpace.IsTalking())
            {
                state = NodeState.RUNNING;
                return state;
            }
            else
            {
                if(topic.Count == 0)
                {
                    //Debug.Log("Cleared partner");
                    parent.parent.SetData("ConversationNon", t);
                    ClearData("ConversationPartner");
                    ClearData("Topic");
                    topic = null;
                    parent.parent.SetData("Energy", (int)GetData("Energy") - 10);
                    state = NodeState.FAILURE;
                    return state;
                }
                
            }
        }
        _transform.LookAt(t);
        _wait = topic[0].Length * 0.1f +0.5f;
        _mySpeakingSpace.GetText(topic[0], _wait);
        topic.RemoveAt(0);
        state = NodeState.RUNNING;
        return state;
    }
}
