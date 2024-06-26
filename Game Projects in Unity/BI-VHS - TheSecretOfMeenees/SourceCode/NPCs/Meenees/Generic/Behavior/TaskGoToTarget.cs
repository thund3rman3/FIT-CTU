using UnityEngine;
using BehaviorTree;
using UnityEngine.AI;

/*
 * Goes to the target using the NavMeshAgent
 * Stops when it is closer than HowFarOkay
 * 
 */
public class TaskGoToTarget : Node
{
    private Transform _transform;
    private Transform _target;
    private Animator _animator;
    private NavMeshAgent _agent;
    private float _distance;

    private bool _nullTarget;

    
    public TaskGoToTarget(Transform transform, Transform target, NavMeshAgent agent, float howFarOkay)
    {
        _transform = transform;
        _target = target;
        _nullTarget = false;
        _agent = agent;
        _animator = _transform.GetComponent<Animator>();
        _distance = howFarOkay;
    }
    public TaskGoToTarget(Transform transform, NavMeshAgent agent, float howFarOkay)
    {
        _transform = transform;
        _agent = agent;
        _nullTarget = true;
        _animator = _transform.GetComponent<Animator>();
        _distance = howFarOkay;
    }

    public override NodeState Evaluate()
    {
        if(_nullTarget)
        {
            _target = (Transform)GetData("ConversationPartner");
            //Debug.Log(_target);
        }
        //set up a failure to reach here, the Meenee bugs out when he cant get within the distance of the target
        if (Vector3.Distance(_transform.position, _target.position) <= _distance)
        {
            state = NodeState.SUCCESS;
            _agent.ResetPath();
            if (_nullTarget) _target = null;
            _animator.SetBool("Walking", false);
        }
        else if (!(_agent.hasPath || _agent.pathPending) && Vector3.Distance(_transform.position, _target.position) > _distance)
        {
            _agent.SetDestination(_target.position);
            _animator.SetBool("Walking", true);
           // Debug.Log("Calculated path to target");
           // Debug.Log(_target.position);
            state = NodeState.RUNNING;
        }
        else if (_agent.hasPath || _agent.pathPending)
        {
            state = NodeState.RUNNING;
        }
        else state = NodeState.FAILURE;
        return state;

    }
}
