using UnityEngine;
using BehaviorTree;
using UnityEngine.AI;


public class TaskWander : Node
{
    //private ScriptableObject _
    private Transform[] _POI;
    private Transform _pos, _CurrDestination;
    NavMeshAgent _agent;
    //So, there have to be points of Interest in the village - Like the NPC houses, the Crucible, etc.
    //Where and how do we store those? Every Meenee has to have access to them.
    public TaskWander(Transform transform, NavMeshAgent agent, Transform[] poi)
    {
        _pos = transform;
        _POI = poi;
        _agent = agent;
    }
    public override NodeState Evaluate()
    {
        int en = (int)GetData("Energy"), play = (int)GetData("Playfulness");
        if(en < 50 )//|| play < 50) //noodle code
        {
            state = NodeState.FAILURE;
            return state;
        }
        else if (children.Count == 0)
        {
            _CurrDestination = _POI[Random.Range(0, _POI.Length-1)];
            _Attach(new TaskGoToTarget(_pos, _CurrDestination, _agent, Random.Range(1.0f, 3f)));
            state = NodeState.RUNNING;
            return state;
        }
        else
        {
            foreach (Node node in children)
            {
                switch (node.Evaluate())
                {
                    case NodeState.FAILURE:
                        state = NodeState.FAILURE;
                        break;
                    case NodeState.SUCCESS:
                        state = NodeState.SUCCESS;
                        break;
                    case NodeState.RUNNING:
                        state = NodeState.RUNNING;
                        return state;
                    default:
                        continue;
                }
            }
            parent.SetData("Energy", en -1);
            _DetachAll();
        }
        state = NodeState.SUCCESS;
        return state;
    }
}
